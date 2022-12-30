#pragma once
#include "FixedSizeAllocator.h"
#include "CoalesceAllocator.h"
#include "OSAllocator.h"

#define _10MB 10485760

class MemoryAllocator
{
public:
	MemoryAllocator();
	virtual ~MemoryAllocator();
	virtual void init();
	virtual void destroy();
	virtual void* alloc(size_t size);
	virtual void free(void* p);
#ifdef _DEBUG
	virtual void dumpStat() const;
	virtual void dumpBlocks() const;
#endif

private:
	FixedSizeAllocator m_fsa16;
	FixedSizeAllocator m_fsa32;
	FixedSizeAllocator m_fsa64;
	FixedSizeAllocator m_fsa128;
	FixedSizeAllocator m_fsa256;
	FixedSizeAllocator m_fsa512;

	CoalesceAllocator m_coalesce;

	OSAllocator m_os;
};

MemoryAllocator::MemoryAllocator() :
									m_fsa16(16, _10MB / 16),
									m_fsa32(32, _10MB / 32),
									m_fsa64(64, _10MB / 64),
									m_fsa128(128, _10MB / 128),
									m_fsa256(256, _10MB / 256),
									m_fsa512(512, _10MB / 512),
									m_coalesce(_10MB)
{
	//...
}

MemoryAllocator::~MemoryAllocator()
{

}

void MemoryAllocator::init()
{
	m_fsa16.init();
	m_fsa32.init();
	m_fsa64.init();
	m_fsa128.init();
	m_fsa256.init();
	m_fsa512.init();

	m_coalesce.init();
}

void MemoryAllocator::destroy()
{
	m_fsa16.destroy();
	m_fsa32.destroy();
	m_fsa64.destroy();
	m_fsa128.destroy();
	m_fsa256.destroy();
	m_fsa512.destroy();

	m_coalesce.destroy();

	m_os.destroy();
}

void* MemoryAllocator::alloc(size_t size)
{
	void* p;

	if (size <= 16)
	{
		p = m_fsa16.alloc(size);
	}
	else if (size <= 32)
	{
		p = m_fsa32.alloc(size);
	}
	else if (size <= 64)
	{
		p = m_fsa64.alloc(size);
	}
	else if (size <= 128)
	{
		p = m_fsa128.alloc(size);
	}
	else if (size <= 256)
	{
		p = m_fsa256.alloc(size);
	}
	else if (size <= 512)
	{
		p = m_fsa512.alloc(size);
	}
	else if (size <= _10MB)
	{
		p = m_coalesce.alloc(size);
	}
	else 
	{
		p = m_os.alloc(size);
	}

	return p;
}

void MemoryAllocator::free(void* p)
{
	if (m_fsa16.contains(p))
	{
		m_fsa16.free(p);
	}
	else if (m_fsa32.contains(p))
	{
		m_fsa32.free(p);
	}
	else if (m_fsa64.contains(p))
	{
		m_fsa64.free(p);
	}
	else if (m_fsa128.contains(p))
	{
		m_fsa128.free(p);
	}
	else if (m_fsa256.contains(p))
	{
		m_fsa256.free(p);
	}
	else if (m_fsa512.contains(p))
	{
		m_fsa512.free(p);
	}
	else if (m_coalesce.contains(p))
	{
		m_coalesce.free(p);
	}
	else if(m_os.contains(p))
	{
		m_os.free(p);
	}
}

#ifdef _DEBUG
void MemoryAllocator::dumpStat() const
{
	std::cout << "FSA16 Stats" << std::endl;
	m_fsa16.dumpStat();
	std::cout << "FSA32 Stats" << std::endl;
	m_fsa32.dumpStat();
	std::cout << "FSA64 Stats" << std::endl;
	m_fsa64.dumpStat();
	std::cout << "FSA128 Stats" << std::endl;
	m_fsa128.dumpStat();
	std::cout << "FSA256 Stats" << std::endl;
	m_fsa256.dumpStat();
	std::cout << "FSA512 Stats" << std::endl;
	m_fsa512.dumpStat();
	std::cout << "Coalesce Stats" << std::endl;
	m_coalesce.dumpStat();
	std::cout << "OS allocator Stats" << std::endl;
	m_os.dumpStat();
}

void MemoryAllocator::dumpBlocks() const
{
	std::cout << "FSA16 Blocks Stats" << std::endl;
	m_fsa16.dumpBlocks();
	std::cout << "FSA32 Blocks Stats" << std::endl;
	m_fsa32.dumpBlocks();
	std::cout << "FSA64 Blocks Stats" << std::endl;
	m_fsa64.dumpBlocks();
	std::cout << "FSA128 Blocks Stats" << std::endl;
	m_fsa128.dumpBlocks();
	std::cout << "FSA256 Blocks Stats" << std::endl;
	m_fsa256.dumpBlocks();
	std::cout << "FSA512 Blocks Stats" << std::endl;
	m_fsa512.dumpBlocks();
	std::cout << "Coalesce Blocks Stats" << std::endl;
	m_coalesce.dumpBlocks();
	std::cout << "OS allocator Blocks Stats" << std::endl;
	m_os.dumpBlocks();
}
#endif