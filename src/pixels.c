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

pixel_cb pixel_proc[MAX_PIXELS_TYPE] = {NULL};
//pixel_cb pixel_draw[MAX_PIXELS_TYPE] = {NULL} deprecated
uint8_t colormap_arr[256 * 256 * 3];

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
#define SAME(A, B) (A->type == B->type)
#define VOID(A) (A->type == 0)
#define ADDIFSAME(B, A) if SAME(B, A) {cnt++; v+= A->pack;} else if VOID(A) cnt++;
#define ADDIFSAMEANDNOTNIL(B, A) if (A && SAME(B, A)) {cnt++; v+= A->pack;} else if (A && VOID(A)) cnt++;
#define APPLYVAL(B, A, V) if (SAME(B, A) || VOID(A)) {A->pack = V; A->type = B->type;}
static void proc_liquid(struct box* o, uint16_t x, uint16_t y) {
	struct pixel* b = box_get(o, x, y);
	struct pixel* a = box_get(o, x-1, y);
	struct pixel* c = box_get(o, x+1, y);
	struct pixel* d = NULL;
	if (y + 1 != o->h) d = box_get(o, x, y + 1);
	int cnt = 1; int v = b->pack;
	// get particle system summary
	ADDIFSAME(b, a);
	ADDIFSAME(b, c);
	ADDIFSAMEANDNOTNIL(b, d);
	// calculate
	if (v < cnt) { // pack ALL in middle
		if SAME(b, a) a->type = 0;
		if SAME(b, c) c->type = 0;
		if (d && (SAME(b, d) || VOID(d))) { // move bottom
			d->type = b->type;
			b->type = 0;
			d->pack = v;
		} else b->pack = v;
	} else { // if can normalize
		int one = v / cnt;
		int err = v - (one * cnt);
		int luck = (x + y) % cnt;
		int pos = 0;

		b->pack = one + ((luck == pos++) ? err : 0);
		APPLYVAL(b, a, one + ((luck == pos++) ? err : 0));
		APPLYVAL(b, c, one + ((luck == pos++) ? err : 0));
		if (d) {APPLYVAL(b, d, one + ((luck == pos++) ? err : 0));}
		// end of normalize
	}
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

// solid
static void proc_solid (struct box* b, uint16_t x, uint16_t y) {

}


void init_pixel_types(void) {
	for (int y = 0; y < 256; y++) {
		for (int i = 0; i < 256; i++) {
			const int pos = (i + y * 256) * 3;
			colormap_arr[pos + 0] = (i & 7) / 7.0f * 255;
			colormap_arr[pos + 1] = (i >> 3 & 7) /7.0f * 255;
			colormap_arr[pos + 2] = (i >> 6 & 3) / 3.0f * 255;
		}
	}

	for (int i = 0; i < MAX_PIXELS_TYPE; i++) {
		pixel_proc[i] = proc_sand;
	}
	pixel_proc[10] = proc_liquid;
	pixel_proc[30]  = proc_solid;
	debugf("Pixel types loaded!");
}

void free_pixel_types(void) {
	debugf("Pixel types freed!");
}
