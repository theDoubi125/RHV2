#pragma once
#include "util/geometry/vec.h"
#include "util/memory/free_list_allocator.h"
#include "util/memory/stack_allocator.h"
#include "util/handle.h"
#include "util/array/sorted.h"
#include "util/array/table.h"



#ifdef USE_IMGUI
#include "imgui/imgui.h"
#endif

struct RemovedTable
{
	handle* toRemove;

};

struct MovementTable;

using ForcesTableType = HandleTable<vec2>;
struct ForcesTable : ForcesTableType
{
	TABLE_CONSTRUCTOR(ForcesTable, ForcesTableType);
	TABLE_DEBUG_COLUMNS("characters", "forces")

	void assignBindings(MovementTable& movementTable);
	void updateBindings();

	handle* characters = get<0>();
	vec2* forces = get<1>();
	TableBinding<100>* entityBinding;
};

using MovementTableType = HandleTable<vec2, vec2, float>;
struct MovementTable : MovementTableType
{
	TABLE_CONSTRUCTOR(MovementTable, MovementTableType);
	TABLE_DEBUG_COLUMNS("characters", "positions", "velocities", "masses")

	handle* characters = get<0>();
	vec2* positions = get<1>();
	vec2* velocities = get<2>();
	float* masses = get<3>();

	handle* removed;
	int removedCount = 0;

};

void updateMovement(MovementTable& movement, float deltaTime);
void removeMovements(MovementTable& movement, handle* handle, int count);
void applyForces(MovementTable& movementTable, ForcesTable& forcesTable, float deltaTime);
