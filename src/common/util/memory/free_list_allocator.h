#pragma once

#ifdef _DEBUG
#include <iostream>
#endif

namespace memory
{
	class MallocAllocator
	{
	public:
		template<typename T>
		T* allocate(int size)
		{
			return new T[size];
		}
	};

	class FreeListAllocator
	{
	private:
		struct SlotHeader { int size; };
		struct EmptySlot { int size; EmptySlot* next; };
	public:
		FreeListAllocator(int size)
		{
			data = new char[size];
			head = (EmptySlot*)data;
			head->size = size;
			head->next = nullptr;
		}

		~FreeListAllocator()
		{
			delete data;
		}

		template<typename T>
		T* allocate(size_t size)
		{
			EmptySlot* previous = nullptr;
			EmptySlot* cursor = head;
			while (cursor != nullptr && (size_t)cursor->size < size * sizeof(T) + sizeof(SlotHeader))
			{
				previous = cursor;
				cursor = cursor->next;
			}
			if (cursor == nullptr)
			{
				std::cerr << "Error : free list allocator full" << std::endl;
				return nullptr;
			}
			
			if (cursor->size - size * sizeof(T) - 2 * sizeof(SlotHeader) > 0)
			{
				SlotHeader* newSlotHeader = (SlotHeader*)cursor;
				EmptySlot* newEmptySlot = (EmptySlot*)(((char*)newSlotHeader) + sizeof(SlotHeader) + size * sizeof(T));
				// free space after alloc -> new slot created
				if (previous == nullptr)
				{
					head = newEmptySlot;
				}
				else
				{
					previous->next = newEmptySlot;
				}
				newEmptySlot->size = cursor->size - size * sizeof(T) - sizeof(SlotHeader);
				newEmptySlot->next = cursor->next;
				newSlotHeader->size = size * sizeof(T) + sizeof(SlotHeader);
				return (T*)(((char*)newSlotHeader) + sizeof(SlotHeader));
			}
			else
			{
				// full slot used
				if (previous != nullptr)
					previous->next = cursor->next;
				SlotHeader* newSlotHeader = (SlotHeader*)cursor;
				newSlotHeader->size = cursor->size;
				return (T*)(((char*)newSlotHeader) + sizeof(SlotHeader));
			}
		}

		template<typename T>
		void free(T* element)
		{
			SlotHeader* slotToRemove = (SlotHeader*)((char*)element - sizeof(SlotHeader));
			EmptySlot* newEmptySlot = (EmptySlot*)slotToRemove;
			newEmptySlot->size = slotToRemove->size;
			EmptySlot* slotsCursor = head;
			EmptySlot* previousSlot = nullptr;

			while (slotsCursor != nullptr && slotsCursor < newEmptySlot)
			{
				previousSlot = slotsCursor;
				slotsCursor = slotsCursor->next;
			}
			bool bMergeWithPrevious = false;
			if (previousSlot != nullptr && getOffset(previousSlot) + previousSlot->size == getOffset(newEmptySlot))
			{
				bMergeWithPrevious = true;
				previousSlot->size = getOffset(newEmptySlot) + newEmptySlot->size - getOffset(previousSlot);
				newEmptySlot = previousSlot;
			}
			while (slotsCursor != nullptr && getOffset(slotsCursor) == getOffset(newEmptySlot) + newEmptySlot->size)
			{
				newEmptySlot->size += slotsCursor->size;
				newEmptySlot->next = slotsCursor->next;
				slotsCursor = slotsCursor->next;
			}
			if(!bMergeWithPrevious)
			{
				newEmptySlot->next = slotsCursor;
				if (previousSlot == nullptr)
				{
					head = newEmptySlot;
				}
				else previousSlot->next = newEmptySlot;
			}
		}

		void getEmptySlots(size_t* outOffsets, int* outSizes, int& count);

		template<typename T>
		size_t getOffset(T* element)
		{
			return (size_t)((char*)element - data);
		}

	private:
		char* data;
		EmptySlot* head;
	};

	namespace allocators
	{
		extern FreeListAllocator freeList;
	}
}