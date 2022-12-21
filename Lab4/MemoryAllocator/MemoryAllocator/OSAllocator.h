#pragma once
#include <windows.h>

#ifdef _DEBUG
#include <cassert>
#define assert_msg(exp, msg) assert(((void)msg, exp))
#endif

class OSAllocator
{
public:
	OSAllocator();
	~OSAllocator();
	virtual void destroy();
	virtual void* alloc(size_t size);
	virtual void free(void* p);
#ifdef _DEBUG
	virtual void dumpStat() const;
	virtual void dumpBlocks() const;
#endif
	virtual bool contains(void* p);

private:
	struct Buffer
	{
		Buffer* prev;
		Buffer* next;
		size_t size;
		void* data;

		void removeFromList()
		{
			if (prev != nullptr)
			{
				prev->next = next;
			}
			if (next != nullptr)
			{
				next->prev = prev;
			}
		}
	};

#ifdef _DEBUG
	bool isInit;

	size_t usedBlocksCount;
#endif

	Buffer* m_firstBuffer;

	Buffer* allocBuffer(size_t size);
	void freeBuffer(Buffer* buf);
	Buffer* getPointerLocation(void* p);
};

OSAllocator::OSAllocator()
{
#ifdef _DEBUG
	isInit = true;
#endif
}

OSAllocator::~OSAllocator()
{
#ifdef _DEBUG
	assert_msg(isInit == false, "Allocator not destroyed before destructor call!");
#endif
}

void OSAllocator::destroy()
{
#ifdef _DEBUG
	assert_msg(usedBlocksCount == 0, "Allocator have used blocks and can't be destroyed!");
	isInit = false;
#endif
	Buffer* buffer = m_firstBuffer;
	while (buffer != nullptr)
	{
		Buffer* prev = buffer;
		buffer = buffer->next;
		freeBuffer(prev);
	}

	m_firstBuffer = nullptr;
}

void* OSAllocator::alloc(size_t size)
{
#ifdef _DEBUG
	isInit = true;
#endif
	Buffer* newBuffer = allocBuffer(size);

	if (m_firstBuffer != nullptr)
	{
		m_firstBuffer->prev = newBuffer;
		newBuffer->next = m_firstBuffer;
	}
	
	m_firstBuffer = newBuffer;
#ifdef _DEBUG
	usedBlocksCount++;
#endif
	return newBuffer->data;
}

void OSAllocator::free(void* p)
{
#ifdef _DEBUG
	assert_msg(isInit, "Allocator not initialized!");
#endif
	Buffer* buffer = getPointerLocation(p);

	if (buffer == m_firstBuffer)
	{
		m_firstBuffer = buffer->next;
	}

	buffer->removeFromList();

	freeBuffer(buffer);
#ifdef _DEBUG
	usedBlocksCount--;
#endif
}

#ifdef _DEBUG
void OSAllocator::dumpStat() const
{
	assert_msg(isInit, "Allocator not initialized!");
	Buffer* buf = m_firstBuffer;
	size_t bufCount = 0;
	while (buf != nullptr)
	{
		bufCount++;
		buf = buf->next;
	}

	std::cout << "BuffersCount:" << bufCount << std::endl;
}

void OSAllocator::dumpBlocks() const
{
	assert_msg(isInit, "Allocator not initialized!");
	Buffer* buf = m_firstBuffer;

	while (buf != nullptr)
	{
		std::cout << "Buffer:" << buf;
		std::cout << " Size:" << buf->size;
		std::cout << " Prev:" << buf->prev;
		std::cout << " Next:" << buf->next;
		std::cout << std::endl;

		buf = buf->next;
	}

}
#endif

bool OSAllocator::contains(void* p)
{
	return getPointerLocation(p) != nullptr;
}

OSAllocator::Buffer* OSAllocator::allocBuffer(size_t size)
{
	void* pBuf = VirtualAlloc(NULL, sizeof(Buffer) + size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	Buffer* buf = static_cast<Buffer*>(pBuf);

	buf->size = sizeof(Buffer) + size;
	buf->data = static_cast<byte*>(pBuf) + sizeof(Buffer);
	buf->next = nullptr;
	buf->prev = nullptr;

	return buf;
}

void OSAllocator::freeBuffer(Buffer* buf)
{
	if (buf != nullptr)
	{
		VirtualFree(static_cast<void*>(buf), 0, MEM_RELEASE);
	}
}

OSAllocator::Buffer* OSAllocator::getPointerLocation(void* p)
{
#ifdef _DEBUG
	assert_msg(isInit, "Allocator not initialized!");
#endif
	Buffer* buffer = m_firstBuffer;

	while (buffer != nullptr)
	{
		if (static_cast<void*>(buffer->data) == p)
		{
			return buffer;
		}

		buffer = buffer->next;
	}

	return nullptr;
}
