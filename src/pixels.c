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

#include <main.h>
#include <stdio.h>
#include <GLFW/glfw3.h>

pixel_cb pixel_proc[MAX_PIXELS_TYPE] = {NULL};
pixel_cb pixel_draw[MAX_PIXELS_TYPE] = {NULL};

static float pixel_color[MAX_PIXELS_TYPE][3];

static void draw_pix(struct box* b, uint16_t x, uint16_t y) {
	uint8_t t = box_get(b, x, y)->type;
	draw_color(pixel_color[t][0], pixel_color[t][1], pixel_color[t][2]);
	draw_point(x, y, 0);
}

static void swap_pixels(struct pixel* a, struct pixel* b) {
	struct pixel c = *a;
	*a = *b;
	*b = c;
}
// a => from. b => to
static void move_pixel(struct pixel* a, struct pixel* b) {
	*b = *a;
	a->type = 0;
}	

// Liquid!

static inline int unpack_pixel (struct pixel* a, struct pixel *b) {
	if (a->pack >= 2) {
		a->pack /= 2;
		b->pack = a->pack;
		b->type = a->type;
		return 1;
	} else {
		*b = *a;
		a->type = 0;
		return 0;
	};
}

static inline int sync_pixels(struct pixel* a, struct pixel *b) {
	if (a->pack + b->pack >= 2) {
		a->pack = (a->pack + b->pack) / 2;
		b->pack = a->pack;
		return 1;
	} else {
		a->pack += b->pack;
		*b = *a;
		a->type = 0;
		return 0;
	};	
}

static int subproc_liquid(struct pixel* p, struct pixel* d) {
	if (d->type == 0) {
		if (!unpack_pixel(p, d)) return 1;
	} else if (d->type == p->type) {
		if (!sync_pixels(p, d)) return 1;
	}
	return 0;
}

static void proc_liquid(struct box* b, uint16_t x, uint16_t y) {
	struct pixel* p = box_get(b, x, y);
	struct pixel* d = NULL;
	if (y+1 != b->h) {
		d = box_get(b, x, y + 1);
		if (subproc_liquid(p, d)) return;
	}
	d = box_get(b, x + 1, y);
	if (subproc_liquid(p, d)) return;
	d = box_get(b, x - 1, y);
	if (subproc_liquid(p, d)) return;
}

// Sand

static void proc_sand(struct box* b, uint16_t x, uint16_t y) {
	if (y+1 == b->h) return;
	if (box_get(b, x, y + 1)->type == 0) {
		move_pixel(box_get(b, x, y), box_get(b, x, y+1));
	} else {
		int k = (x+y) % 2 == 0 ? 1 : -1;
		if (box_get(b, x+k, y+1)->type == 0 && box_get(b, x+k, y)->type == 0) {
			move_pixel(box_get(b, x, y), box_get(b, x+k, y+1));
		}
	}
}

void init_pixel_types(void) {
	for (int i = 0; i < MAX_PIXELS_TYPE; i++) {
		pixel_color[i][0] = (i & 7) / 7.0f;
		pixel_color[i][1] = (i >> 3 & 7) / 7.0f;
		pixel_color[i][2] = (i >> 6 & 3) / 3.0f;
	}
	for (int i = 0; i < MAX_PIXELS_TYPE; i++) {
		pixel_draw[i] = draw_pix;
		pixel_proc[i] = proc_sand;
	}
	pixel_proc[10] = proc_liquid;
	debugf("Pixel types loaded!");
}

void free_pixel_types(void) {
	debugf("Pixel types freed!");
}
