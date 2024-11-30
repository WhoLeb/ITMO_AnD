#include <iostream>
#include "MyMemory.h"

int main()
{
	myStl::MemoryManager allocator;
	allocator.init();

	void* p1 = allocator.alloc(1000);
	void* p2 = allocator.alloc(1000);
	void* p3 = allocator.alloc(1000);
	void* p4 = allocator.alloc(1000);
	void* p5 = allocator.alloc(1000);

	void** ppc = (void**)allocator.alloc(sizeof(void*) * 6);
	for (int i = 0; i < 6; i++)
		ppc[i] = allocator.alloc(512);

	void* pos = allocator.alloc(1024 * 1024 * 100);

	allocator.dumpStat();

	//allocator.free(ppc[3]);
	for (int i = 0; i < 6; i++)
	{
		//if (i == 3)
		//	continue;
		allocator.free(ppc[i]);
	}
	allocator.free(ppc);

	allocator.free(p1);
	allocator.free(p1);
	allocator.free(p2);
	allocator.free(p3);
	allocator.free(p4);
	allocator.free(p5);

	allocator.free(pos);

	allocator.dumpStat();

	allocator.destroy();
}
