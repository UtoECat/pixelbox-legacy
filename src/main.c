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
#include <stdio.h>
#include <assert.h>

int pbMain() {
	pbRenderCreate();
	pbWinManCreate();
	pbWindow* w = pbDebugWindowCreate();
	w->x = 300;
	w->y = 200;
	assert(pbWinManAdd(w, w->size)==0);
	pbWindowFree(w);
	w = pbBlankWindowCreate();
	assert(pbWinManAdd(w, w->size)==0);
	pbWindowFree(w);
	w = pbSuspiciousWindowCreate();
	w->x = 400;
	assert(pbWinManAdd(w, w->size)==0);
	pbWindowFree(w);
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
	return 0;
}
