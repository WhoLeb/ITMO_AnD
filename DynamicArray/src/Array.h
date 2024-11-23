#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>

#ifdef CHECK_ALLOCATIONS
namespace
{
	struct Stats
	{
		size_t allocations{ 0 };
		size_t deallocations{ 0 };

		friend std::ostream& operator<<(std::ostream& os, Stats st)
		{
			os << "Allocations: " << st.allocations << ", deallocations: " << st.deallocations << "\n";
			return os;
		}
	} g_Stats;
	void* my_malloc(size_t size)
	{
		g_Stats.allocations++;
		return malloc(size);
	}
	void my_free(void* block)
	{
		g_Stats.deallocations++;
		free(block);
	}
}
#else
	void* my_malloc(size_t size)
	{
		return malloc(size);
	}
	void my_free(void* block)
	{
		free(block);
	}
#endif

namespace myStl
{

	template<typename T>
	class Array final
	{
	public:
		struct Iterator
		{
			using iterator_category	= std::random_access_iterator_tag;
			using difference_type	= std::ptrdiff_t;
			using value_type		= T;
			using pointer			= T*;
			using reference			= T&;

		public:
			Iterator(Array<T>* arr, int64_t pos = 0, int direction = 1) : m_pArr(arr), m_Position(pos), m_direction(direction) {}

			reference operator*() const { return m_pArr->m_data[m_Position]; }
			pointer operator->() { return &m_pArr->m_data[m_Position]; }
			//operator T* () const { return &m_pArr->m_data[m_Position]; }

			// Bidirectional iterator increments and decrements
			Iterator& operator++() { m_Position += m_direction; return *this; }
			Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
			Iterator& operator--() { m_Position -= m_direction; return *this; }
			Iterator operator--(int) { Iterator tmp = *this; --(*this); return tmp; }

			// Random access operations
			Iterator& operator +=(difference_type n) { m_Position += n * m_direction; return *this; }
			Iterator operator +(difference_type n) const { return Iterator(m_pArr, m_Position + n * m_direction); }
			friend Iterator operator +(difference_type n, const Iterator& it) { return Iterator(it.m_pArr, it.m_Position + n * it.m_direction); }
			Iterator& operator -=(difference_type n) { m_Position -= n * m_direction; return *this; }
			Iterator operator -(difference_type n) const { return Iterator(m_pArr, m_Position - n * m_direction); }
			difference_type operator-(const Iterator& other) const { return m_Position - other.m_Position; }

			// Element access for random access
			reference operator[](difference_type n) const { return m_pArr->m_data[m_Position + n * m_direction]; }

			friend bool operator==(const Iterator& a, const Iterator& b) { return a.m_pArr == b.m_pArr && a.m_Position == b.m_Position; }
			friend bool operator!=(const Iterator& a, const Iterator& b) { return a.m_pArr != b.m_pArr || a.m_Position != b.m_Position; }
			friend bool operator<(const Iterator& a, const Iterator& b) { return a.m_pArr == b.m_pArr && a.m_Position < b.m_Position; }
			friend bool operator>(const Iterator& a, const Iterator& b) { return b < a; }
			friend bool operator<=(const Iterator& a, const Iterator& b) { return !(b < a); }
			friend bool operator>=(const Iterator& a, const Iterator& b) { return !(a < b); }

			const reference get() const { return m_pArr->m_data[m_Position]; }
			void set(const reference value) { m_pArr->m_data[m_Position] = value; }
			void next() { m_Position += m_direction; }
			void previous() { m_Position -= m_direction; }
			bool hasNext() const { return m_direction == 1 ? m_Position < m_pArr->m_size : m_Position >= 0; }
			bool hasPrevious() const { return m_direction == 1 ? m_Position >= 0 : m_Position < m_pArr->m_size; }

		private:
			Array<T>* m_pArr;
			int64_t m_Position;
			int m_direction;
		};

		struct ConstIterator
		{
			using iterator_category	= std::random_access_iterator_tag;
			using difference_type	= std::ptrdiff_t;
			using value_type		= T;
			using pointer			= const T*;
			using reference			= const T&;

		public:
			ConstIterator(const Array<T>* arr, int64_t pos = 0, int direction = 1) : m_pArr(arr), m_Position(pos), m_direction(direction) {}

			reference operator*() const { return m_pArr->m_data[m_Position]; }
			pointer operator->() { return &m_pArr->m_data[m_Position]; }
			//operator T* () const { return &m_pArr->m_data[m_Position]; }

