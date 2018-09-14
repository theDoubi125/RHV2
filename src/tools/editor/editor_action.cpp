#include "editor_action.h"
#include "entity/action/basic_actions.h"
#include "entity/movement.h"
#include "imgui/imgui.h"


namespace editor
{
	void drawWorldPreview(MovementTable& movementTable, WorldDisplayConfig& config)
	{
		if (ImGui::Begin("World Display", nullptr, ImGuiWindowFlags_NoTitleBar))
		{
			vec2 min = ImGui::GetWindowPos() + vec2(5, 0);
			vec2 max = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMax();
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRect(min, max, ImColor(1.0f, 1.0f, 1.0f), 10);
			for (int i = 0; i < movementTable.count; i++)
			{
				drawList->AddCircle(min + movementTable.positions[i] * config.scale, config.scale, ImColor(1.0f, 1.0f, 1.0f));
			}
			ImGui::End();
		}
	}
}