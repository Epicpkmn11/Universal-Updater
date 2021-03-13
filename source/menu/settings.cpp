/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "common.hpp"
#include "init.hpp"
#include "overlay.hpp"
#include "scriptUtils.hpp"
#include "storeUtils.hpp"
#include <unistd.h>

extern bool exiting, QueueRuns;
extern bool touching(touchPosition touch, Structs::ButtonPos button);
static const std::vector<Structs::ButtonPos> mainButtons = {
	{ 45, 32, 271, 22 },
	{ 45, 62, 271, 22 },
	{ 45, 92, 271, 22 },
	{ 45, 122, 271, 22 },
	{ 45, 152, 271, 22 },
	{ 45, 182, 271, 22 },
	{ 45, 212, 271, 22 }
};

static const std::vector<Structs::ButtonPos> langButtons = {
	{ 45, 32, 271, 22 },
	{ 45, 62, 271, 22 },
	{ 45, 92, 271, 22 },
	{ 45, 122, 271, 22 },
	{ 45, 152, 271, 22 },
	{ 45, 182, 271, 22 },

	{ 45, 220, 16, 16 } // Add Font.
};

static const std::vector<Structs::ButtonPos> toggleAbles = {
	{ 288, 44, 24, 24 },
	{ 288, 120, 24, 24 }
};

static const Structs::ButtonPos back = { 45, 0, 24, 24 }; // Back arrow for directory.
static const Structs::ButtonPos Themes = { 40, 220, 280, 24 }; // Themes.


static const std::vector<std::string> mainStrings = { "LANGUAGE", "SELECT_UNISTORE", "AUTO_UPDATE_SETTINGS_BTN", "GUI_SETTINGS_BTN", "DIRECTORY_SETTINGS_BTN", "CREDITS", "EXIT_APP" };
static const std::vector<std::string> dirStrings = { "CHANGE_3DSX_PATH", "CHANGE_NDS_PATH", "CHANGE_ARCHIVE_PATH", "CHANGE_SHORTCUT_PATH", "CHANGE_FIRM_PATH" };

/* Note: Украïнська is spelled using a latin i with dieresis to work in the system font */
//static const std::vector<std::string> languages = { "Bruh", "Dansk", "Deutsch", "English", "Español", "Français", "Italiano", "Lietuvių", "Magyar", "Polski", "Português", "Português (Brasil)", "Русский", "Украïнська", "日本語" };
//static const std::string langsTemp[] = { "br", "da", "de", "en", "es", "fr", "it", "lt", "hu", "pl", "pt", "pt-BR", "ru", "uk", "jp"};
static const std::vector<std::string> languages = { "Bruh", "Deutsch", "English", "Español", "Français", "Italiano", "Magyar", "Polski", "Português (Brasil)", "Русский", "Украïнська", "日本語" };
static const std::string langsTemp[] = { "br", "de", "en", "es", "fr", "it", "hu", "pl", "pt-BR", "ru", "uk", "jp"};

static const std::vector<std::string> ThemeNames = { "THEME_DEFAULT" };

/*
	Main Settings.

	int selection: The Settings Selection.
*/
static void DrawSettingsMain(int selection) {
	Gui::Draw_Rect(40, 0, 280, 25, GFX::Themes[GFX::SelectedTheme].EntryBar);
	Gui::Draw_Rect(40, 25, 280, 1, GFX::Themes[GFX::SelectedTheme].EntryOutline);
	Gui::DrawStringCentered(20, 2, 0.6, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("SETTINGS"), 280, 0, font);

	for (int i = 0; i < 7; i++) {
		if (i == selection) Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, GFX::Themes[GFX::SelectedTheme].MarkSelected);
		Gui::DrawStringCentered(20, mainButtons[i].y + 4, 0.45f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get(mainStrings[i]), 255, 0, font);
	}
}

/*
	Draw the Language Selection.

	int selection: The Language Selection.
	int sPos: The Screen Position.
*/
static void DrawLanguageSettings(int selection, int sPos) {
	Gui::Draw_Rect(40, 0, 280, 25, GFX::Themes[GFX::SelectedTheme].EntryBar);
	Gui::Draw_Rect(40, 25, 280, 1, GFX::Themes[GFX::SelectedTheme].EntryOutline);
	GFX::DrawSprite(sprites_arrow_idx, back.x, back.y);
	GFX::DrawSprite(sprites_add_font_idx, langButtons[6].x, langButtons[6].y);
	Gui::DrawStringCentered(20, 2, 0.6, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("SELECT_LANG"), 248, 0, font);

	for(int i = 0; i < 6 && i < (int)languages.size(); i++) {
		if (sPos + i == selection) Gui::Draw_Rect(langButtons[i].x, langButtons[i].y, langButtons[i].w, langButtons[i].h, GFX::Themes[GFX::SelectedTheme].MarkSelected);
		Gui::DrawStringCentered(20, langButtons[i].y + 4, 0.45f, GFX::Themes[GFX::SelectedTheme].TextColor, languages[sPos + i], 280, 0, font);
	}
}

