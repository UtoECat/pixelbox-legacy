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
#include "rescli.h"
#include "manual.h"
#include <math.h>

// debug window implementation

extern const char* const license_string;

/*
#define PLACEHOLDER "lorem\ninpsulm\nfuck\nme\noh\tf\nno"
struct manual_page Manual[] = {
	{"general", 
	"# About Pixelbox\n"
	"\tPixelbox is an 'infinite' sandbox game. Your entire world actually limited, but this limits "
	"are very big : your world can contain *2^32 \\* 2^32* pixels in total.\n"
	"\tPixelbox uses *Sqlite3* database to store all this data as efficient as possible, "
	"uses multiple threads to process your world and also processes only interrupted by player, "
	"or by another pixel pieces of world -> chunks.\n"
	"World have limited amount of materials : *64*, and 4 color variations for each.\n"
	"# What i can do in Pixelbox?\n"
	"nothing, since this all is still in refactoring :p"
	},
	{"interface",
	"## good luck :)\n"
	},
	{"controls", 
	"cxuhdshdffds" PLACEHOLDER
	}, 
	{"worlds",
	"wodjsddfad" PLACEHOLDER
	},
	{"internet", 
	"# Header 1\n"
	"\ttext 1\n"
	"## Header2\n"
	"\ttext 2\n"
	"### Header 3\n"
	"\ttext 3, *bold text 1*, text 4\n"
	},
	{"license",
	""
	},
	{(const char*)0,
	(const char*)0
	}
};

void pbManualWindowToggle() {
	if (!once) pbWinManAdd(&debug_window, sizeof(pbWindow));
	else once->flags |= PBOX_WINDOW_CLOSED;
}

static int active_tab = 0;

static pbWindow* once = PBOX_CAST(pbWindow*, PBOX_NULL);
static int debug_create(pbWindow* W) {
	if (once) return -1;
	once = W;
	Manual[5].content = license_string;
	return 0;	
} */

int GuiTabBarEx(Rectangle bounds, int width, int clb, const char **text, int count, int *active);

static int pages_count(struct manual_page *p) {
	int count = 0;
	while (p && p->title != (char*)0 && p->content != (char*)0) {
		p++;
		count++;
	}
	return count;
}

static int manual_render(pbWindow* o, Rectangle rect, int input) {
	struct manual_window* w = PBOX_CAST(struct manual_window*, o);
	if (!w->manual) return -1; // oh no
	Rectangle tabs_rect = rect;
	tabs_rect.height = 25;
	rect.y += 25;
	rect.height -= 25;

	int count = pages_count(w->manual);
	const char* cats[count];

	for (int i = 0; i < count; i++) {
		cats[i] = w->manual[i].title;
	}

	GuiTabBarEx(tabs_rect, 95, 0, cats, count, &(w->active_tab));

	Rectangle unused;	
	GuiScrollPanel(rect, (char*)0, (Rectangle){0, 0, 400,
		w->manual[w->active_tab].height}, &(w->manual[w->active_tab].scroll), &unused);
	Vector2 scroll = w->manual[w->active_tab].scroll;
	
	EndScissorMode();
	BeginScissorMode(rect.x, rect.y, rect.width, rect.height);
	rect.x += 5;
	rect.width -= 5;

	int h = GuiMarkdown((Rectangle){rect.x, rect.y + scroll.y, rect.width, rect.height}, w->manual[w->active_tab].content);
	w->manual[w->active_tab].height = h;
	return 0;
}

#include <stdio.h>

static int manual_create(pbWindow* o) {
	struct manual_window* w = PBOX_CAST(struct manual_window*, o);
	// some failchecks
	if (o->size < sizeof(struct manual_window)) {
		fprintf(stderr, "size is fucked up!\n");
		return -1;
	}
	return 0;
}

static void manual_destroy(pbWindow*) {
	// nothing to do
}

static const struct manual_window manual_window = {
	{
	sizeof(struct manual_window),
	"information",
	0, 0, 450, 350,
	NORMAL_WINDOW_FLAGS,
	manual_create,
	manual_render,
	manual_destroy
	},
	(struct manual_page*)0,
	0
};

struct manual_window* pbManualWindowCreateAndAdd(struct manual_page* man) {
	if (!man) return PBOX_CAST(struct manual_window*, 0);
	pbWindow* o = pbWinManAdd((pbWindow*)&manual_window, sizeof(struct manual_window));
	struct manual_window* w = PBOX_CAST(struct manual_window*, o);
	if (w) {
		w->manual = man;
	}
	return w;
}
