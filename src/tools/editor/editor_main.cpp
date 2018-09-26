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

	struct TestTable : public HandleTable<int>
	{
		TABLE_CONSTRUCTOR(TestTable, HandleTable<int>)
		TABLE_DEBUG_COLUMNS("data", "bla")
	};

	void drawIteratorEditor()
	{
		
		static TestTable data(::memory::MallocAllocator(), 100);
		static TestTable::ElementType dataToAdd;
		static TestTable toRemove(::memory::MallocAllocator(), 100);
		static TestTable::ElementType dataToRemove;
		if (ImGui::Begin("Iterators"))
		{
			data.showEditor("Test Data", &dataToAdd);
			toRemove.showEditor("To Remove", &dataToRemove);
			if (ImGui::Button("Remove"))
			{
				data.removeAll(toRemove.first, toRemove.count);
			}
			ImGui::End();
		}
	}

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

		static TableBinding<100>* walkBinding;
		static bool initialized = false;

		if (!initialized)
		{
			forcesTable.assignBindings(movementManager);
			walkTable.assignBindings(movementManager, forcesTable);
			walkBinding = &movementManager.addBinding();
			initialized = true;
		}

		drawIteratorEditor();
		
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
				static action::WalkTable::Element newAction;
				if (ImGui::Button("Add Walk"))
				{
					handle forceId;
					action::addWalk(walkTable, forcesTable, newAction, forceId);
				}
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
			forcesTable.updateBindings();
			walkTable.updateBindings();
			ImGui::End();
		}
		drawWorldPreview(movementManager, displayConfig);
	}
}