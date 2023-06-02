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

	static void (sqllog) (void*, int code, const char* s) {
		if (s != nullptr) fprintf(stderr, "SQL LOG : %s\n", s);
		else if (code != SQLITE_OK)
		fprintf(stderr, "SQL ERROR CODE : %s\n", sqlite3_errstr(code));
	}

	Storage::Storage(const char* name) {
		sqlite3_config(SQLITE_CONFIG_LOG, sqllog, nullptr); 
		if (sqlite3_open_v2(name, &db, SQLITE_OPEN_READWRITE | 
				SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr) != SQLITE_OK) {
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
		action("PRAGMA synchronous = 1; PRAGMA temp_store = MEMORY;");
		action("pragma page_size = 32768;");
		action("pragma optimize;");
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

#include <thread>

namespace pixelbox {
	// Async writes?
	
	void Storage::set_working(bool v) {
		lock();
		working_flag = v;
		unlock();
	}

	bool Storage::working() {
		lock();
		bool v = working_flag;
		unlock();
		return v;
	}

	void Storage::lock() {
		wmutex.lock();
	}
	void Storage::unlock() {
		wmutex.unlock();
	}

	void Storage::processWrites(bool real) {
		lock();
		WriteNode* list = writelist, *n;
		if (!list) goto skip;
		working_flag = (list != nullptr);
		// process only limited number of tasks
		n = list;
		for (int i = 0; i < 10 && n->next; i++) {
			n = n->next;
		}
		writelist = n->next;
		n->next = nullptr; // end list just here
		skip:
		unlock();

		if (!list) return;
		if (real) fprintf(stderr, "[Writer] : list %p aqquired!\n", list); 
		
		// do at first
		n = list;
		while (n) {
			try {
				setBinary(n->id, n->value.get(), n->len);
			} catch (const char* s) {
				fprintf(stderr, "[%s] : Can't save %i!\n",
						real? "Writer" : "Main", n->id);
			}
			n = n->next;
		}
		if (real) fprintf(stderr, "[Writer] : job done! freeing list %p...\n", list); 

		// then free
		n = list;
		WriteNode* f;
		lock();
		while (n) {
			f = n;
			n = n->next;
			wallocator.destroy(f);
		}
		unsigned int used = wallocator.usage();
		working_flag = false;
		unlock();
		if (real)fprintf(stderr, "[Writer] : all done! Usage %i!\n", used); 
	}

	
	void Storage::syncAsyncThread() {
		// hard part : we can't know who will firstly accuire list of work
		// so try to accuire fistly, and then check is slave thread still
		// working, and keep waiting while job is done.
		processWrites(false);
		while(working()) {};
	}

	void Storage::writeAsync(uint32_t key, void* data, size_t size) {
		WriteNode* n;

		lock();
		retry:
		try {
			n	= wallocator.create();
		} catch (...) {
			unlock();
			syncAsyncThread(); // yeah... it not really stops thread, but
												 // finishes all tasks... :)
			lock();
			goto retry; // we must succeed
		}
		unlock();

		n->id = key;
		n->value = std::make_unique<char[]>(size);
		memcpy(n->value.get(), data, size);
		n->len = size;

		lock();
		n->next = writelist;
		writelist = n;
		unlock();
	}

};

#ifdef __linux__
#include <sys/prctl.h>
static void _SetThreadName( const char* threadName)
{
  prctl(PR_SET_NAME,threadName,0,0,0);
}
#else
#define _SetThreadName()
#endif

#include <chrono>

namespace pixelbox {

	bool Storage::asyncThreadShouldWork() {
		bool v = mthread.try_lock();
		if (v) mthread.unlock();
		return !v;
	}

	static void writer_func(Storage* s) {
		_SetThreadName("writer");
		fprintf(stderr, "[Writer] : started sucessfully!\n");
		while (s->asyncThreadShouldWork()) {
			s->processWrites(true);
			std::this_thread::sleep_for(std::chrono::nanoseconds(10000));
		}
		fprintf(stderr, "[Writer] : stopped!\n");
		fflush(stderr);
	}

	void Storage::startAsyncThread() {
		mthread.lock();
		
		std::thread writer(writer_func, this);
		writer.detach();
	}

	void Storage::stopAsyncThread() {
		mthread.unlock(); // to stop this thread.
		syncAsyncThread();
	}

};
