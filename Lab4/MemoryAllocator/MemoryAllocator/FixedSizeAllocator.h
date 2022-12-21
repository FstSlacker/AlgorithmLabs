#pragma once
#include <windows.h>
#include <cassert>

class FixedSizeAllocator {
public:
	FixedSizeAllocator();
	~FixedSizeAllocator();
	void init(size_t blockSize, size_t blocksCountInBuffer);
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
		size_t initBlocksCount;
		size_t freeHeadBlock;
		Buffer* next;
		void* blocks;
	};

	size_t m_blockSize;
	size_t m_blocksCountInBuffer;
	Buffer* m_firstBuffer;

#ifdef DEBUG
	bool isInit;
	bool isDestroyed;
	size_t usedBlocksCount;

	const size_t kMarker = 12345;
#endif

	Buffer* allocBuffer();
	void freeBuffer(Buffer* buf);
	Buffer* getPointerLocation(void* p);
};

FixedSizeAllocator::FixedSizeAllocator() 
{
	//...
}

FixedSizeAllocator::~FixedSizeAllocator()
{
	destroy();
}

void FixedSizeAllocator::init(size_t blockSize, size_t blocksCountInBuffer)
{

#ifdef DEBUG
	isInit = true;
	isDestroyed = false;
	m_blockSize = blockSize + sizeof(size_t); // Add marker size if debug mode
#else
	m_blockSize = blockSize;
#endif

	m_blocksCountInBuffer = blocksCountInBuffer;
	m_firstBuffer = allocBuffer();

}

void FixedSizeAllocator::destroy() 
{
#ifdef DEBUG
	assert(!isInit, "Allocator not initialized!");
#endif // DEBUG
		
	Buffer* buf = m_firstBuffer;

	while (buf != nullptr)
	{
		Buffer* prev = buf;
		buf = buf->next;
		freeBuffer(prev);
	}

	m_firstBuffer = nullptr;
}

void* FixedSizeAllocator::alloc(size_t size)
{

#ifdef DEBUG
	assert(!isInit, "Allocator not initialized!");
#endif

	Buffer* buffer = m_firstBuffer;

	while (buffer->freeHeadBlock > m_blocksCountInBuffer - 1)
	{
		if (buffer->next == nullptr) 
		{
			buffer->next = allocBuffer();
		}

		buffer = buffer->next;
	}

	void* p = static_cast<byte*>(buffer->blocks) + buffer->freeHeadBlock * m_blockSize;

#ifdef DEBUG
	*static_cast<size_t*>(static_cast<void*>(static_cast<byte*>(p) + (m_blockSize - sizeof(size_t)))) = kMarker;
#endif

	if (buffer->freeHeadBlock >= buffer->initBlocksCount) 
	{
		buffer->initBlocksCount++;
		buffer->freeHeadBlock++;
	}
	else 
	{
		buffer->freeHeadBlock = *static_cast<size_t*>(p);
	}


#ifdef DEBUG
	usedBlocksCount++;
#endif

	return p;
}

void FixedSizeAllocator::free(void* p)
{

#ifdef DEBUG
	assert(!isInit, "Allocator not initialized!");
#endif

	Buffer* buffer = getPointerLocation(p);

	if (buffer != nullptr)
	{

#ifdef DEBUG
		size_t marker = *static_cast<size_t*>(static_cast<void*>(static_cast<byte*>(p) + (m_blockSize - sizeof(size_t))));
		assert(marker == kMarker, "FSA:Memory corrupt detected!");
#endif
		*static_cast<size_t*>(p) = buffer->freeHeadBlock;
		buffer->freeHeadBlock = static_cast<size_t>((static_cast<byte*>(p) - static_cast<byte*>(buffer->blocks)) / m_blockSize);

#ifdef DEBUG
		usedBlocksCount--;
#endif

	}
}

#ifdef DEBUG
void FixedSizeAllocator::dumpStat() const
{


	assert(!isInit, "Allocator not initialized!");

	Buffer* buffer = m_firstBuffer;
	int bufferCounter = 0;

	while (buffer != nullptr)
	{
		std::cout << "Buffer " << bufferCounter << ": address:" << buffer;
		std::cout << ", size:" << m_blockSize * m_blocksCountInBuffer << " bytes" << std::endl;
		std::cout << "Not init blocks count:" << m_blocksCountInBuffer - buffer->initBlocksCount << std::endl;

		size_t freeInd = buffer->freeHeadBlock;
		size_t freeBlocksCount = 0;
		while (freeInd < buffer->initBlocksCount)
		{
			freeBlocksCount++;
			freeInd = *static_cast<size_t*>(static_cast<void*>(static_cast<byte*>(buffer->blocks) + freeInd * m_blockSize));
		}

		std::cout << "Used blocks count:" << buffer->initBlocksCount - freeBlocksCount  << std::endl;
		std::cout << "Free blocks count:" << freeBlocksCount << std::endl;

		buffer = buffer->next;
	}
}

void FixedSizeAllocator::dumpBlocks() const
{

	assert(!isInit, "Allocator not initialized!");

	Buffer* buffer = m_firstBuffer;

	while (buffer != nullptr)
	{
		std::cout << "Buffer:" << buffer << std::endl;

		for (int i = 0; i < m_blocksCountInBuffer; i++)
		{
			std::cout << "Block:" << (static_cast<void*>(static_cast<byte*>(buffer->blocks) + (i * m_blockSize)));
			std::cout << " size:" << m_blockSize << " bytes";
			std::cout << " status:";

			if (i >= buffer->initBlocksCount)
			{
				std::cout << "NOT INIT";
			}
			else
			{
				int freeBlockIndex = buffer->freeHeadBlock;
				bool isFree = false;
				while (freeBlockIndex < buffer->initBlocksCount)
				{
					if (freeBlockIndex == i)
					{
						isFree = true;
						break;
					}

					freeBlockIndex = *static_cast<size_t*>(static_cast<void*>(static_cast<byte*>(buffer->blocks) + freeBlockIndex * m_blockSize));
				}

				std::cout << (isFree ? "FREE" : "USED");
			}

			std::cout << std::endl;

		}

		buffer = buffer->next;
	}
}
#endif

bool FixedSizeAllocator::contains(void* p)
{
	return getPointerLocation(p) != nullptr;
}

FixedSizeAllocator::Buffer* FixedSizeAllocator::allocBuffer()
{
	void* pBuf = VirtualAlloc(NULL, sizeof(Buffer) + m_blockSize * m_blocksCountInBuffer, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	Buffer* newBuffer = static_cast<Buffer*>(pBuf);
	
	newBuffer->initBlocksCount = 0;
	newBuffer->freeHeadBlock = 0;
	newBuffer->next = nullptr;
	newBuffer->blocks = static_cast<byte*>(pBuf) + sizeof(Buffer);

	return newBuffer;
}

void FixedSizeAllocator::freeBuffer(Buffer* buffer)
{
	if (buffer != nullptr)
	{
		VirtualFree(static_cast<void*>(buffer), 0, MEM_RELEASE);
	}
}

FixedSizeAllocator::Buffer* FixedSizeAllocator::getPointerLocation(void* p)
{

#ifdef DEBUG
	assert(!isInit, "Allocator not initialized!");
#endif // DEBUG

	Buffer* buffer = m_firstBuffer;

	while (buffer != nullptr)
	{
		if (static_cast<void*>(buffer->blocks) <= p && static_cast<void*>(static_cast<byte*>(buffer->blocks) + m_blocksCountInBuffer * m_blockSize) > p)
		{
			return buffer;
		}

		buffer = buffer->next;
	}

	return nullptr;
}