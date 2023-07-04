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
#include <raylib.h>
#include "sus.h"

// debug window implementation

static int refcnt = 0;
static Texture sus;
static pbWindow* once = PBOX_CAST(pbWindow*, PBOX_NULL);

static int debug_create(pbWindow* W) {
	if (once) return -1;
	once = W;

	if (!refcnt) {
		Image img = {
			SUS_DATA,
			SUS_WIDTH,
			SUS_HEIGHT,
			1,
			SUS_FORMAT
		};
		sus = LoadTextureFromImage(img);
	}
	refcnt++;
	return 0;
}

#define ETABS 5
static char* cats[ETABS] = {
	"info", "edit", "world", "settings", "debug"
};

static int active_tab = 0;
extern const char* license_string;
extern int GuiToggleTheme();

int GuiTabBarEx(Rectangle bounds, int width, int clb, const char **text, int count, int *active);
static Vector2 Escroll[ETABS] = {{0, 0}};
static Eheights[ETABS] = {475, 100, 100, 100};

#define PBOX_DESCRIPTION "infinite sandbox game"

static int debug_render(pbWindow* w, Rectangle rect, int input) {
	Rectangle tabs = rect;
	tabs.height = 25;
	rect.y += 25;
	rect.height -= 25;
	GuiTabBarEx(tabs, 75, 0, cats, ETABS, &active_tab);
	Rectangle unused;
	GuiScrollPanel(rect, (char*)0, (Rectangle){0, 0, 300, Eheights[active_tab]}, &Escroll[active_tab], &unused);
	Vector2 scroll = Escroll[active_tab];
	
	EndScissorMode();
	BeginScissorMode(rect.x, rect.y, rect.width, rect.height);
	switch (active_tab) {
		case 0 : { // INFO TAB
			int y = rect.y + scroll.y + 10;
			DrawTexture(sus, rect.x + rect.width/2 - SUS_WIDTH/2, y, WHITE);
			y += SUS_HEIGHT - 10;
			GuiDrawText(TextFormat("Pixelbox %s - %s\n v.%i.%i\n BUILD %s %s", 
					PBOX_RELEASE_NAME, PBOX_DESCRIPTION, PBOX_VERSION_MAJOR,
					PBOX_VERSION_MINOR, __DATE__, __TIME__),
				(Rectangle){rect.x, y, rect.width, 80}, TEXT_ALIGN_CENTER, 
				GetColor(GuiGetStyle(DEFAULT, GuiGetState() == STATE_DISABLED ? 
					BORDER_COLOR_DISABLED : LINE_COLOR))
			);
			y += 80;
			Rectangle box = {rect.x + rect.width/4, y, rect.width/2, 25};
			if (GuiButton(box, "Get Source Code")) {
				OpenURL("https://github.com/UtoECat/pixelbox");
			};
			box.y += 30;
			if (GuiButton(box, "Show Help")) {
				pbManualWindowToggle();
			}
		}; break;
		case 1 : { // EDIT TAB
			int y = rect.y + scroll.y + 10;
			Rectangle box = {rect.x + rect.width/4, y, rect.width/2, 25};
			GuiButton(box, "Gui");
		}; break;
		case 2 : { // WORLD TAB
			int y = rect.y + scroll.y + 10;

		}; break;
		case 3 : { // SETTINGS
			int y = rect.y + scroll.y + 10;
			Rectangle box = {rect.x + rect.width/4, y, rect.width/2, 25};
			if (GuiButton(box, "Change theme")) {
				GuiToggleTheme();
			};
		}; break;
		case 4 : { // DEBUG
			int y = rect.y + scroll.y + 10;
			Rectangle box = {rect.x + rect.width/4, y, rect.width/2, 25};
			if (GuiButton(box, "Image Test")) {
				pbWindow* w = pbSuspiciousWindowCreate();
				pbWinManAdd(w, w->size);
				pbWindowFree(w);
			};
			box.y += box.height + 5;
			if (GuiButton(box, "Debug Info...")) {
				pbDebugWindowToggle();
			};
		}; break;
		default : // error?
		break;
	};
}

static void debug_destroy(pbWindow* w) {
	once = PBOX_CAST(pbWindow*, PBOX_NULL);
	refcnt--;
	if (!refcnt) UnloadTexture(sus);
}


static pbWindow explorer_window = {
	sizeof(pbWindow),
	"pixelbox",
	0, 0, 480, 350,
	PBOX_WINDOW_NORMAL | PBOX_WINDOW_PINNABLE,
	debug_create,
	debug_render,
	debug_destroy
};

pbWindow* pbExplorerWindowStatic() {
	return &explorer_window;
}
