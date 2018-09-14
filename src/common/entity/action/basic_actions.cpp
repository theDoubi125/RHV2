#include "basic_actions.h"
#include "util/memory/stack_allocator.h"

#ifdef USE_IMGUI
#include "imgui/imgui.h"
#endif

namespace action
{
	void addWalk(WalkTable& walkTable, ForcesTable& forcesTable, WalkTable::Element data)
	{
		vec2 force = directionVectors[(int)data.direction] * data.velocity;
		handle forceId = forcesTable.add(force);
		walkTable.add(data.character, data.forceId, data.direction, data.velocity, data.duration);
	}

	void updateWalk(WalkTable& walkTable, ForcesTable& forcesTable, float deltaTime)
	{
		int i;
		for (i = 0; i < walkTable.count && walkTable.durations[i] > deltaTime; i++)
		{
			walkTable.durations[i] -= deltaTime;
		}
		for (; i < walkTable.count; i++)
		{

		}
		int unfinishedActionsCount;
		for (unfinishedActionsCount = walkTable.count; unfinishedActionsCount > 0 && walkTable.durations[unfinishedActionsCount - 1] < 0; unfinishedActionsCount--)
		{
		}
		walkTable.count = unfinishedActionsCount;
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