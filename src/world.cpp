/*
 * PixelBox
 * World implementation
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
#include <string>
#include <cstdio>
#include "worldgen.h"
#include <cstring>
#include <math.h>

namespace pixelbox {

	// clever... very clever...
	namespace fsys {
		using namespace std::filesystem;
	};

	void World::setSeed(uint64_t s) {
		seed = s;
	}

	PCGRandom World::getWGenRNGState(Chunk* c) {
		uint32_t hh = hash_manager<Chunk>::hash(c->position);
		return PCGRandom(seed + hh); // easy
	}

	// render 

	void World::releaseRender() {
		for (int i = 0; i < 16; i++) if (textures[i]) {
			glDeleteTextures(1, textures + i);
			textures[i] = 0;
		}
		glUseProgram(0); // else thoubles can happen
		glDeleteProgram(program);
		program = 0;
	}

	static void defaultTextureParams(void) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// no filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	int    World::getMaxTextures(void) {
		return 16;
	}

	GLuint World::getIstTexture(int i) {
		if (i < 0 || i >= 16) throw "bad texture slot";
		if (textures[i]) return textures[i];
		glGenTextures(1, textures + i);
		GLuint tex = textures[i];
		if (!tex) throw "can't create texture";
		glBindTexture(GL_TEXTURE_2D, tex);
		defaultTextureParams();
		// pregenerate buffer on GPU
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, CHUNK_WIDTH, CHUNK_HEIGHT,
			0, GL_RG, GL_UNSIGNED_BYTE, nullptr);
		return textures[i];
	}

	void  World::updateTexture(int i, Chunk *c) {
		GLuint tex = getIstTexture(i);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CHUNK_WIDTH, CHUNK_HEIGHT,
			GL_RG, GL_UNSIGNED_BYTE, c->data);
	}

};

namespace pixelbox {

	void World::setStorage(Storage* sptr) {
		storage = sptr;
		if (storage) {
			storage->createBinaryStorage();
		}
	}

	void World::loadChunk(Chunk* c) {
		uint32_t key = hash_manager<Chunk>::hash(c->position);
		if (storage) try {
			if (storage->getBinary(key, c->data,
					CHUNK_WIDTH * CHUNK_HEIGHT * sizeof(Atom))) return;
		} catch (const char* s) {
			fprintf(stderr, "%s\n", s);
		}
		generateChunk(c);
	};
	
	void  World::saveChunk(Chunk* c) {
		uint32_t key = hash_manager<Chunk>::hash(c->position);
		if (storage) try {
			/*if (storage->setBinary(key, c->data,
					CHUNK_WIDTH * CHUNK_HEIGHT * sizeof(Atom))) return;
			*/
			storage->writeAsync(key, c->data, CHUNK_WIDTH *
				CHUNK_HEIGHT * sizeof(Atom));
		} catch (const char* s) {
			fprintf(stderr, "%s\n", s);
		}	
		// can't save, do nothing
	};

	// highlevel
	Chunk* World::getChunk(chunk_position pos) {
		Chunk* c = table.rawget(pos);
		if (!c) {
			c = table.newitem(pos);
			c->markUpdate(); // update when firstly loaded
			loadChunk(c);
		}	
		c->markUsage();
		return c;
	}

	void   World::collectGarbage(bool force) {
		// raw acess to the hash table
		World::hashTable::Node** data = table.data();
		World::hashTable::Node* n = nullptr;
		without_gc++; // inrease anyway

		float usage = table.usage();

		constexpr float hard_minimum = 0.2; // minimum to run
		static_assert(hard_minimum > 0.1 && hard_minimum < 0.9);
		static_assert(KEEP_LOADED_TIMES > 2); // needed for modern GC

		if (force) goto skipcheck;

		// don't run too often :), it has no sense
		if ((usage < target && usage < 0.5 && without_gc < 200) ||
			usage < hard_minimum) return; 
		skipcheck:

		// save new old usage
		float old = old_usage;
		old_usage = usage;

		float olddiff = usage - old;

		// set decrease level by encounting how much heap is used.
		int decrease;
		{
		float k = usage;
		k -= hard_minimum;
		k *= 1 + 1 * hard_minimum * 2;
		if (olddiff > 0) k += olddiff / 3; 
		if (k < 0) k = 0; else if (k > 1) k = 1; // failchecks
		decrease = floor(k * (KEEP_LOADED_TIMES-1)) + 1;
		}

		// collect
		for (size_t i = 0; i < table.size(); i++) {
			n = data[i];
			while (n) {
				int tmp = n->value.usage_amount;
				tmp -= decrease;
				if (tmp < 0) tmp = 0;
				n->value.usage_amount = tmp;
				if (!tmp) {
					// collect
					saveChunk(&(n->value));
					n = table.remove(&(n->value));
				} else {
					n = n->next;
				}
			}
		};

		// calculate differenses
		float before = usage;
		usage = table.usage();

		// increase/decrease target
		float diff = usage - before;
		float tk = (target/15.0);
		const char* stat = "nul";

		if ((olddiff <= -0.001 && target > usage + tk*2) && without_gc > 100) {
			// decrease target
			target -= olddiff*0.5 + tk;
			if (target <= usage) target = usage + tk;
			if (target < hard_minimum) target = hard_minimum;
			stat = "dec";
		} // else increase
		else if ((olddiff > 0.001 && usage > target)) {
			if (target < usage) target = usage;
			target += fabs(olddiff) * 2;
			if (target > 0.9) target = 0.9;
			stat = "inc";
		} else {
			stat = "nul";
			// nothing
		}
		fprintf(stderr,  "[GCLOG] : oldUsage=%f, newUsage=%f, "
				"target=%f (%s)! timeout=%i, --%i!\n",old, usage, target,
				stat, without_gc, decrease);
		without_gc = 0; // ok
	}

	void World::unloadAll() {
		World::hashTable::Node** data = table.data();
		World::hashTable::Node* n = nullptr;
		// collect
		for (size_t i = 0; i < table.size(); i++) {
			n = data[i];
			while (n) {
				n->value.usage_amount = 0;
				saveChunk(&(n->value));
				n = table.remove(&(n->value));
			}
		}
		if (table.usage() != 0.0) throw "can't collect all chunks!";
	}

	World::~World() {}

	void World::setCamera(int32_t x, int32_t y, int32_t w, int32_t h) {
		camx = x;
		camy = y;
		camw = w;
		camh = h;
	}

	void World::process() {
		// raw acess to the hash table
		World::hashTable::Node** data = table.data();
		World::hashTable::Node* n = nullptr;

		for (size_t i = 0; i < table.size(); i++) {
			n = data[i];
			while (n) {
				Chunk* c = &(n->value);
				if (c->need_update) {
					c->need_update = false;
					processChunk(c);
				}
				n = n->next;
			}
		}
	}

	static inline chunk_coord C2W_cast(int32_t pos, chunk_coord scale) {
		return (chunk_coord)((pos / (int32_t)scale));
	}

	void World::render() {
		// use program
		init_shader_program();

		// calcualte chunks to draw
		chunk_position from = {C2W_cast(camx-CHUNK_WIDTH, CHUNK_WIDTH),
			C2W_cast(camy-CHUNK_WIDTH, CHUNK_HEIGHT)};
		chunk_position to   = {C2W_cast(camx+camw, CHUNK_WIDTH),
			C2W_cast(camy+camh, CHUNK_HEIGHT)};

		// used later in loop
		float halfw = camw/2.0;
		float halfh = camh/2.0;

		// error checks
		if (from.x > to.x || from.y > to.y) throw "bad camera width";
		int texID = 0;

		for (chunk_coord x = from.x; x <= to.x; x++) {
			for (chunk_coord y = from.y; y <= to.y; y++) {
				Chunk* c = getChunk(chunk_position{x, y});
				// make quad coords for OpenGL
				float ax = (x*CHUNK_WIDTH - camx)/(float)halfw;
				ax -= 1;
				float ay = (y*CHUNK_HEIGHT - camy)/(float)halfh;
				ay -= 1;
				float bx = ax + CHUNK_WIDTH/(float)halfw;
				float by = ay + CHUNK_HEIGHT/(float)halfh;
				// refill texture
				if (texID >= getMaxTextures() - 1) {
					texID = 0;
					//glFlush(); // we will reuse textures now
				}
				updateTexture(texID, c);
				texID++;

				// draw texture
				glBegin(GL_TRIANGLE_FAN);
					glVertexAttrib2f(1,    0,    0);
					glVertexAttrib2f(0,   ax,   ay);
					glVertexAttrib2f(1,    0,    1);
					glVertexAttrib2f(0,   ax,   by);
					glVertexAttrib2f(1,    1,    1);
					glVertexAttrib2f(0,   bx,   by);
					glVertexAttrib2f(1,    1,    0);
					glVertexAttrib2f(0,   bx,   ay);
				glEnd();
			}
		}
	}

};
