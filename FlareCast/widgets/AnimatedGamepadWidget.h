﻿#pragma once

#include <cmath>
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../AGamepad.h"
#include "../globals/AppColors.h"

#define IMVEC2_UP ImVec2(0.0f, 1.0f)
#define IMVEC2_DOWN ImVec2(0.0f, -1.0f)
#define IMVEC2_RIGHT ImVec2(1.0f, 0.0f)
#define IMVEC2_LEFT ImVec2(-1.0f, 0.0f)
#define ANIMGAMEPAD_COL_BG COLOR_WIDGET_BG_U32
#define ANIMGAMEPAD_COL_A  ImGui::GetColorU32(IM_COL32(89, 255, 89, 255))
#define ANIMGAMEPAD_COL_B  ImGui::GetColorU32(IM_COL32(255, 102, 102, 255))
#define ANIMGAMEPAD_COL_X  ImGui::GetColorU32(IM_COL32(115, 157, 255, 255))
#define ANIMGAMEPAD_COL_Y  ImGui::GetColorU32(IM_COL32(255, 255, 115, 255))
#define ANIMGAMEPAD_ACTIVE_BG  ImGui::GetColorU32(IM_COL32(40, 40, 40, 255))
#define ANIMGAMEPAD_INACTIVE_BG  ImGui::GetColorU32(IM_COL32(20, 20, 20, 75))

class AnimatedGamepadWidget
{
public:
	AnimatedGamepadWidget(bool fading = false) {
		useFading = fading;
	}
	void render(XINPUT_GAMEPAD gamepad, float height = 40.0f, ImU32 activeColor = COLOR_PRIMARY_U32);
	void renderAnalog(ImVec2 stick, bool isThumbPress, float radius = 20.0f, bool deadzoned = false, ImU32 activeColor = COLOR_PRIMARY_U32, unsigned int pressId = 0);
	void renderDpad(WORD wButtons, float edge = 40.0f, ImU32 activeColor = COLOR_PRIMARY_U32);
	void renderFaceButtons(WORD wButtons, float height = 40.0f);
	void renderTrigger(XINPUT_GAMEPAD gamepad, bool isRightTrigger, float height = 40.0f, ImU32 activeColor = COLOR_PRIMARY_U32, unsigned int pressId = 0);
	time_t lastPress[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	time_t currentTime = 0;
	void updatePressed(unsigned int pressId);
	bool isActive(unsigned int pressId);

private:
	void renderHSpace(float distance = 5.0f);
	void renderSquare(ImDrawList* drawlist, ImVec2 center, float edge, ImU32 color);
	ImVec2 stickShortToFloat(SHORT lx, SHORT ly, float& distance);
	bool useFading = false;

	void drawTrigger();

	inline ImVec2 sum(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	inline ImVec2 mul(const ImVec2& lhs, const float lambda)
	{
		return ImVec2(lhs.x * lambda, lhs.y * lambda);
	}
	inline ImVec2 lerp(const ImVec2& v1, const ImVec2& v2, const float t)
	{
		return sum(mul(v1, 1.0f - t), mul(v2, t));
	}
	inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a)
	{
		return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a);
	}

	void RenderImageRotated(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle);
};




