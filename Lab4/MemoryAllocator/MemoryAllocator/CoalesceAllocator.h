#pragma once
#include <windows.h>

#ifdef _DEBUG
#include <cassert>
#define assert_msg(exp, msg) assert(((void)msg, exp))
#endif

class CoalesceAllocator
{
public:
	CoalesceAllocator(size_t bufferSize);
	~CoalesceAllocator();
	virtual void init();
	virtual void destroy();
	virtual void* alloc(size_t size);
	virtual void free(void* p);
#ifdef _DEBUG
	virtual void dumpStat() const;
	virtual void dumpBlocks() const;
#endif
	virtual bool contains(void* p);

private:
	struct Buffer {
		void* freeHeadBlock;
		Buffer* next;
		void* blocks;
	};

	struct FreeBlockInfo
	{
		void* prevBlock; // Prev block in free list
		void* nextBlock; // Next block in free list

		void removeFromFreeList()
		{
			if (prevBlock != nullptr)
			{
				FreeBlockInfo* pbInfo = static_cast<Block*>(prevBlock)->getFreeBlockInfo();
				pbInfo->nextBlock = nextBlock;
			}

			if (nextBlock != nullptr)
			{
				FreeBlockInfo* nbInfo = static_cast<Block*>(nextBlock)->getFreeBlockInfo();
				nbInfo->prevBlock = prevBlock;
			}
		}
	};

	struct Block
	{

#ifdef _DEBUG
		size_t startMarker;
#endif
		bool isFree;
		void* prevBlock; // Prev block in buffer
		void* nextBlock; // Next block in buffer
		size_t size; // Full size with metadata
		void* data; // User data or FreeBlockInfo if this block is free

#ifdef _DEBUG
		size_t endMarker;

		void clearMarkers()
		{
			startMarker = 0;
			endMarker = 0;
		}
#endif


		size_t getPayloadSize()
		{
			return size - sizeof(Block);
		}

		FreeBlockInfo* getFreeBlockInfo()
		{
			return static_cast<FreeBlockInfo*>(this->data);
		}

	};

#ifdef _DEBUG
	bool isInit;
	size_t usedBlocksCount;

	const size_t kMarker = 12345;
#endif

	Buffer* m_firstBuffer;
	size_t m_BufferSize;

	Buffer* allocBuffer();
	void freeBuffer(Buffer* buf);
	Buffer* getPointerLocation(void* p);
};


CoalesceAllocator::CoalesceAllocator(size_t bufferSize)
{
	m_BufferSize = bufferSize + sizeof(Block);
}

CoalesceAllocator::~CoalesceAllocator()
{
#ifdef _DEBUG
	assert_msg(!isInit, "Allocator not destroyed before destructor call!");
#endif
}

void CoalesceAllocator::init()
{

#ifdef _DEBUG
	assert_msg(!isInit, "Allocator already initialized!");
	isInit = true;
	usedBlocksCount = 0;
#endif

	m_firstBuffer = allocBuffer();
}

void CoalesceAllocator::destroy()
{
#ifdef _DEBUG
	assert_msg(isInit, "Allocator not initialized!");
	assert_msg(usedBlocksCount == 0, "Some memory blocks were not free!");
	isInit = false;
#endif
	Buffer* buf = m_firstBuffer;

	while (buf != nullptr)
	{
		Buffer* prev = buf;
		buf = buf->next;
		freeBuffer(prev);
	}

	m_firstBuffer = nullptr;
}

