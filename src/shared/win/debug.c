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
#include "info.h"
#include "render.h"

// debug window implementation

static pbWindow* once = PBOX_CAST(pbWindow*, PBOX_NULL);

static int debug_create(pbWindow* W) {
	if (once) return -1;
	once = W;
	return 0;	
}

int GuiMarkdown(Rectangle rec, const char* src);

static Vector2 scroll;

static int debug_render(pbWindow* w, Rectangle rect, int input) {
	Rectangle unused;	
	
	int   lines_count = pbLogGetLinesCount();
	int   available   = rect.height / getLineHeight();
	float log_height  = lines_count * getLineHeight();

	int textshift     = -scroll.y / getLineHeight();
	const char*	log = pbLatestLog(available + textshift);

	GuiScrollPanel(rect, (char*)0, (Rectangle){0, 0, 400, log_height},
		&scroll, &unused);

	if (*log == '\n') log++;
	GuiTextView(rect, log);
}

static void debug_destroy(pbWindow* w) {
	once = PBOX_CAST(pbWindow*, PBOX_NULL);
	pbSaveWindowData(w, "debug_log");
}

static const pbWindow debug_window = {
	sizeof(pbWindow),
	"Debug Log",
	0, 0, 300, 200,
	NORMAL_WINDOW_FLAGS,
	debug_create,
	debug_render,
	debug_destroy
};

pbWindow* pbDebugWindowToggle() {
	if (!once) {
		pbWindow* w = pbWinManAdd(&debug_window, sizeof(pbWindow));
		if (pbLoadWindowData(w, "debug_log") <= 0) {
			w->w = 480;
			w->h = 360;
			w->x = GetScreenWidth()/2- w->w/2;
			w->y = GetScreenHeight()/2- w->h/2;
			w->flags ^= PBOX_WINDOW_NORMAL;
		}
	} else once->flags |= PBOX_WINDOW_CLOSED;
}
