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
#include <draw/shaders.h>

static GLuint program;
static GLuint colormap;

extern uint8_t colormap_arr[256 * 256 * 3];

static void check_errors(GLuint shader) {
	char tmp[1024] = {0};
	int success = 1;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 1024, NULL, tmp);
		errorf(0, "Shader(%i) error : %s ", shader, tmp);
		abort();
	}
}

static void check_errors_prog(GLuint prog) {
	char tmp[1024] = {0};
	int success = 1;
	glGetProgramiv(prog, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(prog, 1024, NULL, tmp);
		errorf(0, "Program linking error : %p", tmp);
		abort();
	}
} 

// don't use this! Internal function!
int game_change_texture_size(struct box* b, int mode) {
	if (!b) return -1;
	// generate world representation texture
	// red color -> type
	// greeen color -> pack
	if (mode && !b->w && !b->h) {
		glDeleteTextures(1, &b->texture);
		debugf("Uninitialized world texture");
		return 0;
	}
	if (!mode) {
		glGenTextures(1, &b->texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, b->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // no filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, b->w, b->h, 0, GL_RG, GL_UNSIGNED_BYTE, b->arr);
		debugf("Initialized world texture (%i)", b->texture);
	} else { // resize
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, b->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, b->w, b->h, 0, GL_RG, GL_UNSIGNED_BYTE, b->arr);
		debugf("Resized world texture");
	}
	gl_check_error("game_change_texture_size");
	return 0;
}

void init_shaders() {
	GLuint fragment, vertex;
	// generate colormap for pixel types
	glGenTextures(1, &colormap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colormap);
	debugf("Colormap texture ID : %i", colormap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // no filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// typical color type, no magic here :)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, colormap_arr);
	glActiveTexture(GL_TEXTURE0);
	gl_check_error("generate colormap for pixel types");
	// init shaders
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex);
	check_errors(vertex);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment);
	check_errors(fragment);
	gl_check_error("shader compilation");
	// shader Program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	gl_check_error("shader program linking");
	check_errors_prog(program);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	gl_check_error("shader program creation finalization");
	// apply sampler uniforms to valid values
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "world"), 0);	
	glUniform1i(glGetUniformLocation(program, "colormap"), 1);	
	glUseProgram(0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	debugf("Shader sucessfully initialized!");
}

void free_shaders() {
	glDeleteProgram(program);
	glDeleteTextures(1, &colormap);
	debugf("Shader sucessfully freed!");
}

void game_draw_using_shader(struct box* b, float x, float y, float sx, float sy) {
	if (!b) return;
	gl_check_error("somewhere BEFORE drawing");
	glUseProgram(program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, b->texture);
	gl_check_error("binding world texture");
	// update texture data (texture size MUST BE SAME!)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, b->w, b->h, GL_RG, GL_UNSIGNED_BYTE, b->arr);
	gl_check_error("updating world texture");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colormap);
	gl_check_error("binding stuff for drawing");
	x  /= (float) b->w;
	y  /= (float) b->h;
	sx /= (float) b->w;
	sy /= (float) b->h;
	// i love legacy opengl A LOT
	glBegin(GL_TRIANGLE_FAN);
		glVertexAttrib2f(1,   x,   y);
		glVertexAttrib2f(0, -1.0,  1.0);
		glVertexAttrib2f(1,   x,  sy);
		glVertexAttrib2f(0, -1.0, -1.0);
		glVertexAttrib2f(1,  sx,  sy);
		glVertexAttrib2f(0,  1.0, -1.0);
		glVertexAttrib2f(1,  sx,   y);
		glVertexAttrib2f(0,  1.0,  1.0);
	glEnd();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	gl_check_error("drawing");
	glUseProgram(0);
}
