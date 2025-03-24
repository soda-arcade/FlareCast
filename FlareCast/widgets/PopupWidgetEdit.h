﻿#pragma once

#include <string>
#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../globals/AppColors.h"
#include "IconButton.h"

class PopupWidgetEdit
{
public:
	static bool render(const char* title, bool& showPopup, std::string &oldReason);
};




