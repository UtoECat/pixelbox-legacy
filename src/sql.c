/*
** This file is a part of PixelBox - infinite sandbox game
** Copyright (C) 2021-2023 UtoECat
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "sql.h"
#include "info.h"

enum {
	STMT_OPTIMIZE    = 0, //+ optimize database statement
	STMT_SETCHUNK    = 1, // set binary chunk data
	STMT_GETCHUNK    = 2, // get binary chunk data
	STMT_SETPROPERTY = 4, //+ set some property for GUI's
	STMT_GETPROPERTY = 5, //+ get it
	//STMT_GETACCOUNT  = 6, // get player account by ID and all data with it
	//STMT_SETACCOUNT  = 7, // set player account by ID and all data with it
	//STMT_DROP_ALL = 11,   //+ remove all pixelbox-specific data in database 
	STMT_MAX = 12 // count of statements
};

struct database {
	sqlite3 *db;
	sqlite3_stmt* statements[STMT_MAX];
};

#define SAFE_CREATE "CREATE TABLE IF NOT EXISTS "
const char* prepare_statement = 
"PRAGMA cache_size = -8000;"
"PRAGMA journal_mode = WAL;"
"PRAGMA auto_vacuum = 2;"
"PRAGMA secure_delete = 0;"
"PRAGMA temp_store = MEMORY;"
"PRAGMA page_size = 32768;"
;

static sqlite3_stmt* create_statement(pbDataBase* p, const char* sql) {
	sqlite3_stmt* ptr;
	int err	= sqlite3_prepare_v3(p->db, sql, -1, SQLITE_PREPARE_PERSISTENT,
		&ptr, (const char**)0);
	if (err != SQLITE_OK) {
		pbLog(LOG_ERROR, "SQLITE STATEMENT ERROR (%s): %s\n", sql, sqlite3_errmsg(p->db));
		return (sqlite3_stmt*)0;
	}
	return ptr;
}

#include <stdlib.h>

static int init_statements(pbDataBase* p, int mode) {
	if (!pbExecuteDataBaseQuery(p, prepare_statement)) return 0;
	sqlite3_stmt* stmt;

	// common INIT
 	if (!pbExecuteDataBaseQuery(p, 
		SAFE_CREATE "PROPERTIES (key STRING PRIMARY KEY, value);" 
		SAFE_CREATE "CRYPTO  (id INTEGER PRIMARY KEY, value STRING);"
	)) return 0;

	// specific INIT statements. Important to execute fistly
	if (mode == PBOX_SERVER_DATABASE) {
		// INIT
 		pbExecuteDataBaseQuery(p, 
			SAFE_CREATE "WCHUNKS  (id INTEGER PRIMARY KEY, value BLOB);"
			SAFE_CREATE "CLIENTS (key STRING PRIMARY KEY, data  BLOB);"
		);

	} else if (mode == PBOX_CLIENT_DATABASE) {

		// INIT
 		pbExecuteDataBaseQuery(p, 
			SAFE_CREATE "SERVERS (key STRING PRIMARY KEY, rated BOOL);"
		);
		
	}

	// IMPORTANT
	pbInitializeDataBase(p);
	pbLog(LOG_INFO, "SQLITE3 : Init statements compiled and executed!\n");

	// basic statements : common for both client and server

	// OPTIMIZE
 	stmt = create_statement(p, "PRAGMA optimize;");
	if (!stmt) return -1; // pbDataBaseDestroy() will free all statements
	p->statements[STMT_OPTIMIZE] = stmt;

	// PROPERTY GET
 	stmt = create_statement(p, 
		"SELECT value FROM PROPERTIES WHERE key = ?1;"
	);
	if (!stmt) return -1;
	p->statements[STMT_GETPROPERTY] = stmt;

	// PROPERTY SET
	stmt = create_statement(p, 
		"INSERT OR REPLACE INTO PROPERTIES VALUES(?1, ?2);"
	);
	if (!stmt) return -1;
	p->statements[STMT_SETPROPERTY] = stmt;

	if (mode == PBOX_SERVER_DATABASE) {

		// EXCLUSIVE FOR SERVER!
		// CHUNK GET
 		stmt = create_statement(p, 
			"SELECT value FROM WCHUNKS WHERE id = ?1;"
		);
		if (!stmt) return -1;
		p->statements[STMT_GETCHUNK] = stmt;
	
		// CHUNK SET
		stmt = create_statement(p, 
			"INSERT OR REPLACE INTO WCHUNKS VALUES(?1, ?2);"
		);
		if (!stmt) return -1;
		p->statements[STMT_SETCHUNK] = stmt;

	} else if (mode == PBOX_CLIENT_DATABASE) {

	} else return -2; // bad mode
	return 1; // hehe
}

pbDataBase* pbDataBaseCreate(const char* filename, int mode) {
	pbDataBase* p = (pbDataBase*)calloc(sizeof(pbDataBase), 1);
	if (!p) {
		pbLog(LOG_ERROR, "Can't allocate database!\n");
		return (pbDataBase*)0;
	}
	int err = sqlite3_open_v2(filename, &(p->db), 
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
		SQLITE_OPEN_FULLMUTEX, (char*)0);
	if (err != SQLITE_OK) {
		pbLog(LOG_ERROR, "%s", p->db?sqlite3_errmsg(p->db):"nomem");
	error_handler:
		pbDataBaseDestroy(p);
		return (pbDataBase*)0;
	}	
	if (init_statements(p, mode) <= 0) {
		pbLog(LOG_ERROR, "%s", "can't init statements for database!");
		goto error_handler;
	}
	pbLog(LOG_INFO, "database %s opened successfully!", filename);
	return p;
}

void  pbDataBaseDestroy(pbDataBase* p) {
	if (!p) return;
	for (int i = 0; i < STMT_MAX; i++) {
		if (p->statements[i]) {
			int code = sqlite3_finalize(p->statements[i]);
			if (code != SQLITE_OK) pbLog(LOG_ERROR, "%s", sqlite3_errstr(code));
			p->statements[i] = (sqlite3_stmt*)0;
		}
	}
	if (p->db) sqlite3_close_v2(p->db);
	free(p); 
}

// returns 1 if there is still data to process
// returns 0 if statement cmpleted successfully
// returns -1 if error occured
int statement_iterator(sqlite3_stmt* stmt) {
	int attemts = 0;
	retry :
	int res = sqlite3_step(stmt);
	if (res == SQLITE_BUSY) {
		attemts++;
		if (attemts < 100) goto retry;
	}
	if (res == SQLITE_DONE) {
		pbLog(LOG_TRACE, "SQLITE_DONE!");
	}
	int stat;
	if (res == SQLITE_DONE) stat = 0;
	else if (res == SQLITE_ROW) stat = 1;
	else {
		stat = -1;
		pbLog(LOG_ERROR, "SQLITE3: STATEMENT: %s", sqlite3_errstr(res));
	}
	if (stat < 1) sqlite3_reset(stmt);
	return stat;
}

// UTINILY
const char* pbGetDataBasePath(pbDataBase* p) {
	const char* c = sqlite3_db_name(p->db, 0);
	if (!c) c = "???";
	return c;
}

void  pbOptimizeDataBase(pbDataBase* p) {
	// eazy :D
	while (statement_iterator(p->statements[STMT_OPTIMIZE]) > 0) {}
}

int pbInitializeDataBase(pbDataBase* p) {

}

int pbExecuteDataBaseQuery(pbDataBase* p, const char* sql) { // UNSAFE
	char* errmsg = (char*) 0;
	if (sqlite3_exec(p->db, sql, (char*)0, (char*)0, &errmsg) != SQLITE_OK) {
		pbLog(LOG_ERROR, "Storage action error %s!\n", errmsg);
		sqlite3_free(errmsg);
		return 0;
	}
	return 1;
}

int   pbFlushDataBaseData(pbDataBase* p) {
	return sqlite3_db_cacheflush(p->db) == SQLITE_OK;
}

#include <string.h>

// GET
float pbGetDataBaseFloatProperty(pbDataBase* p, const char* prop);

PBOX_INT64 pbGetDataBaseLongProperty(pbDataBase* p, const char* prop) {
	int stat = 0;
	sqlite3_stmt* stmt = p->statements[STMT_GETPROPERTY];
	PBOX_INT64 out = -1;

	// bind key
	if (sqlite3_bind_text(stmt, 1, prop, -1, SQLITE_STATIC) != SQLITE_OK)
		return out;

	// get result
	while ((stat = statement_iterator(stmt)) > 0) {
		out = sqlite3_column_int64(stmt, 0);
	}

	// clear bindings
	sqlite3_clear_bindings(stmt);
	return out;
}

//
char* pbGetDataBaseStringProperty(pbDataBase* p, const char* prop, char* dst, PBOX_SIZE_T len) {
	int stat = 0;
	sqlite3_stmt* stmt = p->statements[STMT_GETPROPERTY];
	if (!len || !dst) return (char*) 0;

	// get result
	char* sout = (char*)0;

	// bind key
	if (sqlite3_bind_text(stmt, 1, prop, -1, SQLITE_STATIC) != SQLITE_OK)
		return sout;

	while ((stat = statement_iterator(stmt)) > 0) {
		PBOX_SIZE_T size = sqlite3_column_bytes(stmt, 0);
		sout = dst;
		if (size >= len) size = len-1;
		memcpy(dst, sqlite3_column_text(stmt, 0), size);
		dst[size] = '\0'; // since sqlite don't adds '\0'.
	}

	// clear bindings
	sqlite3_clear_bindings(stmt);
	return sout;
}

void* pbGetDataBaseChunk(pbDataBase* p, PBOX_UINT32 id, PBOX_SIZE_T size);

// SET
int pbSetDataBaseFloatProperty(pbDataBase* p, const char* prop, float v);
int pbSetDataBaseLongProperty(pbDataBase* p, const char* prop, PBOX_INT64 v) {
	sqlite3_stmt* stmt = p->statements[STMT_SETPROPERTY];
	int err = SQLITE_OK;

	// bind key
	if ((err = sqlite3_bind_text(stmt, 1, prop, -1, SQLITE_STATIC)) != SQLITE_OK) {
		return -1;
	}

	// bind value
	if ((err = sqlite3_bind_int64(stmt, 2, v)) != SQLITE_OK) {
		sqlite3_clear_bindings(stmt); // important, can produce UB in other case
		return -1;
	}
	
	pbLog(LOG_TRACE, "props[%s] = '%li'!", prop, v);

	int stat = 0;
	while ((stat = statement_iterator(stmt)) > 0) {}
	sqlite3_clear_bindings(stmt);
	return stat >= 0; // true if not error
}
int pbSetDataBaseStringProperty(pbDataBase* p, const char* prop, const char* src) {
	sqlite3_stmt* stmt = p->statements[STMT_SETPROPERTY];

	// bind key
	if (sqlite3_bind_text(stmt, 1, prop, -1, SQLITE_STATIC) != SQLITE_OK)
		return -1;
	// bind value
	if (sqlite3_bind_text(stmt, 2, src , -1, SQLITE_STATIC) != SQLITE_OK) {
		sqlite3_clear_bindings(stmt); // important, can produce UB in other case
		return -1;
	}

	pbLog(LOG_TRACE, "props[%s] = '%s'!", prop, src);

	int stat = 0;
	while ((stat = statement_iterator(stmt)) > 0) {}
	sqlite3_clear_bindings(stmt);
	return stat >= 0; // true if not error
}

int pbSetDataBaseChunk(pbDataBase* p, PBOX_UINT32 id, void* data, PBOX_SIZE_T size);
