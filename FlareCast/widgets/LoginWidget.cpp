#include "LoginWidget.h"

LoginWidget::LoginWidget(Hosting& hosting, HostSettingsWidget& hostSettingsWidget)
	: _hosting(hosting), _isLoginLocked(false), _hostSettingsWidget(hostSettingsWidget)
{
}

void LoginWidget::render(bool& showLogin) {

    ImGui::PushStyleColor(ImGuiCol_Text, AppColors::panelTitleBarActiveText);  // Focused text color
    ImGui::PushStyleColor(ImGuiCol_Border, AppColors::panelBorderActive);

    // Set the font
    ImGui::PushFont(AppFonts::title);

    // Get the window size
    size = ImGui::GetIO().DisplaySize;

    // Set window position and size constraints
    ImGui::SetNextWindowPos(ImVec2((size.x/2)-175, (size.y / 2) - 250));
    ImGui::SetNextWindowSizeConstraints(ImVec2(350, 500), ImVec2(350, 500));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.2f));

    // Can't collapse the window
    ImGui::Begin("## Parsec Login", (bool*)0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

    size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetWindowPos();

    // Reset style
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    startBody(true);

        ImGui::Dummy(ImVec2(0, 10));
        ImGui::SetCursorPosX((size.x - 200.0f) * 0.5f);
        ImGui::Image(AppIcons::logo, ImVec2(200, 49));

        ImGui::Dummy(ImVec2(0, 20));

        elParagraph("This app uses the service Parsec for sharing your screen with other players.");

        renderPersonal(350, showLogin);

    endBody();

    startFooter();

        if (!_isLoginLocked) {

            if (elBtn("Log in##Login Button")) {
                attemptLoginPersonal(showLogin);
            }

            ImGui::SameLine();
            ImGui::Indent(68);

            if (elBtnSecondary("Create new account")) {
                ShellExecute(0, 0, L"https://parsec.app/activate/", 0, 0, SW_SHOW);
            }

        } else {
            static ImVec2 cursor;
            cursor = ImGui::GetCursorPos();
            cursor.x += 140;
            cursor.y -= 10;
            ImGui::SetCursorPos(cursor);
            LoadingRingWidget::render();
        }

    endFooter();

    if (_showError) {
        ImGui::OpenPopup("Login Failed");
        _showError = false;
    }

    if (AlertWidget::render("Login Failed", _sessionError.c_str())) {
        _showError = false;
    }

	endWidget();
}

void LoginWidget::renderPersonal(float width, bool& showLogin) {

    if (elText("E-Mail Address", _email)) {}

    if (elPassword("Password", _password)) {}

    if (elText("2FA", _2fa, "The 2 factor authentication code if it is enabled on your account.")) {}

}

void LoginWidget::render3rd(float width)
{

    ImGui::Text("Auth Code");
    AppColors::pushPrimary();
    AppFonts::pushSugoiDekai();
    ImGui::SetNextItemWidth(width);
    ImGui::InputText("##Login auth", _auth.userCode, 16, ImGuiInputTextFlags_ReadOnly);
    AppFonts::pop();
    AppColors::pop();
    TitleTooltipWidget::render(
        "Authentication Code",
        "Copy and Paste this code to the link in your browser window\n\nIf a browser window does not show automatically, go to:\nhttps://parsec.app/activate/"
    );
}

void LoginWidget::renderCreateAccount(float width) {
    ImGui::SetCursorPosX(width - 166.0f);
    if (elBtnSecondary("Create new account")) {
        ShellExecute(0, 0, L"https://parsec.app/activate/", 0, 0, SW_SHOW);
    }
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
}

void LoginWidget::renderLoginTooltip()
{
    TitleTooltipWidget::render(
        "Parsec Account",
        (
            string () +
            "Login with your Parsec account.\n" +
            "* Parsec Soda does not collect any data from you."
        ).c_str()
    );
}

void LoginWidget::attemptLoginPersonal(bool& showLogin)
{
    if (!_isLoginLocked)
    {
        _isLoginLocked = true;
        LoadingRingWidget::render(true);
        _loginThread = thread([&]() {
            _hosting.getSession().fetchSession(_email, _password, _2fa);

            if (!_hosting.getSession().isValid())
            {
                _sessionError = _hosting.getSession().getSessionError();
                _sessionStatus = _hosting.getSession().getSessionStatus();
                _showError = true;
            }
            else
            {
                showLogin = false;
            }

            _hosting.fetchAccountData();
            _hostSettingsWidget.updateSecretLink();
            _isLoginLocked = false;
            _loginThread.detach();
        });
    }
}

void LoginWidget::attemptLogin3rd(bool& showLogin)
{
    if (!_isLoginLocked)
    {
        _sessionCancelled = false;
        _showCancelButton = false;
        _isLoginLocked = true;
        LoadingRingWidget::render(true);
        _loginThread = thread([&]() {
            _auth = _hosting.getSession().authenticate();

            if (_auth.success)
            {
                string uri = string(_auth.verificationUri) + "/" + _auth.userCode;
                wstring wuri(&uri[0], &uri[uri.size()]);
                ShellExecute(0, 0, wuri.c_str(), 0, 0, SW_SHOW);
            }

            // Minimum auth cooldown
            Sleep((_auth.interval+1) * 1000);
            ParsecSession::SessionStatus status;

            _showCancelButton = true;

            bool done = false;
            while (!done && !_sessionCancelled)
            {
                status = _hosting.getSession().pollSession(_auth);
                switch (status)
                {
                case ParsecSession::SessionStatus::PENDING:
                    Sleep(1500);
                    break;
                case ParsecSession::SessionStatus::APPROVED:
                    done = true;
                    showLogin = false;
                    break;
                case ParsecSession::SessionStatus::INVALID:
                default:
                    done = true;
                    break;
                }
            }

            _hosting.fetchAccountData();
            _hostSettingsWidget.updateSecretLink();
            _isLoginLocked = false;
            _loginThread.detach();
        });
    }
}



