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
#include <sqlite3.h>
#include <cstdio>
#include <string>

namespace pixelbox {
	
	class Storage {
		protected:
		sqlite3* db = nullptr;
		public:
		Storage(const char* name = ":memory:"); 
		~Storage();
		
		bool isOpened() {
			return db;
		}

		operator bool() {
			return db;
		}

		bool action(const char* sql) {
			if (!isOpened()) throw "cannot run actions in not opened storage!";
			char* errmsg = nullptr;
			if (sqlite3_exec(db, sql, nullptr, nullptr, &errmsg) != SQLITE_OK) {
				fprintf(stderr, "Storage action error %s!\n", errmsg);
				sqlite3_free(errmsg);
				return false;
			}
			return true;
		}

		private :
		class Statement {
			friend class Storage;
			sqlite3*        db = nullptr; // from master
			sqlite3_stmt *stmt = nullptr;
			int           stat = SQLITE_DONE;
			public:
			Statement(sqlite3* master, const char* sql) {
				db = master;
				int err = 0;
				fprintf(stderr, "SQLITE : compiling %s\n", sql);
			 	err	= sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
				if (err != SQLITE_OK) {
					fprintf(stderr, "SQLITE : %s\n", sqlite3_errmsg(master));
					throw "can't compile statement";
				}
			}
			~Statement() {
				sqlite3_finalize(stmt);
			}
			bool bind(int id, int value) {
				return sqlite3_bind_int(stmt, id, value) == SQLITE_OK;
			}
			bool bind(int id, uint32_t value) {
				return sqlite3_bind_int(stmt, id, 
						*reinterpret_cast<int32_t*>(&value)) == SQLITE_OK;
			}
			bool bind(int id, const char* value) {
				return sqlite3_bind_text(stmt, id, value, -1, SQLITE_TRANSIENT) == SQLITE_OK;
			}
			bool bind(int id, const void* value, size_t sz) {
				return sqlite3_bind_blob(stmt, id, value, sz, SQLITE_TRANSIENT) == SQLITE_OK;
			}
			int indexOf(const char* name) {
				return sqlite3_bind_parameter_index(stmt, name);
			}
			bool unbindAll() {
				return sqlite3_clear_bindings(stmt) == SQLITE_OK;
			}
			// reset, status and step
			bool step() { // step returns true each time any data is available
				if (isError()) throw "bad step call!";
				while ((stat = sqlite3_step(stmt)) == SQLITE_BUSY) {}
				if (isError() || isDone()) return false;
				return true;
			}
			int status() {
				return stat;
			}
			bool isDone() {return stat == SQLITE_DONE;}
			bool isError() {return stat != SQLITE_DONE && stat != SQLITE_ROW;}
			bool reset() {
				return sqlite3_reset(stmt) == SQLITE_OK;
			}

			// getting results (ONLY IF !isDone()!!!)

			int columnInt(int icol) {
				if (stat != SQLITE_ROW) throw "bad usage of column!";
				return sqlite3_column_int(stmt, icol);
			}
			int columnSize(int icol) {
				if (stat != SQLITE_ROW) throw "bad usage of column!";
				return sqlite3_column_bytes(stmt, icol);
			}
			const void* columnVoid(int icol) {
				if (stat != SQLITE_ROW) throw "bad usage of column!";
				return sqlite3_column_blob(stmt, icol);
			}
			const char* columnString(int icol) {
				if (stat != SQLITE_ROW) throw "bad usage of column!";
				return (const char*)sqlite3_column_text(stmt, icol);
			}
			// cumber of columns in row
			int data_count() {
				return sqlite3_data_count(stmt);
			}
		};
		protected :
		Statement* stmts[7] = {nullptr};
		enum {
			// table operations
			STMT_CREATEV = 0,
			STMT_CREATEB = 1,
			STMT_CLEARV = 2,
			STMT_DROP = 3,
			// fields operations
			STMT_SETB = 4,
			STMT_GETB = 5,
			STMT_REMOVEB = 6,
		};
		void freeInternals() {
			for (int i = 0; i < 7; i++) delete stmts[i];
		}
		void prepareInternals(); 

		public:
		bool createBinaryStorage(); 
		bool createUniformStorage(); 
		bool removeStorages(); 
		// chunks has always fixed size, but size is passed here anyway, to
		// not crash ourselves in case of DB corruption or malicious DB.
		bool setBinary(uint32_t key, const void* data, size_t size);
		bool getBinary(uint32_t key, void* data, size_t size); 
	};
};
