#pragma once

#include "Array.h"

#include "stdexcept"

namespace myStl
{
	template<typename T>
	inline Array<T>::Array()
		: m_size(0), m_capacity(8)
	{
		m_data = (T*)my_malloc(sizeof(T) * m_capacity);
		if (!m_data)
			throw std::bad_alloc();
	}


	template<typename T>
	inline Array<T>::Array(size_t capacity)
		: m_size(0), m_capacity(capacity)
	{
		m_data = (T*)my_malloc(sizeof(T) * m_capacity);
		if (!m_data)
			throw std::bad_alloc();
	}


	template<typename T>
	inline Array<T>::Array(std::initializer_list<T> initList)
		: m_size(initList.size()), m_capacity(initList.size())
	{
		m_data = (T*)my_malloc(sizeof(T) * m_capacity);
		if (!m_data)
			throw std::bad_alloc();
		int i = 0;
		for (const auto& item : initList)
			new (&m_data[i++]) T(item);
	}

	
	template<typename T>
	inline Array<T>::Array(const Array<T>& other)
	{
		m_capacity = other.m_capacity;
		m_size = other.m_size;

		m_data = (T*)my_malloc(sizeof(T) * m_capacity);

		memcpy(m_data, other.m_data, m_size * sizeof(T));
	}


	template<typename T>
	inline Array<T>& Array<T>::operator=(const Array<T>& other)
	{
		if (this == &other)
			return *this;

		for (size_t i = 0; i < m_size; i++)
			m_data[i].~T();
		my_free(m_data);

		m_capacity = other.m_capacity;
		m_size = other.m_size;

		m_data = (T*)my_malloc(sizeof(T) * m_capacity);

		memcpy(m_data, other.m_data, m_size * sizeof(T));

		return *this;
	}


	template<typename T>
	inline Array<T>::Array(Array<T>&& other)
	{
		m_capacity = other.m_capacity;
		m_size = other.m_size;
		m_data = other.m_data;

		other.m_data = nullptr;
		other.m_capacity = 0;
		other.m_size = 0;
	}

	
	template<typename T>
	inline Array<T>& Array<T>::operator=(Array<T>&& other)
	{
		if (this == &other)
			return *this;

		for (size_t i = 0; i < m_size; i++)
			m_data[i].~T();
		my_free(m_data);

		m_capacity = other.m_capacity;
		m_size = other.m_size;
		m_data = other.m_data;

		other.m_data = nullptr;
		other.m_capacity = 0;
		other.m_size = 0;
		return *this;
	}


	template<typename T>
	inline Array<T>::~Array()
	{
		for (size_t i = 0; i < m_size; i++)
			m_data[i].~T();
		my_free(m_data);
		m_size = 0;
		m_capacity = 0;
		m_data = nullptr;
	}


	template<typename T>
	inline size_t Array<T>::insert(const T& value)
	{
		return insert(m_size, value);
	}


	template<typename T>
	inline size_t Array<T>::insert(T&& value)
	{
		return insert(m_size, std::move(value));
	}


	template<typename T>
	inline size_t Array<T>::insert(size_t index, const T& value)
	{
		if (m_size == m_capacity)
			reserve(m_capacity == 0 ? 1 : m_capacity * 2);

		if constexpr (std::is_move_constructible<T>::value)
		{
			for (size_t i = m_size; i > index; i--)
			{
				new (&m_data[i]) T(std::move(m_data[i - 1]));
			}
		}
		else
		{
			for (size_t i = m_size; i > index; i--)
			{
				new (&m_data[i]) T(m_data[i - 1]);
				m_data[i - 1].~T();
			}
		}

		new (&m_data[index]) T(value);
		m_size++;

		return index;
	}


	template<typename T>
	inline size_t Array<T>::insert(size_t index, T&& value)
	{
		if (m_size == m_capacity)
			reserve(m_capacity == 0 ? 1 : m_capacity * 2);

		if constexpr (std::is_move_constructible<T>::value)
		{
			for (size_t i = m_size; i > index; i--)
				new (&m_data[i]) T(std::move(m_data[i - 1]));
		}
		else
		{
			for (size_t i = m_size; i > index; i--)
			{
				new (&m_data[i]) T(m_data[i - 1]);
				m_data[i - 1].~T();
			}
		}

		new (&m_data[index]) T(std::move(value));
		m_size++;

		return index;
	}


	template<typename T>
	inline size_t Array<T>::insert(std::initializer_list<T> initList)
	{
		return insert(m_size, initList);
	}


	template<typename T>
	inline size_t Array<T>::insert(size_t index, std::initializer_list<T> initList)
	{
		size_t n = initList.size();
		if ((m_size + n) >= m_capacity)
			reserve(m_capacity == 0 ? n				// if empty resize to list size
				: (m_capacity * 2) > (m_size + n) ? m_capacity * 2 : m_size + n);
		// else if doubling size is enough double it, else resize to size + list size

		if constexpr (std::is_move_constructible<T>::value)
		{
			for (size_t i = m_size + n - 1; i > index + n - 1; i--)
				new (&m_data[i]) T(std::move(m_data[i - n]));
		}
		else
		{
			for (size_t i = m_size + n - 1; i > index + n - 1; i--)
			{
				new (&m_data[i]) T(m_data[i - n]);
				m_data[i - n].~T();
			}
		}
		size_t j = index;
		for (const auto& it : initList)
			new (&m_data[j++]) T(it);

		m_size += n;
		return index;
	}


	template<typename T>
	inline void Array<T>::remove(size_t index)
	{
		m_data[index].~T();
		if constexpr (std::is_move_assignable<T>::value)
			for (size_t i = index; i < m_size; i++)
				m_data[i] = std::move(m_data[i + 1]);
		else
			for (size_t i = index; i < m_size; i++)
			{
				m_data[i] = m_data[i + 1];
				m_data[i + 1].~T();
			}
		m_size--;
	}


	template<typename T>
	inline const T& Array<T>::operator[](size_t index) const
	{
		return m_data[index];
	}


	template<typename T>
	inline T& Array<T>::operator[](size_t index)
	{
		return m_data[index];
	}


	template<typename T>
	void inline Array<T>::reserve(size_t newCapacity)
	{
		T* tmp = (T*)my_malloc(sizeof(T) * newCapacity);
		if (!tmp)
			throw std::bad_alloc();

		if constexpr (std::is_move_constructible<T>::value)
		{
			for (size_t i = 0; i < m_size; i++)
				new (&tmp[i]) T(std::move(m_data[i]));
		}
		else
		{
			for (size_t i = 0; i < m_size; i++)
			{
				new (&tmp[i]) T(m_data[i]);
				m_data[i].~T();
			}
		}

		my_free(m_data);
		m_data = tmp;
		m_capacity = newCapacity;
	}

}
