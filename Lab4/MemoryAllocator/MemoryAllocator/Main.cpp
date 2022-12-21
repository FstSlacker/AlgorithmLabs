#include <iostream>
#include "MemoryAllocator.h"

int main()
{
	MemoryAllocator a;

	a.init();

	void* p16 = a.alloc(16);
	void* p32 = a.alloc(32);
	void* p64 = a.alloc(64);
	void* p128 = a.alloc(128);
	void* p256 = a.alloc(256);
	void* p512 = a.alloc(512);
	void* p1024 = a.alloc(1024);
	void* p10mb = a.alloc(_10MB + 1);

#ifdef _DEBUG
	a.dumpStat();
	//a.dumpBlocks();
#endif

	a.free(p16);
	a.free(p32);
	a.free(p64);
	a.free(p128);
	a.free(p256);
	a.free(p512);
	a.free(p1024);
	a.free(p10mb);

#ifdef _DEBUG
	a.dumpStat();
	//a.dumpBlocks();
#endif

	a.destroy();

}