void* CoalesceAllocator::alloc(size_t size)
{
#ifdef _DEBUG
	assert_msg(isInit, "Allocator not initialized!");
#endif

	Buffer* buffer = m_firstBuffer;
	Block* freeBlock = static_cast<Block*>(m_firstBuffer->freeHeadBlock);

	while (freeBlock == nullptr || freeBlock->getPayloadSize() < size)
	{
		// If buffer not contains require blocks
		if (freeBlock == nullptr)
		{
			if (buffer->next == nullptr)
			{
				buffer->next = allocBuffer();
			}

			buffer = buffer->next;
			freeBlock = static_cast<Block*>(buffer->freeHeadBlock);
		}
		else
		{
			freeBlock = static_cast<Block*>(freeBlock->getFreeBlockInfo()->nextBlock);
		}
	}

	void* nextFreeBlock;

	freeBlock->isFree = false;

	// If free space after create fill block will not be enough for another block
	// Create fill block, remove block from free list
	if (freeBlock->getPayloadSize() <= size + sizeof(Block) + sizeof(FreeBlockInfo))
	{
		nextFreeBlock = freeBlock->getFreeBlockInfo()->nextBlock;
		freeBlock->getFreeBlockInfo()->removeFromFreeList();
	}
	// Split block on filled and free blocks
	else 
	{

		Block* newFreeBlock = static_cast<Block*>(static_cast<void*>(static_cast<byte*>(freeBlock->data) + size));

		newFreeBlock->isFree = true;
		newFreeBlock->prevBlock = freeBlock;
		newFreeBlock->nextBlock = freeBlock->nextBlock;
		newFreeBlock->data = static_cast<byte*>(static_cast<void*>(newFreeBlock)) + sizeof(Block);

#ifdef _DEBUG
		newFreeBlock->startMarker = kMarker;
		newFreeBlock->endMarker = kMarker;
#endif

		if (freeBlock->nextBlock != nullptr)
		{
			static_cast<Block*>(freeBlock->nextBlock)->prevBlock = newFreeBlock;
		}

		newFreeBlock->size = freeBlock->size - size - sizeof(Block);

		FreeBlockInfo* blockInfo = freeBlock->getFreeBlockInfo();
		FreeBlockInfo* newBlockInfo = newFreeBlock->getFreeBlockInfo();

		newBlockInfo->nextBlock = blockInfo->nextBlock;
		newBlockInfo->prevBlock = blockInfo->prevBlock;

		if (blockInfo->prevBlock != nullptr)
		{
			static_cast<Block*>(blockInfo->prevBlock)->getFreeBlockInfo()->nextBlock = newFreeBlock;
		}

		if (blockInfo->nextBlock != nullptr)
		{
			static_cast<Block*>(blockInfo->nextBlock)->getFreeBlockInfo()->prevBlock = newFreeBlock;
		}

		freeBlock->nextBlock = newFreeBlock;
		freeBlock->size = size + sizeof(Block);

		nextFreeBlock = newFreeBlock;
	}

	// Change head of free list if block was head
	if (static_cast<void*>(freeBlock) == buffer->freeHeadBlock)
	{
		// Set head of free list to next free block
		buffer->freeHeadBlock = nextFreeBlock;
	}

#ifdef _DEBUG
	usedBlocksCount++;
#endif

	return freeBlock->data;
}

void CoalesceAllocator::free(void* p)
{
#ifdef _DEBUG
	assert_msg(isInit, "Allocator not initialized!");
#endif

	Buffer* buffer = getPointerLocation(p);

	if (buffer != nullptr)
	{
		Block* block = static_cast<Block*>(static_cast<void*>(static_cast<byte*>(p) - sizeof(Block)));

#ifdef _DEBUG
		assert_msg(block->startMarker == kMarker, "Coalesce:Memory corrupt detected!");
		assert_msg(block->endMarker == kMarker, "Coalesce:Memory corrupt detected!");
#endif
		// Append block to left neighbour
		if (block->prevBlock != nullptr && static_cast<Block*>(block->prevBlock)->isFree)
		{
			Block* prevBlock = static_cast<Block*>(block->prevBlock);

			prevBlock->nextBlock = block->nextBlock;
			prevBlock->size = prevBlock->size + block->size;

#ifdef _DEBUG
			block->clearMarkers(); // Delete markers of block
#endif
			block = prevBlock;
		}
		// If block hasnt free left neighbour add block to free list
		else
		{
			FreeBlockInfo* info = block->getFreeBlockInfo();
			info->prevBlock = nullptr;
			info->nextBlock = buffer->freeHeadBlock;

			if (buffer->freeHeadBlock != nullptr)
			{
				static_cast<Block*>(buffer->freeHeadBlock)->getFreeBlockInfo()->prevBlock = block;
			}

			buffer->freeHeadBlock = block;
		}


		// Append right neighbour to block
		if (block->nextBlock != nullptr && static_cast<Block*>(block->nextBlock)->isFree)
		{
			Block* nextBlock = static_cast<Block*>(block->nextBlock);
			nextBlock->getFreeBlockInfo()->removeFromFreeList(); // remove right neighbour from free list

#ifdef _DEBUG
			nextBlock->clearMarkers(); // Delete markers of block
#endif
			block->nextBlock = nextBlock->nextBlock;
			block->size = block->size + nextBlock->size;
		}

		block->isFree = true;

#ifdef _DEBUG
		usedBlocksCount--;
#endif
	}
}

