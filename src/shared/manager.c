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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static pbWinMan _WM;
static pbWinMan *WM = &_WM;

static int updateSize(PBOX_SIZE_T nsz) {
	PBOX_SIZE_T old = WM->size;
	WM->size = nsz;
	void* p = realloc(WM->array, WM->size * sizeof(pbWindow*));	
	if (p == PBOX_NULL) {
		return -1;
	}
	WM->array = PBOX_CAST(pbWindow**, p);
	if (nsz > old)
		memset(WM->array + old, 0, (nsz - old) * sizeof(pbWindow*)); // hehe
	return 0;
}

int  pbWinManCreate() {
	WM->size = 0;
	WM->count = 0;
	WM->array = PBOX_CAST(pbWindow**, PBOX_NULL);
	return updateSize(10);
}

pbWindow* pbWinManAdd(pbWindow* src, PBOX_SIZE_T size) {
	pbWindow* n = pbWindowClone(src, size);
	if (!n) return n;

	// no place in the array anymore -> resize array
	if (WM->size >= WM->count) {
		int stat = updateSize(WM->size + 5);	
		if (stat < 0) {
			pbWindowFree(n);
			fprintf(stderr, "mamalloc error in window array resizing\n");
			return PBOX_CAST(pbWindow*, 0); // memalloc error
		}
	}

	if (src->flags & PBOX_WINDOW_BACKGROUND) { // hard one. add window on bottom
		WM->count++;
		// shift array on top
		for (PBOX_SIZE_T i = WM->count; i > 0; i--) {
			WM->array[i] = WM->array[i-1];
		}
		WM->array[0] = n; // hehe
	} else {
		// add on top
		WM->array[WM->count++] = n;
	}

	return n; // OK
}

void pbWinManDestroy() {
	for (PBOX_SIZE_T i = 0; i < WM->count; i++) {
		if (WM->array[i] != PBOX_NULL) {
			pbWindowFree(WM->array[i]);
			WM->array[i] = PBOX_CAST(pbWindow*, PBOX_NULL);
		}
	}
	WM->select = PBOX_CAST(pbWindow*, PBOX_NULL);
	WM->count = 0;
	updateSize(0); // will return -1, but that's OK
}

static void getWindowBounds(pbWindow* f, Rectangle* bounds, Rectangle* statusBar, Rectangle* scaleDot,
	Rectangle *content);
static void getWindowButtons(pbWindow* f);

static int checkWindowCollision(Vector2 mouse, pbWindow* f) {
	Rectangle rect;
	Rectangle* null = PBOX_CAST(Rectangle*, PBOX_NULL);
	getWindowBounds(f, &rect, null, null, null); 
	return CheckCollisionPointRec(mouse, rect);
}

// if active window was closed
static int closed_active = 0;

void updateFocus() {
	Vector2 mouse = GetMousePosition();
	static Vector2 oldmouse = {0, 0};

	if ((mouse.x == oldmouse.x && mouse.y == oldmouse.y) && !closed_active) {
		return; // nothing to do
	}
	oldmouse = mouse;

	pbWindow* f = WM->select;

	// always check collisions now :p
	{
		pbWindow** p = WM->array + WM->count - 1;
		// from window on top, to the last one
		for (; p >= WM->array; p--) { 
			f = *p;
			if (f && checkWindowCollision(mouse, f)) {
				WM->select = f; // update selected window
				goto loop_end;
			}
		}
		WM->select = PBOX_NULL; // nothing to select
		return; // :(
	}
	loop_end:

	}

static void moveOnTop() {
	// update order if current window is not on top
	if (WM->select && WM->count && WM->array[WM->count-1] != WM->select) {
		// move array of windows to bottom by 1, insert selected window on top
		if (!(WM->select->flags & PBOX_WINDOW_BACKGROUND)) {
			PBOX_SIZE_T i = 0;
			pbWindow* f = WM->select; 
			for (; i < WM->count && WM->array[i] != f; i++); // search current selected window
			if (i == WM->count) { // Fatal error check
				fprintf(stderr, "fuck\n");
				return;
			}
			for (; i < WM->count-1; i++) { // MOVE windows
				WM->array[i] = WM->array[i+1];
			}
			WM->array[WM->count-1] = f; // DONE
		}
	}
}

static int pressing = 0;
static int scaling = 0;
#define SCALE_DOT_SIZE 15

static int windowHasBar(pbWindow* f) {
	return !(f->flags & PBOX_WINDOW_BORDERLESS);
}

static int windowCanMoveScale(pbWindow* f) {
	return !(f->flags & PBOX_WINDOW_PINNED);
}

