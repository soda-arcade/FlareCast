#pragma once
#include "imgui.h"
#include <string>
#include <vector>
#include "../globals/AppColors.h"
#include "../globals/AppFonts.h"
#include "IntRangeWidget.h"

class Widget {
public:
	char* widgetName;
	bool isFocused = false;
	bool isTitleFocused = false;
	bool isBodyFocused = false;
	bool isFooterFocused = false;
	bool hasFooter = false;
	int activeTab = 0;
	ImVec2 size = ImVec2(0, 0);
	ImVec2 pos = ImVec2(0, 0);

	void startWidget(const char* name, bool& visible, int x, int y, int w, int h, int minW = 350, int minH = 350);
	void startTabs(bool footer = false);
	void endTabs();
	void startBody(bool footer = false);
	void endBody();
	void startFooter();
	void endFooter();
	void endWidget();

	void elLabel(std::string label);
	void elHelp(std::string help);
	void elParagraph(std::string text);
	bool elText(std::string label, char* buffer, std::string help = "");
	bool elPassword(std::string label, char* buffer, std::string help = "");
	bool elSelect(std::string label, std::vector<std::pair<std::string, std::string>> options,
		string& callback, std::string help = "");
	bool elTextArea(std::string label, char* buffer, std::string help = "");
	bool elCheckbox(std::string label, bool& isOn, std::string help = "");
	bool elNumber(std::string label, int& value, int from, int to, std::string help = "");
	bool elBtn(std::string label);
	bool elBtnSecondary(std::string label);
};