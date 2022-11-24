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
void debugf (const char* fmt, ...) 
__attribute__ ((format (printf, 1, 2)));
void errorf (const char* fmt, ...) 
__attribute__ ((format (printf, 1, 2)));
void warningf (const char* fmt, ...) 
__attribute__ ((format (printf, 1, 2)));
void crash (const char* fmt) 
__attribute__ ((__noreturn__));
void gl_check_error(const char* stage);

// functions for application
void (app_set_status) (const char*);
int  (app_should_exit) (void);

int   (app_get_key)    (int);
int   (app_get_button) (int);
float (app_mouse_x)    (void);
float (app_mouse_y)    (void);

// call this in your loop
void  (app_tick)       (void);

// should be declared
int  (app_init) (void);
void (app_loop) (void);
void (app_free) (void);
