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
#include <draw/gl.h>
#include <GLFW/glfw3.h>

extern uint8_t colormap_arr[256 * 256 * 3];

// drawing

void draw_clear(void);
void draw_camera(float x, float y, float w, float h);
void draw_rect(float x, float y, float w, float h, float z);
void draw_color(float r, float g, float b);
void draw_point(float x, float y, float z);

// shaders and textures
typedef struct gl_texture texture_t;
int  texture_init(texture_t* t, const void* data, int w, int h);
int  texture_set (const void* data, int x, int y, int w, int h);
int  texture_file(const char* file);
int  texture_subset();
int  shader_init()
