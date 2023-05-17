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
#include <stddef.h>
#include <string.h>

// debug and log functions

void (debugf) (const char* fmt, ...);
void (errorf) (int v, const char* fmt, ...); // v must be true
void gl_check_error(const char* stage);

// functions for window system

void (set_status) (const char*);
int  (should_exit) (void);

int   (get_key)    (int);
int   (get_button) (int);
float (mouse_x)    (void);
float (mouse_y)    (void);

void (main_tick) (void);
