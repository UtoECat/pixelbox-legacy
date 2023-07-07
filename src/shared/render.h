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

#include <raylib.h>

int  pbRenderCreate(int isClient);
void pbRenderSetFullscreen(int v);
void pbRenderDestroy(void);

int GuiLoadTheme();
int GuiToggleTheme();

int GuiTextView(Rectangle rec, const char* src);
float getStringHeight(const char* str);
float getLineHeight();
