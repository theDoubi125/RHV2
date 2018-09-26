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

	using WalkTableType = Table<handle, handle, action::Direction, float, float>;
	// Action tables are sorted by duration
	struct WalkTable : WalkTableType
	{
		TABLE_CONSTRUCTOR(WalkTable, WalkTableType);

		void assignBindings(MovementTable& movement, ForcesTable& forces)
		{
			entityBinding = &movement.addBinding();
			forcesBinding = &forces.addBinding();
		}

		void updateBindings()
		{
			entityBinding->update<WalkTable, 0>(*this);
			forcesBinding->update<WalkTable, 1>(*this);
		}

		handle* characters = get<0>();
		handle* forces = get<1>();
		action::Direction* direction = get<2>();
		float* velocity = get<3>();
		float* durations = get<4>();

		TableBinding<100>* entityBinding;
		TableBinding<100>* forcesBinding;

		struct Element
		{
			handle character;
			action::Direction direction;
			float velocity;
			float duration;
		};

		TABLE_DEBUG_COLUMNS("characters", "forces", "directions", "velocities", "durations")
	};

	using IdleTableType = Table<float, handle>;
	struct IdleTable : IdleTableType
	{
		TABLE_CONSTRUCTOR(IdleTable, IdleTableType);

		float* durations = get<0>();
		handle* characters = get<1>();

#ifdef USE_IMGUI
		char* columns[2] = { "durations", "characters" };
		virtual char** getColumnNames() { return columns; }
#endif
	};

	void addWalk(WalkTable& walkTable, ForcesTable& forcesTable, const WalkTable::Element& in_data, handle& out_forceId);
	void updateWalk(WalkTable& walkTable, ForcesTable& forcesTable, float deltaTime);
	void updateIdle(IdleTable& idleTable, float deltaTime);
}
