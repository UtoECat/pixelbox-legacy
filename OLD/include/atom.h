/*
 * PixelBox
 * Atom definition.
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
#include <cstdint>

namespace pixelbox {
	constexpr unsigned char TYPES_COUNT = 64;
	constexpr unsigned char TYPE_MASK   = 63; // 00111111b

	struct Atom {
		uint8_t kind;
		uint8_t data;
		public:
		inline unsigned char getType(void) {
			return kind & TYPE_MASK;
		}
		inline unsigned char getKind(void) {
			return kind >> 6;
		}
	};

	static_assert(sizeof(Atom) == sizeof(uint16_t));

};
