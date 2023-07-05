/* remove all deprecated SQLITE3 code */
#define SQLITE_OMIT_DEPRECATED 1

/* by security reasons */
#define define SQLITE_OMIT_LOAD_EXTENSION 1

/* we never need that shit */
#define SQLITE_OMIT_AUTHORIZATION 1

/* security */
#define SQLITE_OMIT_COMPILEOPTION_DIAGS 1
