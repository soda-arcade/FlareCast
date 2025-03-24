#include "HostInfoWidget.h"

HostInfoWidget::HostInfoWidget(Hosting& hosting)
	: _hosting(hosting)
{
	_loginTime = Clock::now();
}

void HostInfoWidget::renderArcade() {

	static ImVec2 windowSize = ImVec2(250, 64);
	static ImVec2 windowPos = ImVec2(0, 0);
	static ImVec2 padding = ImVec2(8, 6);
	static ImVec2 viewportSize;
	viewportSize = ImGui::GetMainViewport()->GetCenter();
	viewportSize.x *= 2;
	viewportSize.y *= 2;

	windowPos.x = viewportSize.x - windowSize.x - 10;
	windowPos.y = (viewportSize.y - windowSize.y) - 78;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));

	ImGui::Begin(
		"##ArcadeInfoWidget", (bool*)0,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);

	ImGui::SetWindowSize(windowSize);
	ImGui::SetWindowPos(windowPos);

	ImGui::PushFont(AppFonts::label);
	ImGui::BeginGroup();
		ImGui::Image(AppIcons::arcadeIcon, ImVec2(48, 48));
		ImGui::SameLine();

		// Move cursor UP a bit
		static ImVec2 cursor;
		cursor = ImGui::GetCursorPos();
		cursor.y -= 2;
		ImGui::SetCursorPos(cursor);

		ImGui::BeginGroup();
			if (!Arcade::instance.credentials.token.empty() &&
			!Arcade::instance.credentials.username.empty()) {
				elLabel(Arcade::instance.credentials.username);
				AppColors::pushButtonSecondary();
				if (ImGui::Button("Logout")) {
					Arcade::instance.logout();
				}
				AppColors::popButton();
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);
				ImGui::Text("Not logged in");
				ImGui::PopStyleColor();
				AppColors::pushButtonSecondary();
				if (ImGui::Button("Login")) {
					Config::cfg.arcade.showLogin = true;
				}
				AppColors::popButton();
			}	
		ImGui::EndGroup();
	ImGui::EndGroup();
	ImGui::PopFont();

	ImGui::End();

	ImGui::PopStyleVar();

}

void HostInfoWidget::render()
{
	static bool isSessionExpired = false;
	isSessionExpired = (_hosting.getHost().status == Guest::Status::EXPIRED);

	static ImVec2 windowSize = ImVec2(250, 60);
	static ImVec2 windowPos = ImVec2(0, 0);
	static ImVec2 padding = ImVec2(8, 6);
	static ImVec2 viewportSize;
	viewportSize = ImGui::GetMainViewport()->GetCenter();
	viewportSize.x *= 2;
	viewportSize.y *= 2;
	windowPos.x = viewportSize.x - windowSize.x - 10;
	windowPos.y = viewportSize.y - windowSize.y - 10;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));

	ImGui::Begin(
		"##HostInfoWidget", (bool*)0,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);

	ImGui::SetWindowSize(windowSize);
	ImGui::SetWindowPos(windowPos);

	static ParsecSession& session = _hosting.getSession();

	if (session.type == MetadataCache::SessionCache::SessionType::THIRD)
	{
		static uint32_t remainingTime;
		static float fill;

		if (session.getRemainingTime() > 0)
		{
			remainingTime = session.getRemainingTime();
			fill = 1.0f - ((float)(remainingTime) / session.getLifespan());
		}
		else if (session.isValid() && !session.isUpdating())
		{
			_hosting.fetchAccountData();
		}


		static float radius = 20;
		static float thickness = radius * 0.5f;
		
		ImGui::BeginGroup();

		static ImVec2 cursor;
		cursor = ImGui::GetCursorPos();
		cursor.y += 2;
		ImGui::SetCursorPosY(cursor.y);

		ProgressCircularWidget::render(
			radius, thickness,
			fill, 0.0f, 1.0f,
			isSessionExpired ? COLOR_NEGATIVE_U32 : COLOR_POSITIVE_U32
		);
		if (ImGui::IsItemHovered())
		{
			TitleTooltipWidget::render
			(
				"Session Expiry",
				(
					string() +
					"Parsec Soda tries to predict the expiry time.\n\n" +
					"Next check in:\n" +
					" > " + Timespan::fromSeconds(remainingTime).toString() + "\n\n"
					"Click to force early refresh."
				).c_str()
			);
		}

		ImGui::SetCursorPos(cursor);

		if (ImGui::Button("### Session refresh button", ImVec2(2 * radius, 2 * radius)))
		{
			if (!session.isUpdating())
			{
				static Debouncer debouncer = Debouncer(500, [&]() {
					_hosting.fetchAccountData(true);
				});
				debouncer.start();
			}
		}
		ImGui::EndGroup();

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(2, 0));
		ImGui::SameLine();
	}

	ImGui::BeginGroup();
	if (_hosting.getHost().isValid()) {
		ImGui::Image(AppIcons::parsecIcon, ImVec2(48, 48));
		ImGui::SameLine();

		// Move cursor UP a bit
		static ImVec2 cursor;
		cursor = ImGui::GetCursorPos();
		cursor.y -= 4;
		ImGui::SetCursorPos(cursor);

		ImGui::PushFont(AppFonts::label);
		ImGui::BeginGroup();
			ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelText);
			ImGui::Text("%s", _hosting.getHost().name.c_str());
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, AppColors::formHelpText);
			ImGui::Text("#%d", _hosting.getHost().userID);

			AppColors::pushButtonSecondary();
			if (ImGui::Button("Logout")) {
				_hosting.getSession().deleteSessionCache();
			}
			AppColors::popButton();
		ImGui::EndGroup();
		ImGui::PopFont();
	}
	else if (isSessionExpired)
	{
		AppStyle::pushLabel();
		ImGui::TextWrapped("");
		AppStyle::pop();
		AppFonts::pushInput();
		AppColors::pushNegative();
		ImGui::TextWrapped("Session Expired");
		AppColors::pop();
		AppFonts::pop();
		TitleTooltipWidget::render(
			"Parsec Session Expired",
			(
				string() +
				"Parsec Soda recommends you get a new session:" +
				"  1. Stop streaming.\n" +
				"  2. Click the Log off button.\n" +
				"  3. Authenticate again.\n\n" +
				"Until a new session is acquired:\n" +
				" - Your room will not be listed in public Arcade.\n" +
				" - Guests cannot join your room even through the link.\n" +
				" - Only guests already in this room will see your stream."
			).c_str()
		);
	}
	else
	{
		AppStyle::pushLabel();
		ImGui::TextWrapped("(# ???)\t");
		AppStyle::pop();
		AppFonts::pushInput();
		ImGui::TextWrapped("Unknown Host");
		AppFonts::pop();
	}
	ImGui::EndGroup();

	ImGui::End();

	ImGui::PopStyleVar();

	renderArcade();
}