/*
	Directory Change Draw.

	int selection: The Settings Selection.
*/
static void DrawSettingsDir(int selection) {
	Gui::Draw_Rect(40, 0, 280, 25, GFX::Themes[GFX::SelectedTheme].EntryBar);
	Gui::Draw_Rect(40, 25, 280, 1, GFX::Themes[GFX::SelectedTheme].EntryOutline);
	GFX::DrawSprite(sprites_arrow_idx, back.x, back.y);
	Gui::DrawStringCentered(20, 2, 0.6, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("DIRECTORY_SETTINGS"), 248, 0, font);

	for (int i = 0; i < 5; i++) {
		if (i == selection) Gui::Draw_Rect(mainButtons[i].x, mainButtons[i].y, mainButtons[i].w, mainButtons[i].h, GFX::Themes[GFX::SelectedTheme].MarkSelected);
		Gui::DrawStringCentered(20, mainButtons[i].y + 4, 0.45f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get(dirStrings[i]), 255, 0, font);
	}
}

/*
	Draw Auto-Update Settings page.
*/
static void DrawAutoUpdate(int selection) {
	Gui::Draw_Rect(40, 0, 280, 25, GFX::Themes[GFX::SelectedTheme].EntryBar);
	Gui::Draw_Rect(40, 25, 280, 1, GFX::Themes[GFX::SelectedTheme].EntryOutline);
	GFX::DrawSprite(sprites_arrow_idx, back.x, back.y);

	Gui::DrawStringCentered(20, 2, 0.6, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("AUTO_UPDATE_SETTINGS"), 240, 0, font);

	/* Toggle Boxes. */
	Gui::Draw_Rect(40, 44, 280, 24, (selection == 0 ? GFX::Themes[GFX::SelectedTheme].MarkSelected : GFX::Themes[GFX::SelectedTheme].MarkUnselected));
	Gui::DrawString(47, 48, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("AUTO_UPDATE_UNISTORE"), 210, 0, font);
	GFX::DrawToggle(toggleAbles[0].x, toggleAbles[0].y, config->autoupdate());
	Gui::DrawString(47, 75, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("AUTO_UPDATE_UNISTORE_DESC"), 265, 0, font, C2D_WordWrap);

	Gui::Draw_Rect(40, 120, 280, 24, (selection == 1 ? GFX::Themes[GFX::SelectedTheme].MarkSelected : GFX::Themes[GFX::SelectedTheme].MarkUnselected));
	Gui::DrawString(47, 124, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("AUTO_UPDATE_UU"), 210, 0, font);
	GFX::DrawToggle(toggleAbles[1].x, toggleAbles[1].y, config->updatecheck());
	Gui::DrawString(47, 151, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("AUTO_UPDATE_UU_DESC"), 265, 0, font, C2D_WordWrap);
}

/*
	Draw the GUI Settings.

	int selection: The Settings Selection.
*/
static void DrawGUISettings(int selection) {
	Gui::Draw_Rect(40, 0, 280, 25, GFX::Themes[GFX::SelectedTheme].EntryBar);
	Gui::Draw_Rect(40, 25, 280, 1, GFX::Themes[GFX::SelectedTheme].EntryOutline);
	GFX::DrawSprite(sprites_arrow_idx, back.x, back.y);

	Gui::DrawStringCentered(20, 2, 0.6, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("GUI_SETTINGS"), 248, 0, font);

	Gui::Draw_Rect(40, 44, 280, 24, (selection == 0 ? GFX::Themes[GFX::SelectedTheme].MarkSelected : GFX::Themes[GFX::SelectedTheme].MarkUnselected));
	Gui::DrawString(47, 48, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("UNISTORE_BG"), 210, 0, font);
	GFX::DrawToggle(toggleAbles[0].x, toggleAbles[0].y, config->usebg());
	Gui::DrawString(47, 75, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("UNISTORE_BG_DESC"), 265, 0, font, C2D_WordWrap);

	Gui::Draw_Rect(40, 120, 280, 24, (selection == 1 ? GFX::Themes[GFX::SelectedTheme].MarkSelected : GFX::Themes[GFX::SelectedTheme].MarkUnselected));
	Gui::DrawString(47, 124, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("CUSTOM_FONT"), 210, 0, font);
	GFX::DrawToggle(toggleAbles[1].x, toggleAbles[1].y, config->customfont());
	Gui::DrawString(47, 151, 0.4f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("CUSTOM_FONT_DESC"), 265, 0, font, C2D_WordWrap);

	Gui::Draw_Rect(40, 196, 280, 24, (selection == 2 ? GFX::Themes[GFX::SelectedTheme].MarkSelected : GFX::Themes[GFX::SelectedTheme].MarkUnselected));
	Gui::DrawString(47, 200, 0.5f, GFX::Themes[GFX::SelectedTheme].TextColor, Lang::get("ACTIVE_THEME") + ": " + Lang::get(ThemeNames[GFX::SelectedTheme]), 210, 0, font);
}


