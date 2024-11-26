#include <iostream>
#include "MyMemory.h"

int main()
{
	myStl::MemoryManager allocator;
	allocator.init();

	//void* p1 = allocator.alloc(1000);
	//void* p2 = allocator.alloc(1000);
	//void* p3 = allocator.alloc(1000);
	//allocator.dumpStat();
	//allocator.free(p1);
	//allocator.dumpStat();
	//allocator.free(p1);
	//allocator.dumpStat();
	//allocator.free(p2);
	//allocator.free(p3);

	void** ppc = (void**)allocator.alloc(sizeof(void*) * 6);
	for (int i = 0; i < 6; i++)
		ppc[i] = allocator.alloc(512);
	allocator.free(ppc[3]);
	for (int i = 0; i < 6; i++)
	{
		if (i == 3)
			continue;
		allocator.free(ppc[i]);
	}
	allocator.free(ppc);

	//int* p1 = (int*)allocator.alloc(1000 * sizeof(int));
	//int* p2 = (int*)allocator.alloc(8020 * sizeof(int));
	//double* pd = (double*)allocator.alloc(sizeof(double));
	//int* pa = (int*)allocator.alloc(10 * sizeof(int));
	//allocator.dumpStat();
	//allocator.dumpBlocks();
	//allocator.free(pa);
	//allocator.free(pd);
	//allocator.free(p1);
	//allocator.dumpStat();
	//allocator.free(p2);
	//allocator.dumpStat();
	//std::cout << "\n";
	//for (int i = 0; i < 1000; i++)
	//{
	//	std::cout << pi;
	//}
	allocator.destroy();
}