			// Bidirectional iterator increments and decrements
			ConstIterator& operator++() { m_Position += m_direction; return *this; }
			ConstIterator operator++(int) { ConstIterator tmp = *this; ++(*this); return tmp; }
			ConstIterator& operator--() { m_Position -= m_direction; return *this; }
			ConstIterator operator--(int) { ConstIterator tmp = *this; --(*this); return tmp; }

			// Random access operations
			ConstIterator& operator +=(difference_type n) { m_Position += n * m_direction; return *this; }
			ConstIterator operator +(difference_type n) const { return ConstIterator(m_pArr, m_Position + n * m_direction); }
			friend ConstIterator operator +(difference_type n, const ConstIterator& it) { return ConstIterator(it.m_pArr, it.m_Position + n * it.m_direction); }
			ConstIterator& operator -=(difference_type n) { m_Position -= n * m_direction; return *this; }
			ConstIterator operator -(difference_type n) const { return ConstIterator(m_pArr, m_Position - n * m_direction); }
			difference_type operator-(const ConstIterator& other) const { return m_Position - other.m_Position; }

			// Element access for random access
			reference operator[](difference_type n) const { return m_pArr->m_data[m_Position + n * m_direction]; }

			friend bool operator==(const ConstIterator& a, const ConstIterator& b) { return a.m_pArr == b.m_pArr && a.m_Position == b.m_Position; }
			friend bool operator!=(const ConstIterator& a, const ConstIterator& b) { return a.m_pArr != b.m_pArr || a.m_Position != b.m_Position; }
			friend bool operator<(const ConstIterator& a, const ConstIterator& b) { return a.m_pArr == b.m_pArr && a.m_Position < b.m_Position; }
			friend bool operator>(const ConstIterator& a, const ConstIterator& b) { return b < a; }
			friend bool operator<=(const ConstIterator& a, const ConstIterator& b) { return !(b < a); }
			friend bool operator>=(const ConstIterator& a, const ConstIterator& b) { return !(a < b); }

			const reference get() const { return m_pArr->m_data[m_Position]; }
			void set(const reference value) { m_pArr->m_data[m_Position] = value; }
			void next() { m_Position += m_direction; }
			void previous() { m_Position -= m_direction; }
			bool hasNext() const { return m_direction == 1 ? m_Position < m_pArr->m_size : m_Position >= 0; }
			bool hasPrevious() const { return m_direction == 1 ? m_Position >= 0 : m_Position < m_pArr->m_size; }

		private:
			const Array<T>* m_pArr;
			int64_t m_Position;
			int m_direction;
		};

		// smort stl-comforming iterators
		Iterator begin() { return Iterator(this); }
		Iterator end() { return Iterator(this, m_size); }
		Iterator rbegin() { return Iterator(this, m_size - 1, -1); }
		Iterator rend() { return Iterator(this, -1, -1); }
		ConstIterator cbegin() const { return ConstIterator(this); }
		ConstIterator cend() const { return ConstIterator(this, m_size); }
		ConstIterator crbegin() const { return ConstIterator(this, m_size - 1, -1); }
		ConstIterator crend() const { return ConstIterator(this, -1, -1); }

		//silly non-stl iterators. Task
		Iterator iterator() { return Iterator(this); }
		Iterator reverseIterator() { return Iterator(this, m_size - 1, -1); }
		ConstIterator constIterator() const { return ConstIterator(this); }
		ConstIterator constReverseIterator() const { return ConstIterator(this, m_size - 1, -1); }

	public:
		Array();
		Array(size_t capacity);
		Array(std::initializer_list<T> initList);

		Array(const Array<T>& other);
		Array<T>& operator=(const Array<T>& other);
		Array(Array<T>&& other);
		Array<T>& operator=(Array<T>&& other);

		~Array();

	public:
		size_t size() const { return m_size; }
		size_t capacity() const { return m_capacity; }

		size_t insert(const T& value);
		size_t insert(T&& value);
		size_t insert(size_t index, const T& value);
		size_t insert(size_t index, T&& value);

		size_t insert(const std::initializer_list<T>& initList);
		size_t insert(size_t index, const std::initializer_list<T>& initList);

		void remove(size_t index);
		
		const T& operator[](size_t index) const;
		T& operator[](size_t index);

		friend bool operator==(const Array<T>& a, const Array<T>& b)
		{
			if (a.size() != b.size()) {
				return false;
			}

			for (size_t i = 0; i < a.size(); i++)
				if (a.m_data[i] != b.m_data[i])
					return false;

			return true;
		}

	private:
		void reserve(size_t newCapacity);
	private:
		T* m_data;
		size_t m_size;
		size_t m_capacity;
	};

}

#include "Array.ipp"

