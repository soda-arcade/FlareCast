﻿#include "BoolButtonWidget.h"

bool BoolButtonWidget::render(const char* label, bool& value)
{
    static float width;

    ImGui::BeginGroup();
    ImGui::Text(label);
    width = ImGui::GetItemRectSize().x;
    ImGui::Dummy(ImVec2(0.5f * width - 29.0f, 0.0f));
    ImGui::SameLine();
    if (ToggleIconButtonWidget::render(AppIcons::yes, AppIcons::no, value, ImVec2(24.0f, 24.0f)))
    {
        value = !value;
    }
    ImGui::EndGroup();

	return true;
}



