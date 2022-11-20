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
#include <stdint.h>
#include <stdarg.h>

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
	unsigned int texture;
};

// atom type processing
typedef void (*pixel_cb) (struct box*, uint16_t x, uint16_t y);

#define MAX_PIXELS_TYPE 256
extern pixel_cb pixel_proc[MAX_PIXELS_TYPE];
extern pixel_cb pixel_draw[MAX_PIXELS_TYPE];

// functions for pixelbox state

int  (game_init) (struct box* b, uint16_t w, uint16_t h);
int  (game_load) (struct box* b, const char* filename);
void (game_free) (struct box* b);

int  (game_reload) (struct box* b, const char* filename);
int  (game_save) (struct box* b, const char* filename);
void (game_tick) (struct box* b, uint8_t speed);
//void (game_draw) (struct box* b); deprecated
void game_draw_using_shader(struct box* b, float x, float y, float s, float sy);
void (game_noise) (struct box* b, float k);

static inline struct pixel* box_get(struct box* b, uint16_t x, uint16_t y) {
	if (x >= b->w) x = x % b->w;
	if (y >= b->h) y = y % b->h;
	return &b->arr[x + b->w * y];
}

// main initialization :D

int  (main_load) (void);
void (main_free) (void);
void (main_loop) (void);

// functions for window system

void (set_status) (const char*);
int  (should_exit) (void);
void (main_tick) (void);

int   (get_key)    (int);
int   (get_button) (int);
float (mouse_x) (void);
float (mouse_y) (void);

// drawing

void draw_clear(void);
void draw_camera(float x, float y, float w, float h);
void draw_rect(float x, float y, float w, float h, float z);
void draw_color(float r, float g, float b);
void draw_point(float x, float y, float z);

// audio system functions

void (music_menu) (void);
void (music_ost ) (void);
const char* (music_now) (void);

void (music_volume)      (float v);
float (music_get_volume) (void);

// debug and log functions

void (debugf) (const char* fmt, ...);
void (errorf) (int v, const char* fmt, ...); // v must be true
void gl_check_error(const char* stage);
