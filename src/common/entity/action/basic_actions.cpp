#include "basic_actions.h"
#include "util/memory/stack_allocator.h"

#ifdef USE_IMGUI
#include "imgui/imgui.h"
#endif

namespace action
{
	void addWalk(WalkTable& walkTable, ForcesTable& forcesTable, const WalkTable::Element& in_data, handle& out_forceId)
	{
		vec2 force = directionVectors[(int)in_data.direction] * in_data.velocity;
		out_forceId = forcesTable.add(force);
		walkTable.add(in_data.character, out_forceId, in_data.direction, in_data.velocity, in_data.duration);
	}

	void updateWalk(WalkTable& walkTable, ForcesTable& forcesTable, float deltaTime)
	{
		int i;
		for (i = 0; i < walkTable.count && walkTable.durations[i] > deltaTime; i++)
		{
			walkTable.durations[i] -= deltaTime;
		}
		handle* forcesToRemove = memory::allocators::stack.allocate<handle>(walkTable.count - i + 1);
		int newCount = i;
		
		walkTable.count = newCount;
	}

	void updateIdle(IdleTable& idleTable, float deltaTime)
	{
		for (int i = 0; i < idleTable.count; i++)
		{
			idleTable.durations[i] -= deltaTime;
		}
		int unfinishedActionsCount;
		for (unfinishedActionsCount = idleTable.count; unfinishedActionsCount > 1 && idleTable.durations[unfinishedActionsCount - 1] < 0; unfinishedActionsCount--);
		idleTable.count = unfinishedActionsCount + 1;
	}
}