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

#pragma once
#include "config.h"
#include "sqlite3.h"

#define PBOX_SERVER_DATABASE 1
#define PBOX_CLIENT_DATABASE 2

typedef struct database pbDataBase;

extern pbDataBase* MainDB;

pbDataBase* pbDataBaseCreate(const char* filename, int mode);
void  pbDataBaseDestroy(pbDataBase* p);

// UTINILY
const char* pbGetDataBasePath(pbDataBase* p);
void  pbOptimizeDataBase(pbDataBase* p);
int   pbInitializeDataBase(pbDataBase* p);
int   pbExecuteDataBaseQuery(pbDataBase* p, const char* sql); // UNSAFE
int   pbFlushDataBaseData(pbDataBase* p);

// GET
float pbGetDataBaseFloatProperty(pbDataBase* p, const char* prop);
PBOX_INT64 pbGetDataBaseLongProperty(pbDataBase* p, const char* prop);
// STRING IS NOT MALLOCED ANYMORE!
char* pbGetDataBaseStringProperty(pbDataBase* p, const char* prop, char* dst, PBOX_SIZE_T len);
void* pbGetDataBaseChunk(pbDataBase* p, PBOX_UINT32 id, PBOX_SIZE_T size);

// SET
int pbSetDataBaseFloatProperty(pbDataBase* p, const char* prop, float v);
int pbSetDataBaseLongProperty(pbDataBase* p, const char* prop, PBOX_INT64 v);
int pbSetDataBaseStringProperty(pbDataBase* p, const char* prop, const char* src);
int pbSetDataBaseChunk(pbDataBase* p, PBOX_UINT32 id, void* data, PBOX_SIZE_T size);
