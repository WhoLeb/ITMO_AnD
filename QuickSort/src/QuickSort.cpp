#include <iostream>

#include <algorithm>

#include "MyAlgorithm.h"
#include "../../DynamicArray/src/Array.h"
#include "chrono"
#include <string>
#include <random>

int main()
{
	constexpr int ArrSize = 10'000'000;
	
	int* arr = new int[ArrSize];
	int* copyArr = new int[ArrSize];
	for (int i = 0; i < ArrSize; i++)
	{
		arr[i] = ArrSize - i;
	}
	for (int i = 0; i < 10; i++)
		std::cout << arr[i] << ' ';
	std::cout << "\n";
	memcpy(copyArr, arr, ArrSize);

	for (int i = 2; i <= 100000; i *= 2)
	{
		constexpr int iterations = 1000;
		std::chrono::duration<double> qSortDuration;
		for (int j = 0; j < iterations; j++)
		{
			auto start = std::chrono::high_resolution_clock::now();
			myStl::quickSort(copyArr, copyArr + i, std::less<int>());
			auto end = std::chrono::high_resolution_clock::now();
			memcpy(copyArr, arr, i);
			qSortDuration += end - start;
		}
		double qSortAverage = qSortDuration.count() / iterations;
		std::cout << "ArraySize: " << std::setw(8) << i << "\tQSort time: " << qSortAverage << "\t";

		std::chrono::duration<double> iSortDuration;
		for (int j = 0; j < iterations; j++)
		{
			auto start = std::chrono::high_resolution_clock::now();
			myStl::insertionSort(copyArr, copyArr + i, std::less<int>());
			auto end = std::chrono::high_resolution_clock::now();
			memcpy(copyArr, arr, i);
			iSortDuration += end - start;
		}
		double iSortAverage = iSortDuration.count() / iterations;
		std::cout << "ISort time: " << iSortAverage << "\tQSort is " << std::setw(8) << iSortAverage / qSortAverage << " times faster" << "\n";
	}

}

