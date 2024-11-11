#include "pch.h"

#include <random>
#include "../DynamicArray/src/Array.h"
#include "../QuickSort/src/MyAlgorithm.h"

// Helper function to generate an array of random integers
int* generateRandomArray(size_t size)
{
	int* arr = new int[size];
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, 100000);
	for (size_t i = 0; i < size; ++i)
	{
		arr[i] = dis(gen);
	}
	return arr;
}

// Test for random primitive array
TEST(PrimitiveArrays, RandomPrimitiveArray)
{
	constexpr size_t ArrayLength = 100;
	int* arr = generateRandomArray(ArrayLength);
	int* expected = new int[ArrayLength];
	std::copy(arr, arr + ArrayLength, expected);
	std::sort(expected, expected + ArrayLength);

	myStl::quickSort(arr, arr + ArrayLength, std::less<int>());
	for (size_t i = 0; i < ArrayLength; ++i)
	{
		EXPECT_EQ(arr[i], expected[i]);
	}

	delete[] arr;
	delete[] expected;
}

// Test for already sorted primitive array
TEST(PrimitiveArrays, SortedPrimitiveArray)
{
	constexpr size_t ArrayLength = 100;
	int* arr = new int[ArrayLength];
	for (size_t i = 0; i < ArrayLength; ++i)
	{
		arr[i] = static_cast<int>(i);
	}
	int* expected = new int[ArrayLength];
	std::copy(arr, arr + ArrayLength, expected);

	myStl::quickSort(arr, arr + ArrayLength, std::less<int>());
	for (size_t i = 0; i < ArrayLength; ++i)
	{
		EXPECT_EQ(arr[i], expected[i]);
	}

	delete[] arr;
	delete[] expected;
}

// Test for reverse sorted primitive array
TEST(PrimitiveArrays, BackSortedPrimitiveArray)
{
	constexpr size_t ArrayLength = 100;
	int* arr = new int[ArrayLength];
	for (size_t i = 0; i < ArrayLength; ++i)
	{
		arr[i] = static_cast<int>(ArrayLength - i);
	}
	int* expected = new int[ArrayLength];
	std::copy(arr, arr + ArrayLength, expected);
	std::sort(expected, expected + ArrayLength);

	myStl::quickSort(arr, arr + ArrayLength, std::less<int>());
	for (size_t i = 0; i < ArrayLength; ++i)
	{
		EXPECT_EQ(arr[i], expected[i]);
	}

	delete[] arr;
	delete[] expected;
}

// Test for small random primitive array
TEST(PrimitiveArrays, SmallRandomPrimitiveArray)
{
	constexpr size_t ArrayLength = 100;
	int* arr = generateRandomArray(ArrayLength);
	int* expected = new int[ArrayLength];
	std::copy(arr, arr + ArrayLength, expected);
	std::sort(expected, expected + ArrayLength);

	myStl::quickSort(arr, arr + ArrayLength, std::less<int>());
	for (size_t i = 0; i < ArrayLength; ++i)
	{
		EXPECT_EQ(arr[i], expected[i]);
	}

	delete[] arr;
	delete[] expected;
}

// Test for large random primitive array
TEST(PrimitiveArrays, LargeRandomPrimitiveArray)
{
	constexpr size_t ArrayLength = 1000000;
	int* arr = generateRandomArray(ArrayLength);
	int* expected = new int[ArrayLength];
	std::copy(arr, arr + ArrayLength, expected);
	std::sort(expected, expected + ArrayLength);

	myStl::quickSort(arr, arr + ArrayLength, std::less<int>());
	for (size_t i = 0; i < ArrayLength; ++i)
	{
		EXPECT_EQ(arr[i], expected[i]);
	}

	delete[] arr;
	delete[] expected;
}

TEST(PrimitiveArrays, EmptyArray) {
	int* arr = nullptr;
	myStl::quickSort(arr, arr, std::less<int>());
	EXPECT_EQ(arr, nullptr);
}

TEST(PrimitiveArrays, SingleElementArray) {
	int arr[] = { 42 };
	myStl::quickSort(arr, arr + 1, std::less<int>());
	EXPECT_EQ(arr[0], 42);
}

TEST(PrimitiveArrays, ArrayWithDuplicates) {
	int arr[] = { 5, 3, 8, 3, 9, 1, 5, 3 };
	int expected[] = { 1, 3, 3, 3, 5, 5, 8, 9 };
	myStl::quickSort(arr, arr + 8, std::less<int>());
	for (int i = 0; i < 8; ++i) {
		EXPECT_EQ(arr[i], expected[i]);
	}
}

TEST(PrimitiveArrays, ArrayWithAllIdenticalElements) {
	int arr[] = { 7, 7, 7, 7, 7 };
	int expected[] = { 7, 7, 7, 7, 7 };
	myStl::quickSort(arr, arr + 5, std::less<int>());
	for (int i = 0; i < 5; ++i) {
		EXPECT_EQ(arr[i], expected[i]);
	}
}

