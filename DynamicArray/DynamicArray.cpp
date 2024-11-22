#include <iostream>
#include <vector>

#define CHECK_ALLOCATIONS
#include "src/Array.h"

int main()
{
	int iArr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	std::cout << &iArr[9] - &iArr[0] << "\n";
	for (auto k : iArr)
	{
		std::cout << k << " ";
	}
	std::cout << "\n";
	{
		myStl::Array<int> a = { 1, 2, 3 };
		a.insert(4);
		a.insert({ 5, 6, 7, 8, 9 });
		auto i = a.begin();
		auto j = a.end();
		std::cout << i - j << " " << a.size() << "\n";
		myStl::Array<int> b;
		b = std::move(a);
		for (const auto& it : a)
			std::cout << it << ' ';
		for (const auto& it : b)
			std::cout << it << ' ';
	}
	std::cout << "\n\n" << g_Stats;

	myStl::Array<int> a = { 1, 2, 3, 4 };
	std::vector<int> tmp(4);
	std::vector<int> control = { 4, 3, 2, 1 };
	int i = 0;
	for (auto it = a.reverseIterator(); it.hasNext(); it.next())
		tmp[i++] = it.get();


	return 0;
}
