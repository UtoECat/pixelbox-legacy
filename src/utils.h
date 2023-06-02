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
#include <cstdio>

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
			uint64_t redzone[2] = {0};
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
					if (o->redzone[0] != 0 || o->redzone[1] != 0) 
						throw "heap corruption!";
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
				if (((size_t)bump - (size_t)o) % alignof(Object) != 0) {
					fprintf(stderr, "ERR: bad poiner alligment given in BumpAllocator::free()\n");
					fprintf(stderr, "ERR: given %p, excepted %p", o, 
						(void*)(((size_t)o/alignof(Object))*alignof(Object)));
				}
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

	using size_t = std::size_t;

	using void_type = struct {};

	template <typename T>
	struct hash_manager {
		using key_type = void_type;
		static constexpr bool implemented = false;
	};

	static_assert(sizeof(uint32_t) == sizeof(chunk_position));

	template<>
	struct hash_manager<Chunk> {
		using key_type = chunk_position;
		static inline key_type getkey(Chunk& c) {
			return c.position;
		}
		static inline uint32_t hash(key_type k) {
			return dirty_cast<uint32_t, chunk_position>(k);
		}
		static constexpr bool implemented = true;
	};

	template <typename T, unsigned int HSZ = HASH_SIZE>
	class HashTable {
		public :
		using hashman = hash_manager<T>;
		static_assert(hashman::implemented == true);
		struct Node {
			Node* next = nullptr;
			T value;
			public:
			Node() = default;

			template<typename... _Args>
			Node(_Args&&... __args) : value(std::forward<_Args>(__args)...) {}

			inline uint32_t getHash() {
				return hashman::hash(hashman::getkey(value));
			}
		};
		protected :
		Bump<Node> allocator;
		Node* table[HSZ] = {nullptr};
		public:
		HashTable() = default;
		~HashTable() = default;
		HashTable(const HashTable&) = delete;
		public:
		inline Node** data() {
			return table;
		};
		inline size_t size() const {
			return HSZ;
		}
		T* rawget(typename hashman::key_type key) {
			uint32_t hh = hashman::hash(key);
			hh = hash_function(hh, HSZ);
			Node* n = table[hh];
			while (n != nullptr && hashman::getkey(n->value) != key) {
				n = n->next;
			}
			return n ? &(n->value) : nullptr;
		}
		
		template<typename... _Args>
		T* newitem(_Args&&... __args) {
			Node* newnode = allocator.create(std::forward<_Args>(__args)...);
			uint32_t hh = hashman::hash(hashman::getkey(newnode->value));
			hh = hash_function(hh, HSZ);
			// insert into list
			newnode->next = table[hh];
			table[hh]     = newnode; 
			return &(newnode->value);
		}

		Node* remove (T* c) {
			uint32_t hh = hashman::hash(hashman::getkey(*c));
			hh = hash_function(hh, HSZ);
			Node* n = table[hh];
			if (!n) throw "no chunks with the same hash in the table AT ALL!!!";
			if (&(n->value) == c) { // same chunk pointer at the beginning :p
				table[hh] = n->next;  // remove current node
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

	template <unsigned int HSZ = HASH_SIZE>
	using ChunkTable = HashTable<Chunk, HSZ>;

};