/*
	Settings Main Handle.
	Here you can..

	- Change the Language.
	- Access the UniStore Manage Handle.
	- Enable UniStore auto update on boot.
	- Show the Credits.
	- Exit Universal-Updater.

	int &page: Reference to the page.
	bool &dspSettings: Reference to the display Settings.
	int &storeMode: Reference to the store Mode.
	int &selection: Reference to the Selection.
*/
static void SettingsHandleMain(int &page, bool &dspSettings, int &storeMode, int &selection) {
	if (hDown & KEY_B) {
		selection = 0;
		storeMode = 0;
	}

	if (hRepeat & KEY_DOWN) {
		if (selection < 6) selection++;
		else selection = 0;
	}

	if (hRepeat & KEY_UP) {
		if (selection > 0) selection--;
		else selection = mainStrings.size() - 1;
	}

	if (hRepeat & KEY_RIGHT) {
		if (selection + 8 < (int)mainStrings.size()) selection += 8;
		else selection = mainStrings.size() - 1;
	}

	if (hRepeat & KEY_LEFT) {
		if (selection - 8 > 0) selection -= 8;
		else selection = 0;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, mainButtons[0])) {
			selection = 0;
			page = 4;

		} else if (touching(touch, mainButtons[1])) {
			if (QueueRuns) {
				if (Msg::promptMsg(Lang::get("FEATURE_SIDE_EFFECTS"))) Overlays::SelectStore();

			} else {
				Overlays::SelectStore();
			}

		} else if (touching(touch, mainButtons[2])) {
			selection = 0;
			page = 2;

		} else if (touching(touch, mainButtons[3])) {
			selection = 0;
			page = 3;

		} else if (touching(touch, mainButtons[4])) {
			selection = 0;
			page = 1;

 		} else if (touching(touch, mainButtons[5])) {
			Overlays::ShowCredits();

		} else if (touching(touch, mainButtons[6])) {
			if (QueueRuns) exiting = Msg::promptMsg(Lang::get("FEATURE_SIDE_EFFECTS"));
			else exiting = true;
		}
	}

	if (hDown & KEY_A) {
		switch(selection) {
			case 0:
				selection = 0;
				page = 4;
				break;

			case 1:
				if (QueueRuns) {
					if (Msg::promptMsg(Lang::get("FEATURE_SIDE_EFFECTS"))) Overlays::SelectStore();

				} else {
					Overlays::SelectStore();
				}
				break;

			case 2:
				selection = 0;
				page = 2;
				break;

			case 3:
				selection = 0;
				page = 3;
				break;

			case 4:
				selection = 0;
				page = 1;
				break;

			case 5:
				Overlays::ShowCredits();
				break;

			case 6:
				if (QueueRuns) exiting = Msg::promptMsg(Lang::get("FEATURE_SIDE_EFFECTS"));
				else exiting = true;
				break;
		}
	}
}

