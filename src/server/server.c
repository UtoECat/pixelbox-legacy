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
#include "ressrv.h"
#include "sql.h"
#include "info.h"
#include <raylib.h>
#include <stdio.h>
#include <assert.h>

static void CreateWindows() {
	#if PBOX_DEBUG 
	pbDebugWindowToggle();
	#endif
	pbWindow* w = pbExplorerWindowStatic();
	assert(pbWinManAdd(w, w->size));
}

pbDataBase* MainDB;

int pbServer() {
	pbLogSystemInit();
	MainDB = pbDataBaseCreate("server.db", PBOX_SERVER_DATABASE);
	if (!MainDB) return -1;
	pbRenderCreate(0);
	pbWinManCreate();
	CreateWindows();

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		const char* text = "Pixelbox Server!";
		float len = TextLength(text);
		DrawText(text, GetScreenWidth()/2 - len*10/2, 
			GetScreenHeight()/2 - 10, 20, LIGHTGRAY);
		pbWinManRender();
		EndDrawing();
	}
	pbWinManDestroy();
	pbRenderDestroy();
	pbDataBaseDestroy(MainDB); // hehe
	return 0;
}
