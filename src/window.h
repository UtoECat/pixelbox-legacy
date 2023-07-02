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

#pragma once
#include "config.h"
#include "ext/raygui.h"

// custom subwindows implementation
typedef struct screen_impl pbWindow;

enum pbWindowFlags {
	PBOX_WINDOW_NORMAL    = 0b00000001, // minimized if not setted
	PBOX_WINDOW_BORDERLESS= 0b00000010, // border and X button :)
	PBOX_WINDOW_PINNED    = 0b00000100, // static position and size if setted 
	PBOX_WINDOW_CLOSEABLE = 0b00001000, // window can be closed forever 
	PBOX_WINDOW_PINNABLE  = 0b00010000, // pin button
																			// pin and close button, resize and move requires border!
	PBOX_WINDOW_BACKGROUND= 0b00100000, // always at bottom
	PBOX_WINDOW_CLOSED    = 0b10000000  // will be destroyed on next tick
};

#define BACKGROUND_WINDOW_FLAGS PBOX_WINDOW_BACKGROUND | PBOX_WINDOW_BORDERLESS |\
PBOX_WINDOW_PINNED | PBOX_WINDOW_NORMAL

#define NORMAL_WINDOW_FLAGS PBOX_WINDOW_NORMAL | PBOX_WINDOW_CLOSEABLE | PBOX_WINDOW_PINNABLE
#define POPUP_WINDOW_FLAGS PBOX_WINDOW_NORMAL | PBOX_WINDOW_CLOSEABLE | PBOX_WINDOW_PINNED

struct screen_impl { // should be malloced
	PBOX_SIZE_T size;  // this structure size
	const char* title; // should be malloced or be NULL
	int  x, y, w, h;   // position
	char flags; // hehe
	int  (*xCreate)  (pbWindow*); // init new instance
	int  (*xRender)  (pbWindow*, Rectangle rect, int input); // process and draw
	void (*xDestroy) (pbWindow*); // destroy
};

typedef struct window_manager pbWinMan;

struct window_manager {
	pbWindow** array;
	pbWindow  *select; // selected window
	PBOX_SIZE_T size, count;
};

// highlevel (window manager)
// creates wm
int  pbWinManCreate();

// adds window from structure. Returns -1 on error
int  pbWinManAdd(pbWindow* src, PBOX_SIZE_T size);

void pbWinManDestroy(); // destroys window manager and all windows
void pbWinManRender(); // renders and processes all windows

// lowlevel
// calls create
pbWindow* pbWindowClone(const pbWindow* src, PBOX_SIZE_T size);

// calls destroy too
void      pbWindowFree (pbWindow* w); 

// Window implemented here
pbWindow* pbDebugWindowCreate(void);
pbWindow* pbBlankWindowCreate(void);
pbWindow* pbSuspiciousWindowCreate(void);
