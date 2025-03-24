#include "Widget.h"

/**
 * Set the position and dimensions.
 *
 * \param x	The x coordinate.
 * \param y	The y coordinate.
 * \param w	The width.
 * \param h The height.
 */
void Widget::startWidget(const char* name, bool& visible, int x, int y, int w, int h, int minW, int minH) {

    // Set widget name
    widgetName = (char*)name;

    // Check if the window is focused, and set text color accordingly
    if (isTitleFocused || isBodyFocused || isFooterFocused) {
        ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelTitleBarActiveText);  // Focused text color
        ImGui::PushStyleColor(ImGuiCol_Border, AppColors::panelBorderActive);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelTitleBarText);  // Inactive text color
        ImGui::PushStyleColor(ImGuiCol_Border, AppColors::panelBorder);
    }

    // Set the font
    ImGui::PushFont(AppFonts::title);

    // Set window position and size constraints
    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(minW, minH), ImVec2(w, h));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0,0,0,0.2f));

    ImGui::Begin(name, &visible, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetWindowPos();

    // Is ImGui::Begin() focused?
    isTitleFocused = ImGui::IsWindowFocused();

    // Reset style
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}

void Widget::startTabs(bool footer) {

    hasFooter = footer;
    ImGui::PushStyleColor(ImGuiCol_Border, AppColors::invisible);
    if (hasFooter) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, AppColors::panelBackground);
    }

    ImGui::SetCursorPos(ImVec2(0, 0));
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    pos = ImGui::GetCursorScreenPos();
    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 64), ImGui::ColorConvertFloat4ToU32(AppColors::panelFooter), 8, ImDrawFlags_RoundCornersBottom);
    ImGui::SetCursorPos(ImVec2(0, 0));

    ImGui::Dummy(ImVec2(0, 30));

    // Create name for pills element
    std::string pillsName = "##pills-" + std::string(widgetName);
    ImGui::BeginTabBar("Guest Tabs", ImGuiTabBarFlags_None);

    ImGui::Dummy(ImVec2(20, 10));  // Adds top-left padding
    ImGui::Indent(20);

}

void Widget::endTabs() {
    ImGui::Unindent(20);
    ImGui::EndTabBar();
    ImGui::PopStyleColor();
    if (hasFooter) {
        ImGui::PopStyleColor();
    }
}

void Widget::startBody(bool footer) {
    hasFooter = footer;
    ImGui::PushStyleColor(ImGuiCol_Border, AppColors::invisible);
    if (hasFooter) {
		ImGui::PushStyleColor(ImGuiCol_ChildBg, AppColors::panelBackground);
	}
    ImGui::BeginChild("##body", ImVec2(size.x, (footer ? size.y-58 : size.y)));
    ImGui::Dummy(ImVec2(20, 10));  // Adds top-left padding
    ImGui::Indent(20);
    isBodyFocused = ImGui::IsWindowFocused();
}

void Widget::endBody() {
    ImGui::Unindent(20);
	ImGui::EndChild();
    ImGui::PopStyleColor();
    if (hasFooter) {
        ImGui::PopStyleColor();
    }
}

/**
 * Start widget footer.
 *
 */
void Widget::startFooter() {
    ImGui::PushStyleColor(ImGuiCol_Border, AppColors::primary);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    size = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("##footer", ImVec2(size.x, 58), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImDrawFlags_RoundCornersBottom);
    isFooterFocused = ImGui::IsWindowFocused();
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    pos = ImGui::GetCursorScreenPos();
    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 52), ImGui::ColorConvertFloat4ToU32(AppColors::panelFooter), 8, ImDrawFlags_RoundCornersBottom);
	ImGui::PopStyleVar();
	ImGui::Indent(10);
	ImGui::Dummy(ImVec2(10, 6));  // Adds top-left padding
}

/**
 * Stop widget footer.
 *
 */
void Widget::endFooter() {
    ImGui::Unindent(10);
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

/**
 * End the widget.
 *
 */
void Widget::endWidget() {
	ImGui::End();
    ImGui::PopFont();
    ImGui::PopStyleColor();
}

void Widget::elLabel(std::string label) {
    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formLabel);
    ImGui::SetNextItemWidth(size.x - 40);
	ImGui::Text(label.c_str());
	ImGui::PopStyleColor();
    ImGui::PopFont();
}

void Widget::elHelp(std::string help) {
    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + size.x - 40);
	ImGui::TextWrapped(help.c_str());
    ImGui::PopStyleColor();
	ImGui::PopFont();
}

void Widget::elParagraph(std::string text) {
    ImGui::PushFont(AppFonts::input);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + size.x - 40);
    ImGui::TextWrapped(text.c_str());
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0, 10.0f));
}

