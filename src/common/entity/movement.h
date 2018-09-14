#pragma once
#include "util/geometry/vec.h"
#include "util/memory/free_list_allocator.h"
#include "util/memory/stack_allocator.h"
#include "util/handle.h"
#include "util/array/sorted.h"
#include "util/array/table.h"

#define TABLE_CONSTRUCTOR(ClassName, ...) template<typename Allocator> ClassName(Allocator& allocator, int capacity) : Table<__VA_ARGS__>(allocator, capacity) {}
#define HANDLE_TABLE_CONSTRUCTOR(ClassName, ...) template<typename Allocator> ClassName(Allocator& allocator, int capacity) : HandleTable<__VA_ARGS__>(allocator, capacity) {}

#ifdef USE_IMGUI
#define TABLE_DEBUG_COLUMNS(...) char* columns[100] = { __VA_ARGS__ }; virtual char** getColumnNames() { return columns; }
#else
#define TABLE_DEBUG_COLUMNS(...)
#endif

#ifdef USE_IMGUI
#include "imgui/imgui.h"
#endif

struct ForcesTable : HandleTable<vec2>
{
	HANDLE_TABLE_CONSTRUCTOR(ForcesTable, vec2);
	TABLE_DEBUG_COLUMNS("characters", "forces")

	handle* characters = get<0>();
	vec2* forces = get<1>();
};

struct MovementTable : HandleTable<vec2, vec2, float>
{
	HANDLE_TABLE_CONSTRUCTOR(MovementTable, vec2, vec2, float);
	TABLE_DEBUG_COLUMNS("characters", "positions", "velocities", "masses")

	handle* characters = get<0>();
	vec2* positions = get<1>();
	vec2* velocities = get<2>();
	float* masses = get<3>();
};

void updateMovement(MovementTable& movement, float deltaTime);
void applyForces(MovementTable& movementTable, ForcesTable& forcesTable, float deltaTime);
