/*
** This file is a part of PixelBox - infinite sandbox game
** General Info provider
** Copyright (C) 2021-2023 UtoECat
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "config.h"
#include <stdio.h>

const char* summary_format = "Pixelbox (%s) - %s\n\
v.%i.%i BUILD(%s %s)\n\
LIBS: %s\n\
";

#define PBOX_DESCRIPTION "infinite sandbox game"

// you can get general info about pixelbox using this function
const char* pbSummaryGetInfo() {
	static char* buffer[1024];
	snprintf(buffer, 1023, summary_format, PBOX_RELEASE_NAME, PBOX_DESCRIPTION, PBOX_VERSION_MAJOR, PBOX_VERSION_MINOR, __DATE__, __TIME__, "bearssl, raylib, sc_sock, miniaudio, libc");
	return buffer;
}