bool Widget::elText(std::string label, char* buffer, std::string help) {

    bool response = false;
    ImVec2 size = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    if (!label.empty()) {
        elLabel(label);
	}
    ImGui::SetNextItemWidth(size.x-20);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
    if (ImGui::InputText(inputLabel.c_str(), buffer, 256)) {
        response = true;
    }
    else {
        response = false;
    }
    ImGui::PopStyleColor();

    ImGui::SetNextItemWidth(size.x-20);
    elHelp(help);

    if (help != "") {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return response;

}

bool Widget::elPassword(std::string label, char* buffer, std::string help) {

    bool response = false;
    ImVec2 size = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    if (!label.empty()) {
        elLabel(label);
    }
    ImGui::SetNextItemWidth(size.x - 20);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
    if (ImGui::InputText(inputLabel.c_str(), buffer, 256, ImGuiInputTextFlags_Password)) {
        response = true;
    }
    else {
        response = false;
    }
    ImGui::PopStyleColor();

    ImGui::SetNextItemWidth(size.x - 20);
    elHelp(help);

    if (help != "") {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return response;

}

bool Widget::elNumber(std::string label, int& value, int from, int to, std::string help) {

    bool response = false;
    ImVec2 size = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    elLabel(label);
    ImGui::SetNextItemWidth(size.x-20);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
    if (IntRangeWidget::render(label.c_str(), value, from, to, 0.025f)) {
        response = true;
    }
    else {
        response = false;
    }
    ImGui::PopStyleColor();

    ImGui::SetNextItemWidth(size.x-20);
    elHelp(help);

    if (help != "") {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return response;

}

bool Widget::elTextArea(std::string label, char* buffer, std::string help) {

    bool response = false;
    ImVec2 size = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    elLabel(label);
    ImGui::SetNextItemWidth(size.x-20);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
    if (ImGui::InputTextMultiline(inputLabel.c_str(), buffer, 500)) {
        response = true;
    }
    else {
        response = false;
    }
    ImGui::PopStyleColor();

    ImGui::SetNextItemWidth(size.x-20);
    elHelp(help);

    if (help != "") {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return response;

}

bool Widget::elCheckbox(std::string label, bool& isOn, std::string help) {

    bool response = false;
    ImVec2 size = ImGui::GetContentRegionAvail();

    ImGui::PushFont(AppFonts::label);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formLabel);
    ImGui::SetNextItemWidth(size.x-20);
    if (ImGui::Checkbox(label.c_str(), &isOn)) {
        response = true;
    }
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::SetNextItemWidth(size.x-20);
    elHelp(help);

    if (help != "") {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return response;

}

bool Widget::elSelect(std::string label,
    std::vector<std::pair<std::string, std::string>> options,
    string& callback, std::string help) {

    ImVec2 size = ImGui::GetContentRegionAvail();
    std::string inputLabel = "##" + label;

    elLabel(label);
    ImGui::SetNextItemWidth(size.x-20);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);

    bool itemSelected = false;

    // Get selected option
    std::string selected = "";
    for (size_t i = 0; i < options.size(); ++i) {
        if (options[i].first == callback) {
			selected = options[i].second.c_str();
			break;
		}
	}

    ImGui::PushStyleColor(ImGuiCol_Border, AppColors::formInputBorder);
    ImGui::PushStyleColor(ImGuiCol_Button, AppColors::buttonPrimary);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, AppColors::buttonPrimaryActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AppColors::buttonPrimaryHovered);
    if (ImGui::BeginCombo(inputLabel.c_str(), selected.c_str(), ImGuiComboFlags_HeightLarge)) {
        for (size_t i = 0; i < options.size(); ++i) {
            bool isSelected = (options[i].first == callback);
            if (ImGui::Selectable(options[i].second.c_str(), isSelected)) {
                callback = options[i].first;
                itemSelected = true; // Store selection state
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    ImGui::SetNextItemWidth(size.x-20);
    elHelp(help);

    if (!help.empty()) {
        ImGui::Dummy(ImVec2(0, 10.0f));
    }

    return itemSelected; // Return after everything is drawn
}

bool Widget::elBtn(std::string label) {
    ImGui::PushStyleColor(ImGuiCol_Button, AppColors::buttonPrimary);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, AppColors::buttonPrimaryActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AppColors::buttonPrimaryHovered);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::buttonPrimaryText);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));

    if (ImGui::Button(label.c_str())) {

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        return true;
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    return false;
}

bool Widget::elBtnSecondary(std::string label) {
	ImGui::PushStyleColor(ImGuiCol_Button, AppColors::buttonSecondary);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, AppColors::buttonSecondaryActive);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AppColors::buttonSecondaryHovered);
    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::buttonSecondaryText);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));
    if (ImGui::Button(label.c_str())) {

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		return true;
	}

    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	return false;
}
