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

// debug window implementation

static pbWindow* once = PBOX_CAST(pbWindow*, PBOX_NULL);

static int debug_create(pbWindow* W) {
	if (once) return -1;
	once = W;
	return 0;	
}

static int debug_render(pbWindow* w, Rectangle rect, int input) {
	int old = rect.height;
	rect.height = 20;
	GuiLine(rect, TextFormat("Pixelbox : %s v.%i.%i", 
		PBOX_RELEASE_NAME, PBOX_VERSION_MAJOR, PBOX_VERSION_MINOR));
	rect.y += rect.height;
	GuiLine(rect, TextFormat("FPS : %i", (int)GetFPS()));
	rect.y += rect.height;
	rect.height = old;
	rect.y -= 40;
}

static void debug_destroy(pbWindow* w) {
	once = PBOX_CAST(pbWindow*, PBOX_NULL);
}

static const pbWindow debug_window = {
	sizeof(pbWindow),
	"debug",
	0, 0, 300, 200,
	NORMAL_WINDOW_FLAGS,
	debug_create,
	debug_render,
	debug_destroy
};

pbWindow* pbDebugWindowCreate() {
	return pbWindowClone(&debug_window, sizeof(pbWindow));
}

pbWindow* pbDebugWindowToggle() {
	if (!once) pbWinManAdd(&debug_window, sizeof(pbWindow));
	else once->flags |= PBOX_WINDOW_CLOSED;
}
