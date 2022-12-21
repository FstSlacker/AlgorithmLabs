#pragma once
#include <windows.h>

struct Buffer {
	size_t initBlocksCount;
	size_t freeHeadBlock;
	Buffer* next;
	void* blocks;
};

class AllocatorBase
{
protected:

	Buffer* allocBuffer();
	void freeBuffer(Buffer* buf);
	void traverseFreeBuffer(Buffer* buf); //Free buffer and its all childs buffers
	Buffer* findPointerLocation(void* p);
};



