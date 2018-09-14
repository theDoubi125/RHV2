#pragma once

#include "entity/movement.h"

namespace editor
{
	struct WorldDisplayConfig
	{
		vec2 windowSize = vec2(800, 600);
		float scale = 10;
	};
	void drawWorldPreview(MovementTable& movementTable, WorldDisplayConfig& config);
}