#include "free_list_allocator.h"

namespace memory
{
	void FreeListAllocator::getEmptySlots(size_t* outOffsets, int* outSizes, int& count)
	{
		int outCursor = 0;
		EmptySlot* dataCursor = head;
		while (dataCursor != nullptr && outCursor < count)
		{
			outOffsets[outCursor] = (size_t)((char*)dataCursor - data);
			outSizes[outCursor] = dataCursor->size;
			dataCursor = dataCursor->next;
			outCursor++;
		}
		count = outCursor;
	}
	namespace allocators
	{
		FreeListAllocator freeList(1000000);
	}
}