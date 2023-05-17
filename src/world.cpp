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
#include <string.h>

namespace pixelbox {

	bool World::setDirectory(std::string path) {
		dir = path;
		return checkDirectory();
	}

	bool World::setDirectory(const char* path) {
		if (path) {
			dir = path;
		} else dir.clear();
		return checkDirectory();
	}

	// clever... very clever...
	namespace fsys {
		using namespace std::filesystem;
	};

	bool World::checkDirectory() {
		if (dir.empty()) return false;
		if (!fsys::is_directory(dir)) { // bad directory
			fprintf(stderr, "%s is a bad directory!\n", dir.c_str());
			dir.clear();
			return false;		
		};
		return true;
	}

	World::~World() {}

	// render 

	void World::releaseRender() {
		for (int i = 0; i < 16; i++) if (textures[i]) {
			glDeleteTextures(1, textures + i);
			textures[i] = 0;
		}
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

	// load/save chunks

	#include <string>
	#include <stdio.h>

	static void chunkgen(Chunk* c) {
		for (atom_coord x = 0; x < CHUNK_WIDTH; x++) {
			for (atom_coord y = 0; y < CHUNK_HEIGHT; y++) {
				(*c)[x][y] = (Atom){0, 0};
			}
		}
	};

	static const char* signature_string = "\033PBC";
	static const uint32_t SIGNATURE = *((const int*)(signature_string));

	static const char* getfilename(path& dir, uint32_t id) {
		static path tmp;
		char fname[64];	
		snprintf(fname, 63, "%i.dat", id);
		tmp = dir;
		tmp /= fname;
		return tmp.c_str();
	}

	void World::loadChunk(Chunk* c) {
		uint32_t key = hashkey_cast(c->position);
		if (!dir.empty()) { // try load from file
			const char* fname = getfilename(dir, key);
			FILE* f = fopen(fname, "rb");
			int cnt = 0;
			if (f) {
				// check file signature
				uint32_t sig = 0;
				if (fread(&sig, sizeof(uint32_t), 1, f) <= 0 || sig != SIGNATURE) {
					perror("Bad file signature : invalid, corrupted file, or different endianess");
					goto loaderr;
				}
				
				// read data
				cnt = fread(c->data, sizeof(Atom), CHUNK_WIDTH * CHUNK_HEIGHT, f);
				if (cnt != CHUNK_WIDTH * CHUNK_HEIGHT) {
					perror("Chunk file has bad length!");
					goto loaderr;
				}
				fclose(f);
				return; // success
				loaderr:
				fclose(f);
			}
			// can't load, generate chunk instead
		}
		chunkgen(c);
	};

	void  World::saveChunk(Chunk* c) {
		uint32_t key = hashkey_cast(c->position);
		if (!dir.empty()) { // try load from file
			const char* fname = getfilename(dir, key);
			FILE* f = fopen(fname, "wb");
			int cnt = 0;
			if (f) {
				// write file signature
				if (fwrite(&SIGNATURE, sizeof(uint32_t), 1, f) <= 0) {
					perror("Can;t write to the file");
					goto loaderr;
				}
				
				// write data
				cnt = fwrite(c->data, sizeof(Atom), CHUNK_WIDTH * CHUNK_HEIGHT, f);
				if (cnt != CHUNK_WIDTH * CHUNK_HEIGHT) {
					perror("Can't write chunk to the file!");
					goto loaderr;
				}
				fflush(f);
				fclose(f);
				return; // success
				loaderr:
				fclose(f);
				remove(fname);
			} else {
				perror("Can't open file to save chunk!");
				fprintf(stderr, "filename : %s\n", fname);				
			};
		};
		// can't save, do nothing
	};

	// highlevel
	Chunk* World::getChunk(chunk_position pos) {
		Chunk* c = table.rawget(pos);
		if (!c) {
			c = table.newchunk(pos);
			c->markUpdate(); // update when firstly loaded
			loadChunk(c);
		}	
		c->markUsage();
		return c;
	}

	void   World::collectGarbage(void) {
		// raw acess to the hash table
		World::hashTable::Node** data = table.data();
		World::hashTable::Node* n = nullptr;

		for (size_t i = 0; i < table.size(); i++) {
			n = data[i];
			while (n) {
				n->value.usage_amount--;
				if (!n->value.usage_amount) {
					// collect
					saveChunk(&(n->value));
					n = table.removeExt(&(n->value));
				} else {
					n = n->next;
				}
			}
		};
	}


};
