/*
 * PixelBox
 * Bump and hashtable
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
#include "chunk.h"
#include <new>
#include <type_traits>
#include <utility>
#include <cstddef>

namespace pixelbox {

	constexpr bool BUMP_DEBUG = true;

	class Allocator {
		
	};

	// single-typed "Bump" allocator with possibility to free individual objects
	template <typename T, unsigned int CNT = CHUNKS_HEAP_COUNT>
	class Bump : public Allocator {

		/**
		 * Internal object in the bump.
		 * Packs together T and the usage flag
		 */
		protected:
		struct Object {
			bool used = false;
			char alligment[alignof(T)-sizeof(bool)];
			char data[sizeof(T)];
		};

		protected:
		Object bump[CNT];
		unsigned int last_freed = 0;
		public:
		Bump() = default;
		Bump(const Bump&) = delete;
		~Bump() = default;

		/**
		 * Allocates memory for T, and nothing more.
		 * Memory is not guaranteed to have some value.
		 * If you want to allocate and construct your object, use create method.
		 */
		T* alloc() {
			// start from last freed, to speed up things
			for (unsigned int i = last_freed; i < CNT; i++) {
				Object *o = &(bump[i]);
				if (!o->used) {
					o->used = true;
					last_freed = i;
					return (T*)o->data;
				}
			}
			// no memory
			return nullptr;
		}

		/**
		 * allocates memory and creates object.
		 * THROWS string if allocator can't allocate more memory
		 */
		template<typename... _Args>
    T* create (_Args&&... __args) {
			void* ptr = alloc();
			if (!ptr) throw "NOMEM";
			return ::new((void*)ptr) T(std::forward<_Args>(__args)...);
		}
		
		/**
		 * deallocates memory for object T, allocated previously from this
		 * bump allocator. 
		 *
		 * This function DOES NOT run DESTRUCTOR!
		 *
		 * If you run into issues when using this allocator, it may be useful for you
		 * to set constexpr BUMP_DEBUG to true. It activates additional pointer
		 * validation checks.
		 */
		void dealloc(T* ptr) {
			char* p = (char*)ptr;
			if (p == nullptr) return;
			// offsetof() ?
			// constexpr should guarantee that this shit will not break runtime at least...
			using std::size_t;
			constexpr size_t offset = offsetof(Object, data);
			p = p - offset;
			Object* o = (Object*) p;
			if (BUMP_DEBUG) { // pointer magic here :D
				if (o < bump || o > bump + CNT) throw "pointer is out of bump";
				if (((size_t)bump - (size_t)o) % sizeof(Object) != 0) throw "bad offset";
				if (!o->used) throw "double dealloc";
			}
			o->used = false;
			if ((unsigned int)(bump - o) < last_freed) last_freed = (unsigned int)(bump - o);
		}
	
		/**
		 * This function RUNS DESTRUCTOR and frees allocated memory.
		 */
		void destroy(T* ptr) {
			if (ptr) ptr->~T();
			dealloc(ptr);
		}

	};

	// MUST BE POWER OF 2 !!! (to avoid heavy % operation)
	constexpr unsigned int HASH_SIZE = 32;
	static_assert((HASH_SIZE & (HASH_SIZE-1)) == 0);

	// this is standard XOR + MUL hash function with constant seed :p
	static inline uint32_t hash_function(uint32_t key, const uint32_t size) {
		key ^= key * 2323551956375;
		return key & (size - 1); // map it
	}

	// dirty cast. FOR INTEGERS ONLY!
	// ABSOLUTELY UNSAFE AND UNDEFINED STUFF!
	template <typename T, typename F> 
	static inline T dirty_cast(const F& orig) {
		const void* p = &orig;
		return *((const T*)p);
	}

	static inline uint32_t hashkey_cast(const chunk_position& pos) {
		if (sizeof(uint32_t) == sizeof(chunk_position)) {
			// fast one
			return dirty_cast<uint32_t, chunk_position>(pos);
		} else {// slow one
			uint32_t a = dirty_cast<uint32_t, chunk_coord>(pos.x);
			uint32_t b = dirty_cast<uint32_t, chunk_coord>(pos.y);
			return a | (b << 16);
		}
	}

	using size_t = std::size_t;

	template <unsigned int HSZ = HASH_SIZE, unsigned int CNT = CHUNKS_HEAP_COUNT>
	class ChunkTable {
		public :
		struct Node {
			Node* next = nullptr;
			Chunk value;
			public:
			Node() = default;
			Node(chunk_position pos) : value(pos) {}
			inline uint32_t getKey() {
				return hashkey_cast(value.position);
			}
		};
		protected :
		Bump<Node, CNT> allocator;
		Node* hash[HSZ] = {nullptr};
		public:
		ChunkTable() = default;
		~ChunkTable() = default;
		ChunkTable(const ChunkTable&) = delete;
		public:
		Chunk* rawget(chunk_coord x, chunk_coord y) {
			return rawget((chunk_position){x, y});
		}

		inline Node** data() {
			return hash;
		};

		inline size_t size() const {
			return HSZ;
		}

		Chunk* rawget(chunk_position pos) {
			uint32_t key = hashkey_cast(pos);
			key = hash_function(key, HSZ);
			Node* n = hash[key];
			while (n != nullptr && n->value.position != pos) {
				n = n->next;
			}
			return n ? &(n->value) : nullptr;
		}

		Chunk* newchunk(chunk_position pos) {
			Node* newnode = allocator.create(pos); // see Node::Node(chunk_position)
			uint32_t key = hashkey_cast(pos);
			key = hash_function(key, HSZ);
			Node* n = hash[key];
			if (!n) {
				hash[key] = newnode; 
			} else {
				while (n->next != nullptr) n = n->next;
				n->next = newnode;
			}
			return &(newnode->value);
		}

		void remove (Chunk* c) {
			removeExt(c);
		}

		Node* removeExt (Chunk* c) {
			chunk_position pos = c->position;	
			uint32_t key = hashkey_cast(pos);
			key = hash_function(key, HSZ);
			Node* n = hash[key];

			if (!n) throw "no chunks with the same hash in the table AT ALL!!!";
			if (&(n->value) == c) { // same chunk pointer at the beginning :p
				hash[key] = n->next;  // remove current node
			} else {
				Node* p = n;
				n = n->next;
				while (n && &(n->value) != c) {
					p = n;
					n = n->next;
				}
				if (!n) throw "chunk was not inserted into the table!";
				p->next = n->next; // remove current node
			}
			Node* ret = n->next; // next node to return
			allocator.destroy(n); // destroy and free
			return ret;
		};

	};

};
