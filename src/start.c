/*
** This file is a part of PixelBox - infinite sandbox game
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

static const char* license_string = "\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
You should have received a copy of the GNU General Public License\n\
along with this program. If not, see <https://www.gnu.org/licenses/>.\n\
";

extern int pbMain();

int main(int argc, char** argv) {
	fprintf(stderr, "[LOG] : Pixelbox is starting...\n");
	if (argc > 1) {
		for(int i = 1; i < argc; i++) {
			char* v = argv[i];
			if (v[0] == '-') switch(v[1]) {
				case 'h':
					fprintf(stdout, "Pixelbox - infinite sandbox game.\n");
					fprintf(stdout, "Copyright (C) UtoECat 2021-2023\n");
					fprintf(stdout, "%s\n", license_string);
					fprintf(stdout, "Debug=%i, WordSize=%li, ByteSize=%li\n", PBOX_DEBUG,
							sizeof(PBOX_UINT64), sizeof(PBOX_UINT8));
				/* falltrough */
				case 'v': /* falltrough */
					fprintf(stdout, "Pixelbox %s v.%i.%i\n", 
							PBOX_RELEASE_NAME, PBOX_VERSION_MAJOR, PBOX_VERSION_MINOR);
					return 0;
				break;
				default:
				fprintf(stderr, "[Error] : invalid flag %c!\n", v[1]);
				break;
			}
		}
	}
	return pbMain();
}