#define RECT(...) (Rectangle)__VA_ARGS__
static void getWindowBounds(pbWindow* f, Rectangle* bounds, Rectangle* statusBar, Rectangle* scaleDot, Rectangle *content) {
	int h = f->flags & PBOX_WINDOW_NORMAL ? f->h : 24;
	if (bounds) *bounds = RECT({(float)f->x, (float)f->y, (float)f->w, (float)h});
	if (statusBar) {
		if (windowHasBar(f)) *statusBar = RECT({(float)f->x, (float)f->y, (float)f->w, 24.0f});
		else *statusBar = RECT({(float)f->x, (float)f->y, 0.0f, 0.0f});
	}
	if (scaleDot) {
		if (windowCanMoveScale(f)) 
			*scaleDot = RECT({(float)f->x + (float)f->w - SCALE_DOT_SIZE, (float)f->y +
				(float)f->h - SCALE_DOT_SIZE, SCALE_DOT_SIZE, SCALE_DOT_SIZE});
		else *scaleDot = RECT({ (float)f->x + (float)f->w - SCALE_DOT_SIZE, (float)f->y + 
			(float)f->h - SCALE_DOT_SIZE, 0, 0});
	}
	if (content) {
		if (windowHasBar(f)) {
			*content = RECT({(float)f->x, (float)f->y + 24, (float)f->w, (float)h - 24});
		} else {
			*content = RECT({(float)f->x, (float)f->y, (float)f->w, (float)f->h});
		}
	}
}

// not very useful out of this file :)
// window drawing code is taken from RAYGUI.H:1404 and modified for my amazing purposes
static int pbWindowRender (pbWindow* win, int update) {
	{
		int statusBarHeight = 24;
		Vector2 mouse = GetMousePosition();
		int onScale = 0;
	
		// calculate all positions
		Rectangle bounds, statusBar, scaleDot, contentBounds;
		getWindowBounds(win, &bounds, &statusBar, &scaleDot, &contentBounds);

		if (!GuiIsLocked()) {
			// if no scale dot -> it has zero width -> no need to draw and process
			if (scaleDot.width > 0 && CheckCollisionPointRec(mouse, scaleDot)) onScale = 1;

			// window movement state
			if (statusBar.width > 0 && CheckCollisionPointRec(mouse, statusBar)) {
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressing = 1;
			}

			// window scaling state
			if (onScale && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && windowCanMoveScale(win)) scaling = 1;

			// move only if not pinned and have border
			if (pressing && windowCanMoveScale(win)) {
				// movement
				Vector2 move = GetMouseDelta();	
				win->x += move.x;
				if (win->x < 0) win->x = 0;
				if (win->x + win->w > GetScreenWidth()) win->x = GetScreenWidth() - win->w;
				win->y += move.y;
				if (win->y < 0) win->y = 0;
				if (win->flags & PBOX_WINDOW_NORMAL) {
					if (win->y + win->h > GetScreenHeight()) win->y = GetScreenHeight() - win->h;
				} else {
					if (win->y + statusBarHeight > GetScreenHeight()) win->y = GetScreenHeight() - statusBarHeight;
				}
				// recalculate bounds
				getWindowBounds(win, &bounds, &statusBar, &scaleDot, &contentBounds);
			}

			// scale only if not pinned and have border
			if (scaling) {
				// scaling
				Vector2 move = GetMouseDelta();	
				win->w += move.x;
				if (win->w < 100) win->w	= 100;
				if (win->x + win->w > GetScreenWidth()) win->w = GetScreenWidth() - win->x;
				win->h += move.y;
				if (win->h < 50) win->h = 50;
				if (win->y + win->h > GetScreenHeight()) win->h = GetScreenHeight() - win->y;
				// recalculate bounds
				getWindowBounds(win, &bounds, &statusBar, &scaleDot, &contentBounds);
			}
		}

		if (windowHasBar(win) && bounds.height < statusBarHeight*2.0f) bounds.height = statusBarHeight*2.0f;
	
		Rectangle windowPanel = contentBounds;
		Rectangle closeButtonRec = {0}, hideButtonRec = {0}, pinButtonRec = {0};

		
		// Draw control
		if (windowHasBar(win)) { // Draw window status bar
			// we should have this buttons
 			closeButtonRec = RECT({ statusBar.x + statusBar.width - GuiGetStyle(STATUSBAR, BORDER_WIDTH) - 20,
				statusBar.y + statusBarHeight/2.0f - 18.0f/2.0f, 18, 18 });
			hideButtonRec = RECT({ statusBar.x + statusBar.width -(GuiGetStyle(STATUSBAR, BORDER_WIDTH) + 20)*2,
				statusBar.y + statusBarHeight/2.0f - 18.0f/2.0f, 18, 18 });
			pinButtonRec = RECT({ statusBar.x + GuiGetStyle(STATUSBAR, BORDER_WIDTH),
				statusBar.y + statusBarHeight/2.0f - 18.0f/2.0f, 18, 18 });

			const char* txt = TextFormat(win->flags & PBOX_WINDOW_PINNABLE ? "    %s" : "%s", win->title);
			GuiStatusBar(statusBar, txt); 
			// Draw window buttons
			int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
			int tempTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
			GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
			GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
			if ((win->flags & PBOX_WINDOW_CLOSEABLE)) { // close button
				if (GuiButton(closeButtonRec, GuiIconText(ICON_CROSS_SMALL,NULL)))
					win->flags |= PBOX_WINDOW_CLOSED;
			} 
			{	// hide button
				const char* icon = win->flags & PBOX_WINDOW_NORMAL ? 
					GuiIconText(ICON_ARROW_UP, NULL) :
					GuiIconText(ICON_ARROW_DOWN, NULL);
				Rectangle rec = win->flags & PBOX_WINDOW_CLOSEABLE ?
					hideButtonRec : closeButtonRec;
				if (GuiButton(rec, icon))
					win->flags ^= PBOX_WINDOW_NORMAL;
			}
			if ((win->flags & PBOX_WINDOW_PINNABLE)) {
				const char* icon = win->flags & PBOX_WINDOW_PINNED ? 
					GuiIconText(ICON_LOCK_OPEN, NULL) :
					GuiIconText(ICON_LOCK_CLOSE, NULL);
				if (GuiButton(pinButtonRec, icon)) 
					win->flags = win->flags ^ PBOX_WINDOW_PINNED;  
			}
			GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);
			GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlignment);	
		}

		int res = 0;
		if (win->flags & PBOX_WINDOW_NORMAL) {
			GuiPanel(windowPanel, NULL); // Draw window base
			// and content
			if (win->xRender) {
				BeginScissorMode(windowPanel.x, windowPanel.y, windowPanel.width, windowPanel.height);	
				res = win->xRender(win, windowPanel, update);
				EndScissorMode();
			}
			if (!(win->flags & PBOX_WINDOW_PINNED) && !GuiIsLocked()) {
				DrawTriangle(
					(Vector2){scaleDot.x,              (float)(win->y + win->h)},
					(Vector2){(float)(win->x + win->w), (float)(win->y + win->h)},
					(Vector2){(float)(win->x + win->w),  scaleDot.y},
				GetColor(GuiGetStyle(BUTTON, 1 + ((scaling ? 2 : onScale)*3))));
			}
		}
		return res;
	}
}


