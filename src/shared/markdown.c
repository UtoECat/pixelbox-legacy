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
#include <math.h>

static Texture texture_cache[10] = {0};
static int     textures_count = 0;
static const char* old = PBOX_CAST(const char*, PBOX_NULL);
static int is_code = 0;
static int is_bold = 0;
static int header = 0;

static void drawTextLine(Rectangle rec, Vector2* pos, const char** ptr) {
	const char* c;
	Font font = GuiGetFont();

	c = *ptr; // setup pointer

	// check for spaces at the start
	int spaces = 0;
	int tabs = 0;
	while (*c == ' ' || *c == '\t') {
		if (*c == ' ') spaces++;
		else tabs++; 
		c++;
	}

	// check for header
	while (*c == '#') {
		header++;
		c++;
	}

	// list
	if (*c == '-') {
		is_bold = -1; // remove at next line
	}

	// setup varables
	int lsz = 0; // codepoint size
	float _mul = header ? (log(5.5-header+1)*log(7))-1 : 1;
	float fontSize = (float)GuiGetStyle(DEFAULT, TEXT_SIZE) * _mul;
	float fontSpaceing = (float)GuiGetStyle(DEFAULT, TEXT_SPACING) * _mul;
	float scaleFactor = fontSize/font.baseSize;

	// add starting spaces and tabs
	if (spaces) {
		int index = GetGlyphIndex(font, ' ');
		float w = (font.glyphs[index].advanceX ? font.glyphs[index].advanceX : font.recs[index].width);
		pos->x += (w*scaleFactor + fontSpaceing) * spaces;
	}
	if (tabs) {
		int index = GetGlyphIndex(font, '\t');
		float w = (font.glyphs[index].advanceX ? font.glyphs[index].advanceX : font.recs[index].width);
		pos->x += (w*scaleFactor + fontSpaceing) * tabs * 2;
	}

	Color tint; 
	Color orig_color = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
	Color bold_color = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));

	if (is_bold) {
		tint = bold_color;
		fontSize += 2;
	} else tint = orig_color;
	tint.a = 255;

	for (; *c != '\0'; c += lsz) {
		int lc;
	 	lsz = 0;
	 	lc = GetCodepointNext(c, &lsz);

		if (lc == '*' && is_bold >= 0) {
			if (is_bold) {
				tint = orig_color;
				fontSize -= 2;
			}
			else {
				tint = bold_color;
				fontSize += 2;
			}
			is_bold = !is_bold;
			continue;
		}

		if (lc == '\n') {
			if (is_bold < 0) is_bold = 0;
			header = 0;
			c++;
			break; // process next line in next call
		}

		// special chars
		if (lc == '\\') {
			int tsz = 0;
			c++;
			if (GetCodepointNext(c, &tsz) == '\\') {
				lc = '\\'; lsz = 1;
			} else if (GetCodepointNext(c, &tsz) == '*') {
				lc = '*'; lsz = 1;
			}
		}

		if (lc == 0x3f) lsz = 1; // bad UNICODE

		int index = GetGlyphIndex(font, lc); 
		if ((lc != ' ') && (lc != '\t')) {
			// draw at new line if not fits
    	if (pos->x > (rec.x + rec.width - font.recs[index].width)) break;
      DrawTextCodepoint(font, lc, (Vector2){pos->x, pos->y}, fontSize,
				tint);
		}
		// add offset
		float w = (font.glyphs[index].advanceX ? font.glyphs[index].advanceX : font.recs[index].width);
		pos->x += (w*scaleFactor + fontSpaceing) * (lc == '\t' ? 2 : 1);
	}

	//if (*c != '\0') c++; // failcheck
	*ptr = c;
	// reset position
	pos->y += (float)font.baseSize * scaleFactor + 3;
	pos->x  = rec.x;
}

// and draw too
int GuiMarkdown(Rectangle rec, const char* src) {
	if (src != old) { // cleanup image cache
		old = src;
		for (int i = 0; i < textures_count; i++) {
			UnloadTexture(texture_cache[i]);
		}
		textures_count = 0;
	}

	const char* c = src;
	Vector2 pos = {rec.x, rec.y};
	int old = GuiGetStyle(DEFAULT, TEXT_SIZE);
	is_bold = 0;
	is_code = 0;
	header  = 0;

	while (*c != '\0') {
		drawTextLine(rec, &pos, &c);
	}

	GuiSetStyle(DEFAULT, TEXT_SIZE, old);
	return pos.y;
}
