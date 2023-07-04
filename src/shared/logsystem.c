/*
** This file is a part of PixelBox - infinite sandbox game
** Logging system (uses raylib logging system LMAO)
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
#include "raylib.h"
#include <stdio.h>
#include <string.h>

// size of visible log buffer in GUI's
#define LOG_BUFFER_SIZE 2048

static char buffer[LOG_BUFFER_SIZE + 1];
static size_t pos = 0;

static const char* level_name[] = {
	"[ ALL ] ",
	"[TRACE] ",
	"[DEBUG] ",
	"[INFO:] ",
	"[WARN!] ",
	"[ERROR] ",
	"[FATAL] ",
	"[ NANI]-"
};

#include <assert.h>
static void (tracelog) (int level, const char *fmt, va_list args) {
	va_list copy;

	// get length
	va_copy(copy, args);
	int llen = vsnprintf((char*)0, 0, fmt, copy);
	va_end(copy);

	// calc buffer-related stuff
	if (llen < 0) return;
	size_t len = llen;
	len += 9;
	if (pos + len >= LOG_BUFFER_SIZE) {
		size_t diff = (pos + len) - LOG_BUFFER_SIZE;
		if (diff >= LOG_BUFFER_SIZE) diff = LOG_BUFFER_SIZE-1;
		memmove(buffer, buffer + diff, LOG_BUFFER_SIZE - diff);
		if (pos >= diff) pos -= diff;
		else pos = 0;
	}

	assert(pos < LOG_BUFFER_SIZE);
	assert(fmt);

	fprintf(stdout, "%s", level_name[level]);

	// print to stdout
	va_copy(copy, args);
	vfprintf(stdout, fmt, copy);
	fprintf(stdout, "\n");
	va_end(copy);

	if (pos > 0) buffer[pos-1] = '\n'; // replace '\0' with '\n' 

	memcpy(buffer+pos, level_name[level], 8);
	pos += 8;
	llen = vsnprintf(buffer + pos, LOG_BUFFER_SIZE - pos - 1, fmt, args);
	if (llen) pos += llen;
	if (pos < LOG_BUFFER_SIZE - 1) buffer[pos++] = '\0';
}

const char* pbLatestLog(int lines) {
	int i;
	lines++;
	if (lines < 0) lines = 0;
	for (i = (int)pos; i != 0 && lines; i--) {
		if (buffer[i] == '\n') lines--;
	}
	return buffer + i + 1;
}

int pbLogGetLinesCount() {
	int i;
	int lines = 1;
	for (i = 0; i < LOG_BUFFER_SIZE && buffer[i] != '\0'; i++) {
		if (buffer[i] == '\n') lines++;
	}
	return lines;
}

void pbClearLog() {
	buffer[0] = '\0';
	pos = 0;
}

void pbLogSystemInit() {
	SetTraceLogCallback(tracelog);
	#if PBOX_DEBUG
	SetTraceLogLevel(LOG_DEBUG);
	#else
	SetTraceLogLevel(LOG_INFO);
	#endif
}
