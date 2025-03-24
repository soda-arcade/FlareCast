﻿#include "LibraryWidget.h"
#include "../ImGui/imform.h"

LibraryWidget::LibraryWidget(Hosting& hosting)
    : _hosting(hosting)
{
}

bool LibraryWidget::render(bool& showWindow) {

    // Widget top
    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), ImVec2(800, 900));
    ImGui::Begin("Library", &showWindow);
    if (!showWindow) Config::cfg.widgets.library = showWindow;
    AppStyle::pop();

    AppStyle::pushInput();
    ImVec2 size = ImGui::GetContentRegionAvail();

    if (!showEditForm) {
		renderGameList();
	}
	else {
		renderForm(popupEditIndex);
    }

    AppStyle::pop();

    ImGui::End();
    return true;

}

bool LibraryWidget::renderGameList() {
    
    static bool showRemovePopup = false;
    static bool showEditPopup = false;
    static string popupTitle = "";
    static string name;
    static string path;
    static string parameters;
    static string thumbnailPath;
    static string gameID;
    static vector<GameData>& _games = Cache::cache.gameList.getGames();
    static string filterTextStr;
    static bool filterSuccess = false;
	string defaultID = "1wdoHfhhZH5lPuZCwGBete0HIAj";

    AppStyle::pushInput();
    ImVec2 size = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("Library List", ImVec2(size.x, size.y));

    //AppColors::pushInput();
    AppStyle::pushLabel();
    ImGui::Text("FILTER GAMES");
    AppStyle::pop();
    ImGui::SetNextItemWidth(size.x - 100);
    ImGui::InputText("### Game List Filter", _filterText, 256);
    TitleTooltipWidget::render("Filter Games", "Type game Name or ID to filter.");

    ImGui::SameLine();

    ImGui::BeginGroup();
        ImGui::Indent(10);
        AppColors::pushButtonSolid();
        if (ImGui::Button("Add Game")) {
			popupEditIndex = -1;
            memset(_editName, 0, 256);
            memset(_editPath, 0, 256);
            memset(_editParam, 0, 256);
            showEditForm = true;
        }
        ImGui::PopStyleColor(4);
        //AppColors::pushButton();
	    ImGui::Unindent(10);
	ImGui::EndGroup();



    ImGui::Dummy(ImVec2(0, 10));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 10));

    ImGui::BeginChild("gameslist");

        for (size_t i = 0; i < _games.size(); ++i){

            name = _games[i].name;
            path = _games[i].path;
            parameters = _games[i].parameters;
        
            // Remove game
            IconButton::render(AppIcons::trash, AppColors::primary, ImVec2(30, 30));
            if (ImGui::IsItemActive()) {
                popupTitle = string("Remove Game##Popup");
                showRemovePopup = true;
                popupRemoveIndex = i;
                ImGui::OpenPopup(popupTitle.c_str());
            }

			// Edit game
			ImGui::SameLine();
			IconButton::render(AppIcons::edit, AppColors::primary, ImVec2(30, 30));
			if (ImGui::IsItemActive()) {
                strncpy_s(_editName, _games[i].name.c_str(), 256);
                strncpy_s(_editPath, _games[i].path.c_str(), 256);
                strncpy_s(_editParam, _games[i].parameters.c_str(), 256);

				popupEditIndex = i;
                showEditForm = true;
			}

            // Remove game popup
            if (i == popupRemoveIndex) {

                if (ConfirmPopupWidget::render(
                    popupTitle.c_str(),
                    showRemovePopup,
                    ("Remove\n" + name).c_str()
                )) {

                    // Remove from game list
                    Cache::cache.gameList.remove(name, [&](GameData& guest) {
		
	                });
                    popupRemoveIndex = -1;
                    showRemovePopup = false;

                }
                
            }

            // Game title and path
            ImGui::SameLine();
            ImGui::BeginGroup();
			    ImGui::Indent(10);
                AppStyle::pushInput();
                ImGui::Text("%s", name.c_str());
                AppStyle::pop();
				AppStyle::pushLabel();
				ImGui::Text("%s", path.c_str());
                AppStyle::pop();
				ImGui::Unindent(10);
            ImGui::EndGroup();

            ImGui::Dummy(ImVec2(0, 5));
            
        }

    ImGui::EndChild();
    ImGui::EndChild(); // is it supposed to be here. i had to guess

    // Widget bottom
    AppStyle::pop();
    //ImGui::End();

    return true;

}

bool LibraryWidget::renderForm(int index) {

	ImForm::InputText("Game Name", _editName, "Name of the game you want to add.");
    
	ImForm::InputText("Game Path", _editPath, "Path to the game executable.");

    AppStyle::pushButton();
    AppColors::pushButtonSolid();
    if (ImGui::Button("Browse...")) {
        std::string filePath = openFileDialog();
        if (!filePath.empty()) {
            
            // Copy to _editPath
            strncpy_s(_editPath, filePath.c_str(), 256);

        }
    }
    ImGui::PopStyleColor(4);
    AppStyle::popButton();

    ImGui::Dummy(ImVec2(0, 10));

	ImForm::InputText("Launch Parameters", _editParam, "Parameters to pass to the game executable when launching in kiosk mode.");
    
	// Save button
    AppStyle::pushButton();
    AppStyle::pushPositive();
	if (ImGui::Button("Save")) {

		// New game
        if (index < 0) {
            
			GameData game = GameData();
            game.itemID = Cache::cache.gameList.getGames().size() + 1;
			game.name = _editName;
			game.path = _editPath;
			game.parameters = _editParam;

			// Add to game list
            Cache::cache.gameList.getGames().push_back(game);
            Cache::cache.gameList.SaveToFile();

		}
        
		// Edit game
		else {

			// Update game
			Cache::cache.gameList.getGames()[index].name = _editName;
			Cache::cache.gameList.getGames()[index].path = _editPath;
			Cache::cache.gameList.getGames()[index].parameters = _editParam;

            Cache::cache.gameList.SaveToFile();
            
        }

		// Close form
		showEditForm = false;
        
	}
    AppStyle::pop();
    AppStyle::popButton();

	ImGui::SameLine();

    // Cancel
    AppStyle::pushButton();
	AppStyle::pushNegative();
	if (ImGui::Button("Cancel")) {
		showEditForm = false;
	}
    AppStyle::pop();
    AppStyle::popButton();

    return true;
    
}

std::string LibraryWidget::openFileDialog() {
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = TEXT("Executable Files\0*.exe\0All Files\0*.*\0");
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        // Convert TCHAR* to std::string
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, NULL, 0, NULL, NULL);
        std::string filePath(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, &filePath[0], size_needed, NULL, NULL);
        return filePath;
    }
    else {
        // User canceled the dialog or an error occurred
        return std::string();
    }
}


