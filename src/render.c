/*
** This file is a part of PixelBox - infinite sandbox game
** Abstracted Rendering operations/Context initialization
** Copyright (C) 2021-2023 UtoECat
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "render.h"
#include "config.h"
#include "sql.h"
#include "info.h"

extern int current_theme;

int  pbRenderCreate(int isClient) {
	int flags = FLAG_VSYNC_HINT 		| FLAG_WINDOW_RESIZABLE;
	if (isClient)
		flags |= FLAG_WINDOW_ALWAYS_RUN;

	// during config
	flags |= FLAG_WINDOW_HIDDEN;
	SetConfigFlags(flags);

	const char* name = TextFormat("pixelbox %s v%i.%i - %s", isClient ? "" : "server", PBOX_VERSION_MAJOR, PBOX_VERSION_MINOR, "nomotd");

	// init hidden window
	InitWindow(640, 480, name);
	SetWindowMinSize(200, 150);
	if (!IsWindowReady()) return -1;
	SetExitKey(KEY_NULL);
	if (!isClient) EnableEventWaiting();

	current_theme = dark ? 0 : 1;
	GuiLoadTheme();
	ClearWindowState(FLAG_WINDOW_HIDDEN);

	BeginDrawing();
	EndDrawing();
	return 0;
}

void pbRenderSetFullscreen(int v) {
	if (IsWindowFullscreen() != (!!v)) ToggleFullscreen();
}

void pbRenderDestroy(void) {
	pbSetDataBaseLongProperty(MainDB, "raylib_window_width", GetScreenWidth());
	pbSetDataBaseLongProperty(MainDB, "raylib_window_height", GetScreenHeight());
	Vector2 pos = GetWindowPosition();
	pbSetDataBaseLongProperty(MainDB, "raylib_window_posx", pos.x);
	pbSetDataBaseLongProperty(MainDB, "raylib_window_posy", pos.y);
	pbSetDataBaseLongProperty(MainDB, "raygui_theme", !current_theme);
	pbLog(LOG_INFO, "Window pos[ %i %i ] and size [ %i %i ] saved!",
		pos.x, pos.y,	GetScreenWidth(), GetScreenHeight(), -1, -1);
	CloseWindow();
}