/*
	Directory Handle.
	Here you can..

	- Change the Directory of...
		- 3DSX, NDS & Archives.

	int &page: Reference to the page.
	int &selection: Reference to the Selection.
*/
static void SettingsHandleDir(int &page, int &selection) {
	if (hDown & KEY_B) {
		page = 0;
		selection = 4;
	}

	if (hRepeat & KEY_DOWN) {
		if (selection < 4) selection++;
		else selection = 0;
	}

	if (hRepeat & KEY_UP) {
		if (selection > 0) selection--;
		else selection = dirStrings.size() - 1;
	}

	if (hRepeat & KEY_RIGHT) {
		if (selection + 8 < (int)dirStrings.size() - 1) selection += 8;
		else selection = dirStrings.size() - 1;
	}

	if (hRepeat & KEY_LEFT) {
		if (selection - 8 > 0) selection -= 8;
		else selection = 0;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, back)) {
			page = 0;
			selection = 4;

		} else if (touching(touch, mainButtons[0])) {
			const std::string path = Overlays::SelectDir(config->_3dsxPath(), Lang::get("SELECT_DIR"));
			if (path != "") config->_3dsxPath(path);

		} else if (touching(touch, mainButtons[1])) {
			const std::string path = Overlays::SelectDir(config->ndsPath(), Lang::get("SELECT_DIR"));
			if (path != "") config->ndsPath(path);

		} else if (touching(touch, mainButtons[2])) {
			const std::string path = Overlays::SelectDir(config->archPath(), Lang::get("SELECT_DIR"));
			if (path != "") config->archPath(path);

		} else if (touching(touch, mainButtons[3])) {
			const std::string path = Overlays::SelectDir(config->shortcut(), Lang::get("SELECT_DIR"));
			if (path != "") config->shortcut(path);

		} else if (touching(touch, mainButtons[4])) {
			const std::string path = Overlays::SelectDir(config->firmPath(), Lang::get("SELECT_DIR"));
			if (path != "") config->firmPath(path);
		}
	}

	if (hDown & KEY_A) {
		std::string path = "";

		switch(selection) {
			case 0:
				path = Overlays::SelectDir(config->_3dsxPath(), Lang::get("SELECT_DIR"));
				if (path != "") config->_3dsxPath(path);
				break;

			case 1:
				path = Overlays::SelectDir(config->ndsPath(), Lang::get("SELECT_DIR"));
				if (path != "") config->ndsPath(path);
				break;

			case 2:
				path = Overlays::SelectDir(config->archPath(), Lang::get("SELECT_DIR"));
				if (path != "") config->archPath(path);
				break;

			case 3:
				path = Overlays::SelectDir(config->shortcut(), Lang::get("SELECT_DIR"));
				if (path != "") config->shortcut(path);
				break;

			case 4:
				path = Overlays::SelectDir(config->firmPath(), Lang::get("SELECT_DIR"));
				if (path != "") config->firmPath(path);
				break;
		}
	}
}

/*
	Logic of the Auto-Update Settings.

	Here you can..

	- Enable / Disable Automatically updating the UniStore on boot.
	- Enable / Disable Automatically check for Universal-Updater updates on boot.

	int &page: Reference to the page.
	int &selection: Reference to the Selection.
*/
static void AutoUpdateLogic(int &page, int &selection) {
	if (hDown & KEY_B) {
		page = 0;
		selection = 2;
	}

	if (hRepeat & KEY_DOWN) {
		if (selection < 1) selection++;
	}

	if (hRepeat & KEY_UP) {
		if (selection > 0) selection--;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, back)) {
			page = 0;
			selection = 2;

		} else if (touching(touch, toggleAbles[0])) {
			config->autoupdate(!config->autoupdate());

		} else if (touching(touch, toggleAbles[1])) {
			config->updatecheck(!config->updatecheck());
		}
	}

	if (hDown & KEY_A) {
		switch(selection) {
			case 0:
				config->autoupdate(!config->autoupdate());
				break;

			case 1:
				config->updatecheck(!config->updatecheck());
				break;
		}
	}
}

/*
	Logic of the GUI Settings.

	Here you can..

	- Enable / Disable using the SpriteSheet Background Image, if exist.

	int &page: Reference to the page.
	int &selection: Reference to the Selection.
*/
static void GUISettingsLogic(int &page, int &selection) {
	if (hDown & KEY_B) {
		page = 0;
		selection = 3;
	}

	if (hRepeat & KEY_DOWN) {
		if (selection < 2) selection++;
	}

	if (hRepeat & KEY_UP) {
		if (selection > 0) selection--;
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, back)) {
			page = 0;
			selection = 3;

		} else if (touching(touch, toggleAbles[0])) {
			config->usebg(!config->usebg());

		} else if (touching(touch, toggleAbles[1])) {
			config->customfont(!config->customfont());

			(config->customfont() ? Init::LoadFont() : Init::UnloadFont());

		} else if (touching(touch, Themes)) {
			if (GFX::SelectedTheme < (_THEME_AMOUNT - 1)) GFX::SelectedTheme++;
			else GFX::SelectedTheme = 0;

			config->theme(GFX::SelectedTheme);
		}
	}

	if (hDown & KEY_A) {
		switch(selection) {
			case 0:
				config->usebg(!config->usebg());
				break;

			case 1:
				config->customfont(!config->customfont());

				(config->customfont() ? Init::LoadFont() : Init::UnloadFont());
				break;

			case 2:
				if (GFX::SelectedTheme < (_THEME_AMOUNT - 1)) GFX::SelectedTheme++;
				else GFX::SelectedTheme = 0;

				config->theme(GFX::SelectedTheme);
				break;
		}
	}
}

