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
#include <filesystem>
#include "atom.h"
#include "chunk.h"
#include "utils.h"
#include "ext/gl.h" // OpenGL -> rendering
#include "ext/pcg32.h" // cool RNG generator
#include "storage.h" // brand new storage system

namespace pixelbox {

	using std::filesystem::path;

	class World {
		protected:
		chunk_position        spawn_pos;
		ChunkTable<HASH_SIZE> table;
		Storage*              storage;
		GLuint      textures[16] = {0};
		GLuint             program = 0;
		uint64_t              seed = 0;
		protected : // GC
		float old_usage = 0.1; // how many memory was used last time?
		float    target = 0.1; // when to run GC?
		int    without_gc = 0; // ticks without collecting garbage
		public:
		using hashTable = ChunkTable<HASH_SIZE>;
		int32_t     camx, camy, camw, camh;
		public :
		World() = default;
		~World(); // +
		void releaseRender(); // +
		void setStorage(Storage* sptr);
		void setSeed(uint64_t seed); // +
		/*
		 * Process and render functions.
		 * You MUST call collectGarbage after them, to collect unused
		 * chunks from static memory.
		 *
		 * It's better to start from render function (it loads visible
		 * chunks), and then call process, and, at the end, collectGarbage
		 */
		void process(); 
		void render ();

		/*
		 * Sets camera position and size.
		 * No rotation here, to not overcomplicate things.
		 * Position is specified in atoms of the whole world
		 */
		void setCamera(int32_t x, int32_t y, int32_t w, int32_t h);

		/*
		 * theese one are highlevel... in some kind :p
		 * also marks chunk any time you access it through this
		 */
		Chunk* getChunk(chunk_position pos);

		/*
		 * Collects, saves and frees all unused chunks
		 */
		void   collectGarbage(bool force = false);

		/*
		 * Unloads all chunks.
		 * This is NOT DONE IN DESTRUCTOR!!!111
		 */
		void   unloadAll();


		protected :
		bool   checkDirectory(); // +

		/*
		 * theese once do not frees/allocates chunk, but uses already
		 * allocated space.
		 */
		void   loadChunk(Chunk* c); // +
		void   generateChunk(Chunk* c);
		void   saveChunk(Chunk* c); // +
		void   processChunk(Chunk *c); // +-
		PCGRandom getWGenRNGState(Chunk* c); // yes...

		// render
		void init_shader_program();
		GLuint getIstTexture(int i); // (+) makes if not exists
		int    getMaxTextures(void); // +
		void   updateTexture(int i, Chunk *c); // + + binds it
	};
};
