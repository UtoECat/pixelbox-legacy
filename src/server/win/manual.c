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
#include "info.h"

// debug window implementation


extern const char* const license_string;

struct manual_page Manual[] = {
	{"general", 
	"# About Pixelbox\n"
	"\tPixelbox is an 'infinite' sandbox game. Your entire world actually limited, but this limits "
	"are very big : your world can contain *2^32 \\* 2^32* pixels in total.\n"
	"\tPixelbox uses *Sqlite3* database to store all this data as efficient as possible, "
	"uses multiple threads to process your world and also processes only interrupted by player, "
	"or by another pixel pieces of world -> chunks.\n"
	"World have limited amount of materials : *64*, and 4 color variations for each.\n"
	"Also, client-server model is used, to allow multiplayer in future...\n"
	"# What i can do in Pixelbox?\n"
	"nothing, since this all is still in refactoring :p"
	},
	{"interface",
	"## good luck :)\n"
	},
	{"security", 
	"at current moment of development, there is *NO ANY ENCRYPTION* "
	"for data sended between client and server, and also no any "
	"ways to validate separate clients and servers! I will add ssh "
	"encrypction in future, whan basics will be done!\n"
	"# conclusion\n"
	"DO NOT SEND ANY SENSITIVE, PRIVATE DATA IN GLOBAL CHAT, AND DO NOT "
	"DRAW IT IN THE WORLD!"
	}, 
	{"server",
		"Server can do only ne world, filled up win manu players at once.\n"
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
	}, {
		(char*) 0,
		(char*) 0
	}
};

static pbWindow* once = PBOX_CAST(pbWindow*, PBOX_NULL);

static void debug_destroy(pbWindow* w) {
	once = PBOX_CAST(pbWindow*, PBOX_NULL);
	pbSaveWindowData(w, "manual");
}

void pbManualWindowToggle() {
	Manual[5].content = license_string;
	if (!once) {
		struct manual_window* w = pbManualWindowCreateAndAdd(&(Manual[0]));
		if (w) w->window.xDestroy = debug_destroy;
		if (pbLoadWindowData(&(w->window), "manual") <= 0) {
			pbLog(LOG_ERROR, "can't load manual!");
			w->window.w = 480;
			w->window.h = 360;
			w->window.x = GetScreenWidth()/2- w->window.w/2;
			w->window.y = GetScreenHeight()/2- w->window.h/2;
		}
		once = PBOX_CAST(pbWindow*, w);
	} else once->flags |= PBOX_WINDOW_CLOSED;
}
