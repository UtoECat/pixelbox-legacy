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
#include <draw/draw.h>
#include <stdlib.h>
#include <box.h>
#include <shaders.h>

static d_shader_t shader;
d_texture_t colormap;
extern uint8_t colormap_arr[256 * 256 * 3];

int game_shader_init() {
	// generate colormap for pixel types
	d_texture_init_ex(&colormap, GL_TEXTURE_2D, GL_RGB, GL_RGB);
	d_texture_load(&colormap, colormap_arr, MAX_PIXELS_TYPE, MAX_PIXELS_TYPE);
	d_texture_bind(&colormap, 1);
	glActiveTexture(GL_TEXTURE0);
	gl_check_error("pixelmap texture loading");
	// init shaders
	d_shader_init(&shader);
	if (d_shader_add(&shader, vertex_shader_text, GL_VERTEX_SHADER) != 0)
		return -1;
	if (d_shader_add(&shader, fragment_shader_text, GL_FRAGMENT_SHADER) != 0)
		return -1;
	if (d_shader_link(&shader) != 0) return -1;
	// apply sampler uniforms to valid values
	d_shader_bind(&shader);
	glUniform1i(d_shader_uniform_id(&shader, "world"   ), 0);
	glUniform1i(d_shader_uniform_id(&shader, "colormap"), 1);	
	d_shader_bind(NULL);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	debugf("Shader sucessfully initialized!");
	return 0;
}

void game_shader_free() {
	d_shader_free(&shader);
	d_texture_free(&colormap);
	debugf("Shader sucessfully freed!");
}

void game_draw(struct box* b, float x, float y, float w, float h, float camx, float camy, float camw, float camh) {
	if (!b) return;
	gl_check_error("before drawing game");
	d_shader_bind(&shader);
	gl_check_error("shader binding");
	d_texture_bind(&b->texture, 0); // world texture slot
	d_texture_subset(&b->texture, (uint8_t*)b->arr, 0, 0, b->w, b->h); // update texture
	d_texture_bind(&colormap, 1); // colormap slot
	d_shader_pass_matrixes(&shader); // pass matrixes
	w += x; h += y;
	// i love legacy opengl A LOT
	glBegin(GL_TRIANGLE_FAN);
		glVertexAttrib2f(1, camx, camh);
		glVertexAttrib2f(0,    x,    h);
		glVertexAttrib2f(1, camx, camy);
		glVertexAttrib2f(0,    x,    y);
		glVertexAttrib2f(1, camw, camy);
		glVertexAttrib2f(0,    w,    y);
		glVertexAttrib2f(1, camw, camh);
		glVertexAttrib2f(0,    w,    h);
	glEnd();
	d_texture_bind(NULL, 0);
	d_shader_bind(NULL);
	gl_check_error("drawing game");
}