CoalesceAllocator::Buffer* CoalesceAllocator::allocBuffer()
{
	void* pBuf = VirtualAlloc(NULL, sizeof(Buffer) + m_BufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	Buffer* newBuffer = static_cast<Buffer*>(pBuf);

	newBuffer->next = nullptr;
	newBuffer->blocks = static_cast<byte*>(pBuf) + sizeof(Buffer);
	newBuffer->freeHeadBlock = newBuffer->blocks;

	Block* block = static_cast<Block*>(newBuffer->blocks);
	block->isFree = true;
	block->prevBlock = nullptr;
	block->nextBlock = nullptr;
	block->size = m_BufferSize;
	block->data = static_cast<byte*>(newBuffer->blocks) + sizeof(Block);

#ifdef _DEBUG
	block->startMarker = kMarker;
	block->endMarker = kMarker;
#endif

	FreeBlockInfo* info = block->getFreeBlockInfo();
	info->prevBlock = nullptr;
	info->nextBlock = nullptr;

	return newBuffer;
}

#ifdef _DEBUG
void CoalesceAllocator::dumpStat() const
{
	assert_msg(isInit, "Allocator not initialized!");


	Buffer* buf = m_firstBuffer;

	while (buf != nullptr)
	{
		std::cout << "Buffer:" << buf << std::endl;
		size_t freeCount = 0;
		size_t usedCount = 0;

		Block* block = static_cast<Block*>(buf->blocks);

		while (block != nullptr)
		{
			if (block->isFree) 
			{
				freeCount++;
			}
			else
			{
				usedCount++;
			}

			block = static_cast<Block*>(block->nextBlock);
		}

		std::cout << "Used:" << usedCount << std::endl;
		std::cout << "Free:" << freeCount << std::endl;
		std::cout << std::endl;

		buf = buf->next;
	}
}

void CoalesceAllocator::dumpBlocks() const
{

	assert_msg(isInit, "Allocator not initialized!");

	Buffer* buf = m_firstBuffer;

	while (buf != nullptr)
	{
		std::cout << "Buffer:" << buf << std::endl;

		Block* block = static_cast<Block*>(buf->blocks);

		while (block != nullptr)
		{
			std::cout << "Block:" << block;
			std::cout << " FullSize:" << block->size;
			std::cout << " DataSize:" << block->getPayloadSize();
			std::cout << " Status:" << (block->isFree ? "FREE" : "USED");
			std::cout << " Prev:" << block->prevBlock << " Next:" << block->nextBlock;
			std::cout << std::endl;

			block = static_cast<Block*>(block->nextBlock);
		}

		std::cout << std::endl;

		buf = buf->next;
	}
}
#endif

bool CoalesceAllocator::contains(void* p)
{
	return getPointerLocation(p) != nullptr;
}


void CoalesceAllocator::freeBuffer(Buffer* buffer)
{
	if (buffer != nullptr)
	{
		VirtualFree(static_cast<void*>(buffer), 0, MEM_RELEASE);
	}
}

CoalesceAllocator::Buffer* CoalesceAllocator::getPointerLocation(void* p)
{
#ifdef _DEBUG
	assert_msg(isInit, "Allocator not initialized!");
#endif
	Buffer* buffer = m_firstBuffer;

	while (buffer != nullptr)
	{
		if (static_cast<void*>(buffer->blocks) <= p && static_cast<void*>(static_cast<byte*>(buffer->blocks) + m_BufferSize) > p)
		{
			return buffer;
		}

		buffer = buffer->next;
	}

	return nullptr;
}

