#pragma once
#include <cstddef>
#include <initializer_list>
#include <iterator>

namespace myStl
{

	template<typename T>
	class Array final
	{
	public:
		struct Iterator
		{
			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;

		public:
			Iterator(Array<T>* arr, int64_t pos = 0) : m_pArr(arr), m_Position(pos) {}

			reference operator*() const { return (*m_pArr)[m_Position]; }
			pointer operator->() { return &(*m_pArr)[m_Position]; }

			Iterator& operator++() { m_Position++; return *this; }
			Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
			Iterator& operator--() { m_Position--; return *this; }
			Iterator operator--(int) { Iterator tmp = *this; --(*this); return tmp; }

			friend bool operator==(const Iterator& a, const Iterator& b) { return a.m_pArr == b.m_pArr && a.m_Position == b.m_Position; }
			friend bool operator!=(const Iterator& a, const Iterator& b) { return a.m_pArr != b.m_pArr || a.m_Position != b.m_Position; }

			const reference get() const { return m_pArr->m_data[m_Position]; }
			void set(const reference value) { m_pArr->m_data[m_Position] = value; }
			void next() { m_Position++; }
			void previous() { m_Position--; }
			bool hasNext() const { return m_Position < m_pArr->m_size; }
			bool hasPrevious() const { return m_Position >= 0; }

		private:
			Array<T>* m_pArr;
			int64_t m_Position;
		};

		struct ConstIterator
		{
			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = const T*;
			using reference = const T&;

		public:
			ConstIterator(const Array<T>* arr, int64_t pos = 0) : m_pArr(arr), m_Position(pos) {}
			
			reference operator*() const { return (*m_pArr)[m_Position]; }
			pointer operator->() const { return &(*m_pArr)[m_Position]; }

			ConstIterator& operator++() { m_Position++; return *this; }
			ConstIterator operator++(int) { ConstIterator tmp = *this; ++(*this); return tmp; }
			ConstIterator& operator--() { m_Position--; return *this; }
			ConstIterator operator--(int) { ConstIterator tmp = *this; --(*this); return tmp; }

			friend bool operator==(const ConstIterator& a, const ConstIterator& b) { return a.m_pArr == b.m_pArr && a.m_Position == b.m_Position; }
			friend bool operator!=(const ConstIterator& a, const ConstIterator& b) { return a.m_pArr != b.m_pArr || a.m_Position != b.m_Position; }

			const reference get() const { return m_pArr->m_data[m_Position]; }
			void next() { m_Position++; }
			void previous() { m_Position--; }
			bool hasNext() const { return m_Position < m_pArr->m_size; }
			bool hasPrevious() const { return m_Position >= 0; }
		private:
			const Array<T>* m_pArr;
			int64_t m_Position;
		};

		// smort stl-comforming iterators
		Iterator begin() { return Iterator(this); }
		Iterator end() { return Iterator(this, m_size); }
		Iterator rbegin() { return --end(); }
		Iterator rend() { return --begin(); }
		ConstIterator cbegin() const { return ConstIterator(this); }
		ConstIterator cend() const { return ConstIterator(this, m_size); }
		ConstIterator crbegin() const { return --cend(); }
		ConstIterator crend() const { return --cbegin(); }

		//silly non-stl iterators. Task
		Iterator iterator() { return Iterator(this); }
		Iterator reverseIterator() { return Iterator(this, m_size - 1); }
		ConstIterator constIterator() const { return ConstIterator(this); }
		ConstIterator constReverseIterator() const { return ConstIterator(this, m_size - 1 ); }

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

		size_t insert(std::initializer_list<T> initList);
		size_t insert(size_t index, std::initializer_list<T> initList);

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