/*
	Logic of the Language Settings.

	Here you can..

	- Select the language, which should be used with the app.

	int &page: Reference to the page.
	int &selection: Reference to the Selection.
	int &sPos: Reference to the ScreenPos variable.
*/
static void LanguageLogic(int &page, int &selection, int &sPos) {
	if (hRepeat & KEY_DOWN) {
		if (selection < (int)languages.size() - 1) selection++;
		else selection = 0;
	}

	if (hRepeat & KEY_UP) {
		if (selection > 0) selection--;
		else selection = languages.size() - 1;
	}

	if (hRepeat & KEY_RIGHT) {
		if (selection + 6 < (int)languages.size() - 1) selection += 6;
		else selection = languages.size() - 1;
	}

	if (hRepeat & KEY_LEFT) {
		if (selection - 6 > 0) selection -= 6;
		else selection = 0;
	}

	if ((hDown & KEY_B) || (hDown & KEY_TOUCH && touching(touch, back))) {
		selection = 0;
		sPos = 0;
		page = 0;
	}

	if (hDown & KEY_A) {
		const std::string l = langsTemp[selection];

		/* Check if is "uk". */
		if (l == "uk") {
			if (access("sdmc:/3ds/Universal-Updater/font.bcfnt", F_OK) != 0) {
				ScriptUtils::downloadFile("https://github.com/Universal-Team/extras/raw/master/files/universal-updater.bcfnt", "sdmc:/3ds/Universal-Updater/font.bcfnt", Lang::get("DOWNLOADING_COMPATIBLE_FONT"));
			}

			config->customfont(true);
			Init::LoadFont();
		}

		config->language(l);
		Lang::load(config->language());
		selection = 0;
		sPos = 0;
		page = 0;
	}

	if (hDown & KEY_TOUCH) {
		for (int i = 0; i < 6; i++) {
			if (touching(touch, mainButtons[i])) {
				if (i + sPos < (int)languages.size()) {
					const std::string l = langsTemp[i + sPos];

					/* Check if is "uk". */
					if (l == "uk") {
						if (access("sdmc:/3ds/Universal-Updater/font.bcfnt", F_OK) != 0) {
							ScriptUtils::downloadFile("https://github.com/Universal-Team/extras/raw/master/files/universal-updater.bcfnt", "sdmc:/3ds/Universal-Updater/font.bcfnt", Lang::get("DOWNLOADING_COMPATIBLE_FONT"));
						}

						config->customfont(true);
						Init::LoadFont();
					}

					config->language(l);
					Lang::load(config->language());
					selection = 0;
					sPos = 0;
					page = 0;
				}
			}
		}
	}

	if (hDown & KEY_TOUCH) {
		if (touching(touch, langButtons[6])) {
			/* Download Font. */
			ScriptUtils::downloadFile("https://github.com/Universal-Team/extras/raw/master/files/universal-updater.bcfnt", "sdmc:/3ds/Universal-Updater/font.bcfnt", Lang::get("DOWNLOADING_COMPATIBLE_FONT"), true);
			config->customfont(true);
			Init::LoadFont();
		}
	}

	if (selection < sPos) sPos = selection;
	else if (selection > sPos + 6 - 1) sPos = selection - 6 + 1;
}

/*
	Draw the Settings.

	int page: The page.
	int selection: The selection.
*/
void StoreUtils::DrawSettings(int page, int selection, int sPos) {
	switch(page) {
		case 0:
			DrawSettingsMain(selection);
			break;

		case 1:
			DrawSettingsDir(selection);
			break;

		case 2:
			DrawAutoUpdate(selection);
			break;

		case 3:
			DrawGUISettings(selection);
			break;

		case 4:
			DrawLanguageSettings(selection, sPos);
			break;
	}
}

/*
	Settings Handle.

	int &page: Reference to the page.
	bool &dspSettings: Reference to the display Settings.
	int &storeMode: Reference to the store Mode.
	int &selection: Reference to the Selection.
	int &sPos: Reference to screen position.
*/
void StoreUtils::SettingsHandle(int &page, bool &dspSettings, int &storeMode, int &selection, int &sPos) {
	switch(page) {
		case 0:
			SettingsHandleMain(page, dspSettings, storeMode, selection);
			break;

		case 1:
			SettingsHandleDir(page, selection);
			break;

		case 2:
			AutoUpdateLogic(page, selection);
			break;

		case 3:
			GUISettingsLogic(page, selection);
			break;

		case 4:
			LanguageLogic(page, selection, sPos);
			break;
	}
}