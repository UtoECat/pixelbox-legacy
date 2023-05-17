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

// main initialization :D

int  (main_load) (void);
void (main_free) (void);

// private init
int  music_init(void);
void music_free(void);
void music_tick(void);
void init_pixel_types(void);
void free_pixel_types(void);
void init_shaders();
void free_shaders();
int  window_init();
void window_free();
void window_tick();

// should be declared
void (main_loop) (void);

