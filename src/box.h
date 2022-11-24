/*
 * PixelBox
 * Copyright (C) 2022 UtoECat

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <main.h>
#include <draw/draw.h>

#define MAX_PIXELS_TYPE 256

// atom of world
struct pixel {
	uint8_t type;
	uint8_t pack;
};

_Static_assert(sizeof(struct pixel) == sizeof(short));

// world itself
struct box {
	uint16_t w, h;
	struct pixel* arr;
	d_texture_t texture;
};

// functions for pixelbox state

int  (game_init) (struct box* b, uint16_t w, uint16_t h);
int  (game_load) (struct box* b, const char* filename);
void (game_free) (struct box* b);

int  (game_reload) (struct box* b, const char* filename);
int  (game_save) (struct box* b, const char* filename);
void (game_tick) (struct box* b, uint8_t speed);

void (game_noise) (struct box* b, float k);

static inline struct pixel* box_get(struct box* b, uint16_t x, uint16_t y) {
	if (x >= b->w) x = x % b->w;
	if (y >= b->h) y = y % b->h;
	return &b->arr[x + b->w * y];
}

void game_draw(struct box* b, float x, float y, float w, float h, float camx, float camy, float camw, float camh);
