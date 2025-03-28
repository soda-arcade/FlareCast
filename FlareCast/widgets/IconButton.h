﻿#pragma once

#include <d3d11.h>
#include "../imgui/imgui.h"
#include "../globals/AppIcons.h"
#include "../globals/AppColors.h"

#define ICONBUTTON_SIZE ImVec2(40.0f, 40.0f)

class IconButton
{
public:
	static bool render(Icon icon, ImVec4 tint = AppColors::positive, ImVec2 size = ICONBUTTON_SIZE);

	//-- CodeSomnia Add--
	static bool renderWithLabel(const char* label, Icon icon, ImVec4 tint = AppColors::positive, ImVec2 size = ICONBUTTON_SIZE);
};




