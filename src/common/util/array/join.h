#pragma once

#include "util/memory/stack_allocator.h"

namespace array
{
	template<typename T>
	void getJoinSortedIndexes(T* A, int ASize, T* B, int BSize, int* outAIndex, int* outBIndex, int& outSize)
	{
		outSize = 0;
		int cursorA = 0;
		int cursorB = 0;
		while (cursorA < ASize && cursorB < BSize)
		{
			if (A[cursorA] == B[cursorB])
			{
				// handle duplicates
				int lastDuplicateA, lastDuplicateB;
				for (lastDuplicateA = cursorA + 1; lastDuplicateA < ASize && A[lastDuplicateA] == A[cursorA]; lastDuplicateA++);
				for (lastDuplicateB = cursorB + 1; lastDuplicateB < BSize && B[lastDuplicateB] == B[cursorB]; lastDuplicateB++);
				for (int i = cursorA; i < lastDuplicateA; i++)
				{
					for (int j = cursorA; j < lastDuplicateB; j++)
					{
						outAIndex[outSize] = i;
						outBIndex[outSize] = j;
						outSize++;
					}
				}
				cursorA = lastDuplicateA;
				cursorB = lastDuplicateB;
			}
			else if (A[cursorA] < B[cursorB])
			{
				cursorA++;
			}
			else cursorB++;
		}
	}

	template<typename IndexType>
	void quickSort(IndexType* index, int* out_indexes, int count) // out_indexes must be initialized to t[i] = i
	{
		auto& allocator = memory::allocators::stack;
		IndexType pivot = index[count - 1];

		int cursor = 0;
		IndexType indexBuffer;
		int outBuffer;
		for (int i = 0; i < count; i++)
		{
			if (index[i] < pivot)
			{
				outBuffer = out_indexes[i];
				out_indexes[i] = out_indexes[cursor];
				out_indexes[cursor] = outBuffer;

				indexBuffer = index[i];
				index[i] = index[cursor];
				index[cursor] = indexBuffer;
				cursor++;
			}
		}
		if (count > 0)
		{
			outBuffer = out_indexes[count - 1];
			out_indexes[count - 1] = out_indexes[cursor];
			out_indexes[cursor] = outBuffer;

			index[count - 1] = index[cursor];
			index[cursor] = pivot;
		}

		if (cursor > 1)
		{
			quickSort(index, out_indexes, cursor);
		}
		if (count - cursor > 1)
		{
			quickSort(index + cursor + 1, out_indexes + cursor + 1, count - cursor - 1);
		}
	}

	template<typename FirstType, typename... Rest>
	void reorder(int* order, FirstType* first, Rest*... rest, int count)
	{
		auto& allocator = memory::allocators::stack;
		allocator.pushStack();
		FirstType* buffer = allocator.allocate<FirstType>(count);
		for (int i = 0; i < count; i++)
		{
			buffer[i] = first[order[i]];
		}
		for (int i = 0; i < count; i++)
		{
			first[i] = buffer[i];
		}
		allocator.popStack();
		reorder<Rest...>(order, rest..., count);
	}

	template<typename FirstType>
	void reorder(int* order, FirstType* first, int count)
	{
		auto& allocator = memory::allocators::stack;
		allocator.pushStack();
		FirstType* buffer = allocator.allocate<FirstType>(count);
		for (int i = 0; i < count; i++)
		{
			buffer[i] = first[order[i]];
		}
		for (int i = 0; i < count; i++)
		{
			first[i] = buffer[i];
		}
		allocator.popStack();
	}

	template<typename IndexType, typename... Rest>
	void quickSort(IndexType* index, Rest*... arrays, int count)
	{
		auto& allocator = memory::allocators::stack;
		allocator.pushStack();
		int* sortedIndex = allocator.allocate<int>(count);
		for (int i = 0; i < count; i++)
			sortedIndex[i] = i;
		quickSort(index, sortedIndex, count);
		reorder<Rest...>(sortedIndex, arrays..., count);
		allocator.popStack();
	}

	template<typename IndexType>
	void quickSort(IndexType* index, int count)
	{
		auto& allocator = memory::allocators::stack;
		allocator.pushStack();
		int* sortedIndex = allocator.allocate<int>(count);
		for (int i = 0; i < count; i++)
			sortedIndex[i] = i;
		quickSort(index, sortedIndex, count);
		allocator.popStack();
	}

	// add all content of toAdd without the elements already inside target. All arrays must be sorted
	template<typename IndexType>
	void addAllUnique(IndexType* target, int& io_targetCount, IndexType* toAdd, int toAddCount)
	{
		int targetCursor = 0;
		int toAddCursor = 0;
		for (;toAddCursor < toAddCount, toAddCursor++)
		{
			for (; targetCursor < io_targetCount && toAddCursor < toAddCount && target[targetCursor] < toAdd[toAddCursor]; targetCursor++);
			if (targetCursor >= io_targetCount || target[targetCursor] != toAdd[toAddCursor])
			{
				insertAt(target, io_targetCount, targetCursor, toAdd[toAddCursor]);
				io_targetCount++;
			}
		}
	}
}