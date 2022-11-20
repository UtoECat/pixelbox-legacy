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
#include <stdlib.h>
#include <galogen/gl.h>
#include <GLFW/glfw3.h>

static struct box scene = {0};
static float camx = 0, camy = 0;
static const float cams = 1.0;
static float scale = 3.0f;

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
	if (get_key(GLFW_KEY_EQUAL)) scale += 0.01; 
	if (get_key(GLFW_KEY_MINUS)) scale -= 0.01; 
	uint16_t x = (uint16_t)((mouse_x()+camx)/scale) % scene.w;
	uint16_t y = (uint16_t)((mouse_y()+camy)/scale) % scene.h;
	if (get_button(1)) {
		box_get(&scene, x, y)->type = 30;
		box_get(&scene, x, y)->pack = 0;
	}
	if (get_button(0)) {
		box_get(&scene, x, y)->type = 10;
		box_get(&scene, x, y)->pack = 10;
	}
	if (get_key(GLFW_KEY_R)) {
		box_get(&scene, x, y)->type = rand() % MAX_PIXELS_TYPE;
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
		//draw_camera(0, 0, 640, 480);
		control();
		glPointSize(scale);
		game_draw_using_shader(&scene, 0, 0, camx, camy, camx + 256, camy + 256);
		glBindTexture(GL_TEXTURE_2D, 1);
		glBegin(GL_TRIANGLE_FAN);
			glColor3f(1,1,1);
			glTexCoord2f(0,0);
			glVertex2f(0,0);
			glTexCoord2f(1,0);
			glColor3f(1,1,1);
			glVertex2f(1,0);
			glTexCoord2f(1,1);
			glColor3f(1,1,1);
			glVertex2f(1,1);
			glTexCoord2f(0,1);
			glColor3f(1,1,1);
			glVertex2f(0,1);
		glEnd();
	};
	game_save(&scene, "./world.bin");
}
