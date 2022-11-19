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

void debugf (const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "[DEBUG] : ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void errorf (int v, const char* fmt, ...) {
	if (v) return;
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "[ERROR] : ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}

int main() {
	debugf("Hello World!");

	int v = main_load();
	errorf(v == 0, "Can't init game properly!");
	errorf(v >= 0, "Fatal : Init failed (code %i)!", v);
	if (v < 0) return v;
	main_loop();
	main_free();
	debugf("Goodbye!");
	return 0;
}
