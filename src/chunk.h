/*
 * PixelBox
 * Chunk definition.
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
#include "atom.h"

namespace pixelbox {
	using atom_coord = unsigned char;
	using chunk_coord = signed short;

	constexpr atom_coord CHUNK_WIDTH  = 32;
	constexpr atom_coord CHUNK_HEIGHT = 32;

	constexpr unsigned char KEEP_LOADED_TIMES = 50;
	constexpr unsigned int  CHUNKS_HEAP_COUNT = 1024;

	typedef struct alignas(alignof(uint32_t)) chunk_position {
		chunk_coord x;
		chunk_coord y;
		public :
		bool operator==(const chunk_position& pos) const {
			return x == pos.x && y == pos.y;
		}
		bool operator!=(const chunk_position& pos) const {
			return x != pos.x || y != pos.y;
		}
	} chunk_position;

	struct Chunk {
		bool           need_update = true; // by default 
		uint16_t       rng_state = 32722;
		unsigned char  usage_amount = KEEP_LOADED_TIMES; 
		// theese below are constructed when needed
		Atom           data[CHUNK_WIDTH * CHUNK_HEIGHT];
		chunk_position position;
		public :
		Chunk() = default;
		Chunk(chunk_position pos) : position(pos) {}
		inline void markUsage(void) {
			usage_amount = KEEP_LOADED_TIMES;
		}
		inline void markUpdate(void) {
			need_update = true;
		}
		inline uint16_t getRandom(void) {
			uint16_t b = rng_state + 1;
			rng_state ^= rng_state * 4365420;
			b = b * 303 >> 8;
			rng_state = rng_state + b;
			return rng_state;
		}
		public:
		struct index_proxy {
			Atom* ptr;
			public :
			inline index_proxy(Atom* p) : ptr(p) {}
			inline Atom& operator[](const atom_coord h) {
				//if (i >= CHUNK_HEIGHT) throw "out of bounds";
				return ptr[h * CHUNK_WIDTH];
			}
		};
		inline index_proxy operator[](const atom_coord i) {
			return index_proxy(data + i);
		}
	};

};
