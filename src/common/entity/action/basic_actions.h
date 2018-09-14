#pragma once

#include "util/handle.h"
#include "util/geometry/vec.h"
#include "util/array/sorted.h"
#include "../movement.h"

#define ACTIONLIST_MAX 500

namespace action
{
	enum class ActionType : unsigned char
	{
		None,
		Idle,
		Movement,
		Count
	};

	struct BasicAction
	{
		handle actionId;
		ActionType Type;
	};

	enum class Direction
	{
		N, E, S, W, COUNT
	};

	constexpr vec2 directionVectors[] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };
	constexpr char* directionNames[] = { "N", "E", "S", "W", "INVALID" };

	struct MovementAction
	{
		vec2 velocity;
		float duration;
	};

	// Action tables are sorted by duration
	struct WalkTable : Table<handle, handle, action::Direction, float, float>
	{
		TABLE_CONSTRUCTOR(WalkTable, handle, handle, action::Direction, float, float);

		handle* characters = get<0>();
		handle* forces = get<1>();
		action::Direction* direction = get<2>();
		float* velocity = get<3>();
		float* durations = get<4>();

		struct Element
		{
			handle character;
			handle forceId;
			action::Direction direction;
			float velocity;
			float duration;
		};

		TABLE_DEBUG_COLUMNS("characters", "forces", "directions", "velocities", "durations")
	};

	struct IdleTable : Table<float, handle>
	{
		TABLE_CONSTRUCTOR(IdleTable, float, handle);

		float* durations = get<0>();
		handle* characters = get<1>();

#ifdef USE_IMGUI
		char* columns[2] = { "durations", "characters" };
		virtual char** getColumnNames() { return columns; }
#endif
	};

	void updateWalk(WalkTable& walkTable, ForcesTable& forcesTable, float deltaTime);
	void updateIdle(IdleTable& idleTable, float deltaTime);
}