TEST(PrimitiveArrays, ArrayWithNegativeNumbers) {
	int arr[] = { -3, -1, -7, -4, -5, -2, -6 };
	int expected[] = { -7, -6, -5, -4, -3, -2, -1 };
	myStl::quickSort(arr, arr + 7, std::less<int>());
	for (int i = 0; i < 7; ++i) {
		EXPECT_EQ(arr[i], expected[i]);
	}
}

TEST(PrimitiveArrays, ArrayWithMixedNumbers) {
	int arr[] = { 3, -1, 4, -5, 0, 2, -2 };
	int expected[] = { -5, -2, -1, 0, 2, 3, 4 };
	myStl::quickSort(arr, arr + 7, std::less<int>());
	for (int i = 0; i < 7; ++i) {
		EXPECT_EQ(arr[i], expected[i]);
	}
}

TEST(PrimitiveArrays, ArrayWithFloats) {
	float arr[] = { 3.1f, -1.2f, 4.5f, -5.6f, 0.0f, 2.3f, -2.4f };
	float expected[] = { -5.6f, -2.4f, -1.2f, 0.0f, 2.3f, 3.1f, 4.5f };
	myStl::quickSort(arr, arr + 7, std::less<float>());
	for (int i = 0; i < 7; ++i) {
		EXPECT_FLOAT_EQ(arr[i], expected[i]);
	}
}

TEST(PrimitiveArrays, ArrayWithStrings) {
	std::string arr[] = { "apple", "orange", "banana", "pear", "grape" };
	std::string expected[] = { "apple", "banana", "grape", "orange", "pear" };
	myStl::quickSort(arr, arr + 5, std::less<std::string>());
	for (int i = 0; i < 5; ++i) {
		EXPECT_EQ(arr[i], expected[i]);
	}
}

TEST(PrimitiveArrays, CustomComparator) {
	int arr[] = { 5, 3, 8, 1, 9, 2 };
	int expected[] = { 9, 8, 5, 3, 2, 1 };
	myStl::quickSort(arr, arr + 6, std::greater<int>());
	for (int i = 0; i < 6; ++i) {
		EXPECT_EQ(arr[i], expected[i]);
	}
}

template<typename T>
myStl::Array<T> generateCustomRandomArray(size_t size)
{
	myStl::Array<T> arr(size);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, 100000);
	for (size_t i = 0; i < size; ++i)
	{
		arr.insert(i, dis(gen));
	}
	return arr;
}

// Custom array test using generateCustomRandomArray
TEST(CustomArrays, RandomCustomArray) {
	constexpr size_t ArrayLength = 100;
	myStl::Array<int> arr = generateCustomRandomArray<int>(ArrayLength);
	myStl::Array<int> expected = arr;
	std::sort(expected.begin(), expected.end());

	quickSort(arr.begin(), arr.end(), std::less<int>());
	EXPECT_EQ(arr, expected);
}

// Test for already sorted custom array
TEST(CustomArrays, SortedCustomArray)
{
	constexpr size_t ArrayLength = 100;
	myStl::Array<int> arr(ArrayLength);
	for (size_t i = 0; i < ArrayLength; ++i)
	{
		arr.insert(i, static_cast<int>(i));
	}
	myStl::Array<int> expected = arr;

	quickSort(arr.begin(), arr.end(), std::less<int>());
	EXPECT_EQ(arr, expected);
}

// Test for reverse sorted custom array
TEST(CustomArrays, BackSortedCustomArray)
{
	constexpr size_t ArrayLength = 100;
	myStl::Array<int> arr(ArrayLength);
	for (size_t i = 0; i < ArrayLength; ++i)
	{
		arr.insert(i, static_cast<int>(ArrayLength - i));
	}
	myStl::Array<int> expected = arr;
	std::sort(expected.begin(), expected.end());

	quickSort(arr.begin(), arr.end(), std::less<int>());
	EXPECT_EQ(arr, expected);
}

// Test for small random custom array
TEST(CustomArrays, SmallRandomCustomArray)
{
	constexpr size_t ArrayLength = 10;
	myStl::Array<int> arr = generateCustomRandomArray<int>(ArrayLength);
	myStl::Array<int> expected = arr;
	std::sort(expected.begin(), expected.end());

	quickSort(arr.begin(), arr.end(), std::less<int>());
	EXPECT_EQ(arr, expected);
}

// Test for large random custom array
TEST(CustomArrays, LargeRandomCustomArray)
{
	constexpr size_t ArrayLength = 1000000;
	myStl::Array<int> arr = generateCustomRandomArray<int>(ArrayLength);
	myStl::Array<int> expected = arr;
	std::sort(expected.begin(), expected.end());

	quickSort(arr.begin(), arr.end(), std::less<int>());
	EXPECT_EQ(arr, expected);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}