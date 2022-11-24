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
#include <dirlist.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <main.h>

struct dirnode* dir_read(const char* dir, char* type) {
	DIR *f; struct dirent *d;
	f = opendir(dir);
	if (!f) {
		errorf("Can't open %s directory!", dir);
		return NULL;
	}	
	struct dirnode* list = NULL;
	while ((d = readdir(f)) != NULL) {
		if (d->d_type != DT_REG) continue;
		if (strstr(d->d_name, type) != NULL) {
			struct dirnode* n = malloc(sizeof(struct dirnode) + strlen(d->d_name));
			if (!n) {crash("allocation error");}
			n->next = list;
			list = n;
			memcpy(n->filename, d->d_name, strlen(d->d_name) + 1);
			debugf("founded file %s!", d->d_name);
		}
	}
	closedir(f);
	return list;
}

void dir_free(struct dirnode* d) {
	if (!d) return;
	struct dirnode* f = d;
	while (f) {
		d = d->next;
		free(f);
		f = d;
	}
}

