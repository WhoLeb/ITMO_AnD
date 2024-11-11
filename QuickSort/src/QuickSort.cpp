#include <iostream>

#include <algorithm>

#include "MyAlgorithm.h"
#include "../../DynamicArray/src/Array.h"
#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h";
#include <string>
#include <random>

int main()
{
	constexpr int ArrSize = 10'000'000;
	
	int* arr = new int[ArrSize];
	int* copyArr = new int[ArrSize];
	for (int i = 0; i < ArrSize; i++)
		arr[i] = rand();

	//ankerl::nanobench::Bench().epochIterations(5).run("Insertion sort", [&] {
	//	myStl::insertionSort(copyArr, copyArr + ArrSize, [](int a, int b) { return a < b; });
	//	});
	memcpy(copyArr, arr, ArrSize);

	for (int i = 2; i <= 100000; i *= 2)
	{
		std::string BenchName = "Quick sort with insertion sort threshold " + std::to_string(i);
		ankerl::nanobench::Bench().epochIterations(1).run(BenchName, [&] {
			myStl::insertionSortThreshold = i;
			myStl::quickSort(copyArr, copyArr + ArrSize, [](int a, int b) { return a < b; });
			});
		memcpy(copyArr, arr, ArrSize);
	}
}

