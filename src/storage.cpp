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

#include "storage.h"
#include <cstring>

namespace pixelbox {

	Storage::Storage(const char* name) {
		if (sqlite3_open_v2(name, &db, SQLITE_OPEN_READWRITE | 
				SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, nullptr) != SQLITE_OK) {
			fprintf(stderr, "Cannot open storage!\n");
		}
		prepareInternals();
	}

	Storage::~Storage() {
		freeInternals();
		sqlite3_close_v2(db);
	};

	void Storage::prepareInternals() {
		action("PRAGMA cache_size = -8000; PRAGMA journal_mode = WAL;");
		action("PRAGMA auto_vacuum = 2;PRAGMA secure_delete = 0");
		stmts[STMT_CREATEV] = new Statement(db, 
		"CREATE TABLE IF NOT EXISTS CONFIG (id INTEGER PRIMARY KEY, value);");
		stmts[STMT_CREATEB] = new Statement(db, 
		"CREATE TABLE IF NOT EXISTS CHUNKS (id INTEGER PRIMARY KEY, value BLOB NOT NULL);");
		stmts[STMT_CREATEV]->step();
		stmts[STMT_CREATEV]->reset();
		stmts[STMT_CREATEB]->step();
		stmts[STMT_CREATEB]->reset();

		stmts[STMT_DROP] = new Statement(db, 
			"DROP TABLE IF EXISTS CONFIG;DROP TABLE IF EXISTS CHUNKS;");
		stmts[STMT_SETB] = new Statement(db,
		"INSERT OR REPLACE INTO CHUNKS VALUES(?1 , ?2);");
		stmts[STMT_GETB] = new Statement(db, 
			"SELECT value FROM CHUNKS WHERE id = ?1");
		stmts[STMT_REMOVEB] = new Statement(db, "DELETE FROM CHUNKS WHERE id = ?1;");
		stmts[STMT_CLEARV]  = new Statement(db, "DELETE FROM CONFIG;");
	}

	bool Storage::createBinaryStorage() {
		Statement& s = *stmts[STMT_CREATEB];
		s.step();
		bool v = !s.isError() && s.isDone();
		s.reset();
		return v;
	}
		
	bool Storage::createUniformStorage() {
		Statement& s = *stmts[STMT_CREATEV];
		s.step();
		bool v = !s.isError() && s.isDone();
		s.reset();
		return v;
	}
	bool Storage::removeStorages() {
		Statement& s = *stmts[STMT_DROP];
		s.step();
		bool v = !s.isError() && s.isDone();
		s.reset();
		return v;
	}
	// chunks has always fixed size, but size is passed here anyway, to
	// not crash ourselves in case of DB corruption or malicious DB.
	bool Storage::setBinary(uint32_t key, const void* data, size_t size) {
		Statement& s = *stmts[STMT_SETB];
		s.bind(1, key);
		s.bind(2, data, size);
		s.step();
		bool v = !s.isError() && s.isDone();
		//if (!v) fprintf(stderr, "SET: %s\n", sqlite3_errmsg(db));
		s.reset();
		return v ;
	}
	bool Storage::getBinary(uint32_t key, void* data, size_t size) {
		const void* src = nullptr;
		int srcsize = 0;
		Statement& s = *stmts[STMT_GETB];

		s.bind(1, key);
		s.step();
		if (s.isDone() || s.isError()) {
			s.reset();
			return false; // nothing
		}
		srcsize = s.columnSize(0);
		src = s.columnVoid(0);
		bool v = !s.isError();
		if (src && v) 
			memcpy(data, src, srcsize < size ? srcsize : size); 
		s.step();
		/*if (s.status() != SQLITE_DONE) {
			fprintf(stderr, "GET: %s\n", sqlite3_errmsg(db));
			s.reset();
			return false;
		} */
		s.reset();
		return v && src;
	}

	

};