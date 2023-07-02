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
#include "world.h"

namespace pixelbox {

	// generate individual chunk
	void World::generateChunk(Chunk* c) {
		auto rand = getWGenRNGState(c);
		for (atom_coord x = 0; x < CHUNK_WIDTH; x++) {
			for (atom_coord y = 0; y < CHUNK_HEIGHT; y++) {
				//(*c)[x][y] = (Atom){rand()%256, rand()};
				(*c)[x][y] = (Atom){0, rand()};
			}
		}
	}


};
