#pragma once

namespace array
{
	template<typename Allocator, typename T>
	void allocate(Allocator& allocator, T*& array, int size)
	{
		array = allocator.allocate<T>(size);
	}

	template<typename Allocator, typename T, typename... Types>
	void allocate(Allocator& allocator, T*& array1, Types*&... arrays, int size)
	{
		allocate<Allocator, T>(allocator, array1, size);
		allocate<Allocator, Types...>(allocator, arrays..., size);
	}

	template<typename T>
	bool contains(T* array, int size, const T& element)
	{
		for (int i = 0; i < size; i++)
		{
			if (element == array[i])
				return true;
		}
		return false;
	}

	template<typename T>
	int find(T* array, int size, const T& element)
	{
		for (int i = 0; i < size; i++)
		{
			if (element == array[i])
				return i;
		}
		return -1;
	}

	template<typename T>
	int getSortInsertIndex(T* array, int size, const T& toAdd)
	{
		for (int i = 0; i < size; i++)
		{
			if (array[i] > toAdd)
			{
				return i;
			}
		}
		return size;
	}

	template<typename T>
	void insertAt(T* array, int size, int index, const T& value)
	{
		T buffer = value;
		T copyBuffer;
		for (int i = index; i <= size; i++)
		{
			copyBuffer = array[i];
			array[i] = buffer;
			buffer = copyBuffer;
		}
	}

	template<typename T, typename... Types>
	void insertAt(T* array, Types*... arrays, int size, int insertIndex, const T& value, const Types&... values)
	{
		insertAt<T>(array, size, insertIndex, value);
		insertAt<Types...>(arrays..., size, insertIndex, values...);
	}


	// insertion sort with the first array used as the index
	template<typename T, typename... Types>
	void insert(T* indexArray, Types*... arrays, int size, const T& value, const Types&... values)
	{
		int index = getSortInsertIndex(indexArray, size, value);
		insertAt<T, Types...>(indexArray, arrays..., size, index, value, values...);
	}

	template<typename T>
	void removeAt(T* array, int size, int index)
	{
		for (int i = index; i < size - 1; i++)
		{
			array[i] = array[i + 1];
		}
	}

	template<typename T, typename... Types>
	void removeAt(T* indexArray, Types*... arrays, int size, int index)
	{
		removeAt(indexArray, size, index);
		removeAt<Types...>(arrays..., size, index);
	}

	template<typename T, typename... Types>
	void remove(T* indexArray, Types*... arrays, int size, const T& value)
	{
		int index = find(indexArray, size, value);
		removeAt<T, Types...>(indexArray, arrays..., size, index);
	}
}