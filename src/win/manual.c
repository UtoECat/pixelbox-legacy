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

#include "../config.h"
#include "../window.h"
#include <math.h>

// debug window implementation

static pbWindow* once = PBOX_CAST(pbWindow*, PBOX_NULL);

struct manual_page {
	const char* title;
	const char* content;
	Vector2     scroll;
	int         height;
};

extern const char* const license_string;

#define PAGES_COUNT 6
#define PLACEHOLDER "lorem\ninpsulm\nfuck\nme\noh\tf\nno"
struct manual_page Manual[PAGES_COUNT] = {
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
	}
};

static int active_tab = 0;

static int debug_create(pbWindow* W) {
	if (once) return -1;
	once = W;
	Manual[5].content = license_string;
	return 0;	
}

int GuiTabBarEx(Rectangle bounds, int width, int clb, const char **text, int count, int *active);

static int debug_render(pbWindow* w, Rectangle rect, int input) {
	Rectangle tabs_rect = rect;
	tabs_rect.height = 25;
	rect.y += 25;
	rect.height -= 25;

	const char* cats[PAGES_COUNT];
	for (int i = 0; i < PAGES_COUNT; i++) {
		cats[i] = Manual[i].title;
	}

	int oldtab = active_tab; // to check for changes
	GuiTabBarEx(tabs_rect, 95, 0, cats, PAGES_COUNT, &active_tab);
	
	GuiScrollPanel(rect, (char*)0, (Rectangle){0, 0, 400,
		Manual[active_tab].height}, &(Manual[active_tab].scroll));
	Vector2 scroll = Manual[active_tab].scroll;
	
	EndScissorMode();
	BeginScissorMode(rect.x, rect.y, rect.width, rect.height);
	rect.x += 5;
	rect.width -= 5;

	int h = GuiMarkdown((Rectangle){rect.x, rect.y + scroll.y, rect.width, rect.height},
		Manual[active_tab].content								
	);
	Manual[active_tab].height = h;
}

static void debug_destroy(pbWindow* w) {
	once = PBOX_CAST(pbWindow*, PBOX_NULL);
}

static const pbWindow debug_window = {
	sizeof(pbWindow),
	"information",
	0, 0, 450, 350,
	NORMAL_WINDOW_FLAGS,
	debug_create,
	debug_render,
	debug_destroy
};

void pbManualWindowToggle() {
	if (!once) pbWinManAdd(&debug_window, sizeof(pbWindow));
	else once->flags |= PBOX_WINDOW_CLOSED;
}
