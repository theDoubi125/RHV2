#include "editor_main.h"
#include "imgui/imgui.h"
#include "util/memory/free_list_allocator.h"
#include "util/memory/stack_allocator.h"
#include "editor/memory/editor_memory.h"
#include "entity/movement.h"
#include "entity/action/basic_actions.h"
#include "util/array/table.h"
#include "util/array/join.h"
#include "editor_action.h"

namespace editor
{
	bool showEditor = true;

	bool pArraySelected = false;
	bool aArraySelected = false;
	bool testSelected = false;
	bool idleSelected = false;
	bool demoWindowSelected = false;
	bool moveSelected = false;
	bool dispatcherSelected = false;
	bool pathfindSelected = false;
	bool collisionSelected = true;

	void drawEditor(float deltaTime)
	{
		memory::drawStackAllocatorEditor();
		memory::drawListAllocatorEditor();
		static MovementTable movementManager(::memory::MallocAllocator(), 20);
		static MovementTable::ElementType movementManagerElement({ 0 }, vec2(0, 0), vec2(0, 0), 0);

		static ForcesTable forcesTable(::memory::MallocAllocator(), 20);
		static ForcesTable::ElementType forceTableElement({ 0 }, vec2(0, 0));

		static action::WalkTable walkTable(::memory::MallocAllocator(), 20);
		static action::WalkTable::ElementType walkTableElement({ 0 }, { 0 }, action::Direction::E, 0, 0);
		static WorldDisplayConfig displayConfig;

		static float stepDeltaTime = 1;
		
		if (ImGui::Begin("Tables"))
		{
			if (ImGui::CollapsingHeader("MovementManager"))
			{
				movementManager.showEditor("Movement Manager", &movementManagerElement);
			}
			if (ImGui::CollapsingHeader("Walk"))
			{
				walkTable.showEditor("Walk Table", &walkTableElement);
			}
			if (ImGui::CollapsingHeader("Forces"))
			{
				forcesTable.showEditor("Forces Table", &forceTableElement);
			}
			if (ImGui::CollapsingHeader("Transforms"))
			{
				if (ImGui::Button("Update walk"))
				{
					action::updateWalk(walkTable, forcesTable, stepDeltaTime);
				}
				if (ImGui::Button("Update Forces"))
				{
					applyForces(movementManager, forcesTable, stepDeltaTime);
				}
				if (ImGui::Button("Update movement"))
				{
					updateMovement(movementManager, stepDeltaTime);
				}
			}
			ImGui::End();
		}
		drawWorldPreview(movementManager, displayConfig);
	}
}