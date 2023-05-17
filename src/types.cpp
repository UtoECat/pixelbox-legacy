/*
 * PixelBox
 * World processing implementation
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

#include "world.h"
#include <string.h>
#include "types.h"

namespace pixelbox {

	static void nothing (const processContext* ctx) {}

	static AtomType types[TYPES_COUNT] = {
		{"", nothing}
	};

	void   World::processChunk(Chunk *chunk) {
		processContext tmp = {this, chunk, 0, 0};
		for (; tmp.x < CHUNK_WIDTH; tmp.x++) {
			for (; tmp.y < CHUNK_HEIGHT; tmp.y++) {
				uint8_t type = (*chunk)[tmp.x][tmp.y].getType();
				types[type].proc(&tmp);
			}
		}
	}
};
