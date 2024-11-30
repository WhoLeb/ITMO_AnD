#pragma once

namespace myStl
{

	template <typename T>
	void swap(T& a, T& b)
	{
		T temp = std::move(a);
		a = std::move(b);
		b = std::move(temp);
	}

	template <typename It, typename Compare>
	void insertionSort(It first, It last, Compare comp)
	{
		for (It i = first + 1; i < last; i++)
		{
			auto temp = std::move(*i);
			It j = i;
			while (j > first && comp(temp, *(j - 1)))
			{
				*j = std::move(*(j - 1));
				--j;
			}
			*j = std::move(temp);
		}
	}

	template<typename It, typename Compare>
	It partition(It first, It last, Compare comp)
	{
		It mid = first + (last - first) / 2;
		if (comp(*mid, *first)) swap(*first, *mid);
		if (comp(*(last - 1), *first)) swap(*first, *(last - 1));

		swap(*mid, *(last - 1));

		auto& pivot = *(last - 1);
		It left = first;
		It right = last - 2;

		while (true)
		{
			while (left <= right && comp(*left, pivot)) ++left;
			while (right >= left && comp(pivot, *right)) --right;
			if (left >= right) break;
			swap(*left, *right);
			++left;
			--right;
		}
		swap(*left, *(last-1));
		return left;
	}

	template<typename It, typename Compare>
	void quickSort(It first, It last, Compare comp)
	{
		constexpr int insertionSortThreshold = 0;
		while (last - first > insertionSortThreshold)
		{
			if (last - first <= insertionSortThreshold)
			{
				insertionSort(first, last, comp);
				return;
			}
			It pivot = myStl::partition(first, last, comp);
			if (pivot - first < last - (pivot + 1))
			{
				quickSort(first, pivot, comp);
				first = pivot + 1;
			}
			else
			{
				quickSort(pivot + 1, last, comp);
				last = pivot;
			}
		}
	}

}
