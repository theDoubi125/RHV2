#include "util/memory/stack_allocator.h"
#include "util/memory/free_list_allocator.h"
#include "imgui/imgui.h"
#include <vector>
#include <map>
#include <iostream>

namespace editor
{
	namespace memory
	{
		void drawStackAllocatorEditor()
		{
			static ::memory::StackAllocator allocator(1000000, 1000);
			allocator.pushStack();
			int* intArray = allocator.allocate<int>(10);
			float* floatArray = allocator.allocate<float>(10);
			for (int i = 0; i < 10; i++)
			{
				intArray[i] = i;
				floatArray[i] = ((float)i) / 10;
			}
			allocator.popStack();
		}

		void drawListAllocatorEditor()
		{
			if (ImGui::Begin("List Allocator"))
			{
				static ::memory::FreeListAllocator allocator(1000000);
				static const int testCount = 100;
				static char* allocated[testCount];
				static int allocatedSizes[testCount];
				static int allocatedCount;
				size_t positions[testCount];
				int sizes[testCount];
				int count = testCount;
				allocator.getEmptySlots(positions, sizes, count);
				ImGui::Text("Allocator : ");
				for (int i = 0; i < count; i++)
				{
					ImGui::SameLine();
					ImGui::Text("(%d => %d : %d)", positions[i], positions[i] + sizes[i], sizes[i]);
				}

				for (int i = 0; i < allocatedCount; i++)
				{
					char displayText[100];
					sprintf_s(displayText, "(%d => %d  : %d)", allocator.getOffset(allocated[i]), allocator.getOffset(allocated[i] + allocatedSizes[i]), allocatedSizes[i]);
					if(i > 0)
						ImGui::SameLine();
					if (ImGui::Button(displayText))
					{
						allocator.free(allocated[i]);
						for (int j = i + 1; j < allocatedCount; j++)
						{
							allocated[j - 1] = allocated[j];
							allocatedSizes[j - 1] = allocatedSizes[j];
						}
						allocatedCount--;
					}
				}

				static int allocationSize = 50;
				ImGui::DragInt("Allocation Size", &allocationSize, 0.1f, 1, 100);
				if (ImGui::Button("Allocate"))
				{
					allocated[allocatedCount] = allocator.allocate<char>(allocationSize);
					allocatedSizes[allocatedCount] = allocationSize;
					allocatedCount++;
				}
				ImGui::End();
			}

		}
	}
}
