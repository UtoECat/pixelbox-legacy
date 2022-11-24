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
#include <audio/music.h>
#include <audio/miniaudio.h>
#include <utils/dirlist.h>

static struct dirnode *ost_list = NULL;
static struct dirnode *ost_current = NULL;

static ma_engine engine = {0};
static float volume = 1.0f;
static int   started = 0;

static char now_playing[MAX_FILENAME] = "null";
static ma_sound current = {0};

#define RES_PATH   "./res/"
#define OST_PATH   RES_PATH "ost/"
#define OST_PATH_LEN strlen(OST_PATH)
#define MENU_MUSIC RES_PATH "menu.mp3"

static void parse_ost(void) {
	ost_list = dir_read(OST_PATH, "mp3");
	if (!ost_list) {
		errorf("No OST founded! :(");
	}
}

int music_init (void) {
	ma_result result;
	if (started) return 0;
	
	parse_ost();

	result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS) {
		errorf("Can't initialize audio!");
		return 1;
	}
	started = 1;
	return 0;
}

void music_free (void) {
	if (!started) return;
	if (started == 2) {
		ma_sound_uninit(&current);
	}
	ma_engine_uninit(&engine);

	dir_free(ost_list);
	ost_list = NULL;
	ost_current = NULL;
	started = 0;
	debugf("Audio subsystem is shutdowned!");
}

static int play_it(const char* shortname, int loop) {
	if (!started) return -1;
	if (started == 2) {
		ma_sound_uninit(&current);
	}
	
	char buff[MAX_FILENAME];
	if (!shortname) {
		memcpy(buff, MENU_MUSIC, strlen(MENU_MUSIC) + 1);
	} else {
		const size_t opl = OST_PATH_LEN;
		size_t maxlen = strlen(shortname) + 1;
		if (maxlen + opl >= MAX_FILENAME - 1) maxlen = MAX_FILENAME - opl - 1;
		memcpy(buff, OST_PATH, opl);
		memcpy(buff + opl, shortname, maxlen);
		buff[MAX_FILENAME-1] = '\0';
	}
	
	ma_result res;
	const int flag = MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_STREAM;
	res = ma_sound_init_from_file(&engine, buff, flag, NULL, NULL, &current);

	if (res != MA_SUCCESS) {
		errorf("Can't play file %s!", buff);
		started = 1;
		return -1;
	};

	memcpy(now_playing, buff, MAX_FILENAME);	
	debugf("Now playing : %s", buff);
	ma_sound_set_fade_in_milliseconds(&current, 0, 1, 1000);
	ma_sound_set_looping(&current, loop);
	ma_sound_start(&current);
	started = 2;
	return 0;
}

void music_tick(void) {
	if (started != 2) return;
	if (ma_sound_at_end(&current)) {
		if (!ost_list) return;
		if (!ost_current || !ost_current->next) ost_current = ost_list;
		else ost_current = ost_current->next;
		play_it(ost_current->filename, 0);
	}
}

void (music_menu) (void) {
	play_it(NULL, 1);
}

void (music_ost) (void) {
	if (!ost_list) return;
	if (!ost_current || !ost_current->next) ost_current = ost_list;
	else ost_current = ost_current->next;
	play_it(ost_current->filename, 0);
}
const char* (music_now) (void) {
	return now_playing;
}

void (music_volume)      (float v) {
	if (!started) return;
	volume = v;
	if (volume > 1.0) volume = 1.0;
	if (volume < 0.0) volume = 0.0;
	ma_engine_set_volume(&engine, volume);
}

float (music_get_volume) (void) {
	return volume;
}
