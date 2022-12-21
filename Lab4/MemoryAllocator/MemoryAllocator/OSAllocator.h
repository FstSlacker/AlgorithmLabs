#pragma once
#include <windows.h>

class OSAllocator
{
public:
	OSAllocator();
	~OSAllocator();
	virtual void destroy();
	virtual void* alloc(size_t size);
	virtual void free(void* p);
	virtual void dumpStat() const;
	virtual void dumpBlocks() const;
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

	Buffer* m_firstBuffer;

	Buffer* allocBuffer(size_t size);
	void freeBuffer(Buffer* buf);
	Buffer* getPointerLocation(void* p);
};

OSAllocator::OSAllocator()
{

}

OSAllocator::~OSAllocator()
{
	destroy();
}

void OSAllocator::destroy()
{
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
	Buffer* newBuffer = allocBuffer(size);

	if (m_firstBuffer != nullptr)
	{
		m_firstBuffer->prev = newBuffer;
		newBuffer->next = m_firstBuffer;
	}
	
	m_firstBuffer = newBuffer;

	return newBuffer->data;
}

void OSAllocator::free(void* p)
{
	Buffer* buffer = getPointerLocation(p);

	if (buffer == m_firstBuffer)
	{
		m_firstBuffer = buffer->next;
	}

	buffer->removeFromList();

	freeBuffer(buffer);
}

void OSAllocator::dumpStat() const
{
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
	Buffer* buffer = m_firstBuffer;

	while (buffer != nullptr)
	{
		if (static_cast<void*>(buffer) == p)
		{
			return buffer;
		}

		buffer = buffer->next;
	}

	return nullptr;
}
