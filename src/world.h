/*
 * PixelBox
 * World definition
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
#include "chunk.h"
#include "utils.h"
#include "ext/gl.h"
#include <filesystem>

namespace pixelbox {

	using std::filesystem::path;

	class World {
		chunk_position        spawn_pos;
		ChunkTable<HASH_SIZE> table;
		path                    dir;
		GLuint      textures[16] = {0};
		public:
		using hashTable = ChunkTable<HASH_SIZE>;
		int32_t     camx, camy, camw, camh;
		public :
		World() = default;
		~World(); // +
		void releaseRender(); // +
		bool setDirectory(std::string path); // +
		bool setDirectory(const char* path); // +
		void process();
		void render();

		/*
		 * theese one are highlevel... in some kind :p
		 * also marks chunk any time you access it through this
		 */
		Chunk* getChunk(chunk_position pos);

		/*
		 * Collects, saves and frees all unused chunks
		 */
		void   collectGarbage(void);

		protected :
		bool   checkDirectory(); // +

		/*
		 * theese once do not frees/allocates chunk, but uses already
		 * allocated space.
		 */
		void   loadChunk(Chunk* c); // +
		void   saveChunk(Chunk* c); // +
		void   processChunk(Chunk *c); // +-


		// render
		GLuint getIstTexture(int i); // (+) makes if not exists
		int    getMaxTextures(void); // +
		void   updateTexture(int i, Chunk *c); // + + binds it
	};
};