void pbWinManRender() {
	// update focus
	if ((!pressing && !scaling) || closed_active) {
		updateFocus();
		closed_active = 0;
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
			IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) ||
			IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) moveOnTop();
	}

	if (IsMouseButtonUp(MOUSE_LEFT_BUTTON)) {
		pressing = 0;
		scaling = 0;
	}

	const char* txt;
	if (WM->array) {
 		txt	= TextFormat("#%i active windows", WM->count);
	}
	DrawText(txt, 0, 0, 10, WHITE);

	int need_to_collect = 0;

	// draw windows
	for (PBOX_SIZE_T i = 0; i < WM->count; i++) {
		if (WM->array[i] != PBOX_NULL && !(WM->array[i]->flags & PBOX_WINDOW_CLOSED)) {
			// update only selected window!
			int flag = 0;
			if (WM->array[i] == WM->select) {
				GuiUnlock();
				flag = 1;
			}
			pbWindowRender(WM->array[i], flag);
			GuiLock();
		} else { 
			need_to_collect = 1;
		}
	}

	// remove closed windows from the array
	if (need_to_collect) {
		PBOX_SIZE_T rem = 0; // nothing to remove first time
		for (PBOX_SIZE_T i = 0; i < WM->count - rem; i++) {
			while (i+rem < WM->count && 
					(WM->array[i+rem] == PBOX_NULL ||
					WM->array[i+rem]->flags & PBOX_WINDOW_CLOSED)
				) {
				pbWindowFree(WM->array[i+rem]); // close and free window
				// if closed selected window
				if (WM->array[i+rem] == WM->select) {
					WM->select = PBOX_CAST(pbWindow*, PBOX_NULL);
					closed_active = 1; // need to reselect
				}
				rem++;
			}
			WM->array[i] = WM->array[i + rem];
		}
		WM->count -= rem;
	}
}

#include <stdio.h>

pbWindow* pbWindowClone(const pbWindow* src, PBOX_SIZE_T size) {
	if (size < sizeof(pbWindow)) return PBOX_NULL;
	if (src->size < sizeof(pbWindow)) {
		fprintf(stderr, "bad window structure size field value!\n");
		return PBOX_NULL;
	}
	if (size < src->size) return PBOX_NULL;
	void *p = calloc(size, 1);
	if (!p) {
		fprintf(stderr, "mamalloc error in window allocation\n");
		return PBOX_NULL;
	}
	pbWindow* out = PBOX_CAST(pbWindow*, p);
	memcpy(out, src, src->size);
	out->size = size;
	if (out->xCreate && out->xCreate(out) < 0) {
		fprintf(stderr, "window constructor error!\n");
		free(out);
		return PBOX_NULL;
	};
	fprintf(stderr, "window constructor OK!\n");
	return out;
}

void pbWindowFree (pbWindow* win) {
	if (!win) return;
	if (win->xDestroy) win->xDestroy(win);
	free(win);
}

