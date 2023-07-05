/*
** This file is a part of PixelBox - infinite sandbox game
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

/*
** This file is a part of PixelBox - infinite sandbox game
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

#include "config.h"
#include "window.h"
#include "render.h"
#include "rescli.h"
#include "info.h"
#include "sql.h"
#include <stdio.h>
#include <assert.h>

pbDataBase* MainDB;

static void CreateWindows() {
	pbWindow* w = pbSuspiciousWindowCreate();
	w->x = 0;
	w->y = 0;
	assert(pbWinManAdd(w, w->size));
	pbWindowFree(w);
	// ...
	#if PBOX_DEBUG 
	pbDebugWindowToggle();
	#endif
	w = pbExplorerWindowStatic();
	w->x = GetScreenWidth()/2 - w->w/2;
	w->y = GetScreenHeight()/2 - w->h/2;
	assert(pbWinManAdd(w, w->size));
}

int pbClient() {
	pbLogSystemInit();
	MainDB = pbDataBaseCreate("client.db", PBOX_CLIENT_DATABASE);
	if (!MainDB) {
		MainDB = pbDataBaseCreate(":memory:", PBOX_CLIENT_DATABASE);
	}
	if (!MainDB) return -1;
	pbRenderCreate(1);
	pbWinManCreate();
	CreateWindows();

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		const char* text = "Pixelbox Bootloader!";
		float len = TextLength(text);
		DrawText(text, GetScreenWidth()/2 - len*10/2, 
			GetScreenHeight()/2 - 10, 20, LIGHTGRAY);
		pbWinManRender();
		EndDrawing();
	}
	pbWinManDestroy();
	pbRenderDestroy();
	pbDataBaseDestroy(MainDB);
	return 0;
}
