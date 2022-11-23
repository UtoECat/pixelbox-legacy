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
#include <stdlib.h>
#include <box.h>
#include <draw/draw.h>
#include <audio/music.h>

static struct box scene = {0};
static float camx = 0, camy = 0;
static const float cams = 1.0;
static float scale = 1.0f;

static uint8_t type = 1;
static void control() {
	if (get_key(GLFW_KEY_N)) {
		music_ost();
	};
	if (get_key(GLFW_KEY_G)) {
		game_tick(&scene, 1);
	}
	if (get_key(GLFW_KEY_C)) {
		game_free(&scene);
		game_init(&scene, 256, 256);
	}
	if (get_key(GLFW_KEY_W)) camy -= cams; 
	if (get_key(GLFW_KEY_S)) camy += cams; 
	if (get_key(GLFW_KEY_A)) camx -= cams; 
	if (get_key(GLFW_KEY_D)) camx += cams; 
	if (get_key(GLFW_KEY_EQUAL)) scale += 0.005; 
	if (get_key(GLFW_KEY_MINUS)) scale -= 0.005; 
	uint16_t x = (uint16_t)(mouse_x()/640.0*scene.w);
	uint16_t y = (uint16_t)(mouse_y()/480.0*scene.h);
	x += camx;
	y += camy;
	x %= scene.w;
	y %= scene.h;
	if (get_button(1)) {
		box_get(&scene, x, y)->type = 0;
		box_get(&scene, x, y)->pack = 0;
	}
	if (get_button(0)) {
		box_get(&scene, x, y)->type = type;
		box_get(&scene, x, y)->pack = 10;
	}
	if (get_key(GLFW_KEY_R)) {
		box_get(&scene, x, y)->type = rand() % MAX_PIXELS_TYPE;
	}
	if (get_key(GLFW_KEY_Q)) {
		type--;
	}
	if (get_key(GLFW_KEY_E)) {
		type++;
	}
}

void main_loop() {
	game_init(&scene, 256, 256);
	game_noise(&scene, 0.1);
	game_reload(&scene, "./world.bin");

	music_menu();
	music_volume(0.5);
	set_status("Heloo User :p");

	while (!should_exit()) {
		main_tick();
		draw_clear();
		draw_camera(0, 0, 640, 480);
		control();
		glPointSize(scale);
		game_draw_using_shader(&scene, camx, camy, camx + scene.w, camy + scene.h);
		glBindTexture(GL_TEXTURE_2D, 0);
		draw_color(1,1,1);
		draw_rect(mouse_x(), mouse_y(),10,10,1);
	};
	game_save(&scene, "./world.bin");
}
