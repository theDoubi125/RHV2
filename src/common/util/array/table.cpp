#include "util/memory/free_list_allocator.h"
#include "table.h"
#include "entity/action/basic_actions.h"

template<>
bool showElementEditor<int>(char* label, int* element)
{
	return ImGui::DragInt(label, element);

}

template<>
bool showElementEditor<handle>(char* label, handle* element)
{
	return ImGui::DragInt(label, (int*)&(element->id));

}

template<>
bool showElementEditor<float>(char* label, float* element)
{
	return ImGui::DragFloat(label, element);
}

template<>
bool showElementEditor<ivec2>(char* label, ivec2* element)
{
	return ImGui::DragInt2(label, (int*)element);
}

template<>
bool showElementEditor<vec2>(char* label, vec2* element)
{
	return ImGui::DragFloat2(label, (float*)element);
}

template<>
bool showElementEditor<action::Direction>(char* label, action::Direction* dir)
{
	char labelBuffer[100];
	if (ImGui::BeginCombo(label, action::directionNames[(int)*dir]))
	{
		ImGui::PushID(label);
		for (int i = 0; i < (int)action::Direction::COUNT; i++)
		{
			sprintf_s(labelBuffer, "%s%s", label, action::directionNames[(int)*dir]);
			if (ImGui::Selectable(action::directionNames[i], (int)*dir == i))
			{
				*dir = (action::Direction)i;
			}
		}
		ImGui::PopID();
		ImGui::EndCombo();
	}
	return true;
}