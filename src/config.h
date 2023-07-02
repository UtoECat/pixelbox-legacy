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

#define PBOX_VERSION_MAJOR 0
#define PBOX_VERSION_MINOR 5

#define PBOX_RELEASE_NAME "amogus de amoguso"

// type definitions and predefinitions

#define PBOX_PTR    void*
#define PBOX_INT8   signed char
#define PBOX_INT16  short int
#define PBOX_INT32  int
#define PBOX_INT64  long int
#define PBOX_UINT8  unsigned char
#define PBOX_UINT16 unsigned short int
#define PBOX_UINT32 unsigned int
#define PBOX_UINT64 unsigned long int
#define PBOX_SIZE_T PBOX_UINT64

// null pointer
#define PBOX_NULL   ((PBOX_PTR)0)

// C cast, wrapped to more readable function, that gives exact meaning what is going on here
#define PBOX_CAST(T, V) ((T)V)

// for shitty compilers
#ifndef offsetof
#define offsetof(S, F) ( \
	(PBOX_SIZE_T) (PBOX_CAST(PBOX_PTR*, &(PBOX_CAST(S*, 0)->F))) \
)
#endif

// for shitty build enviroment
#ifndef PBOX_DEBUG
#warning PBOX_DEBUG was not defined!
#define PBOX_DEBUG 1
#endif

/*
** Naming practice :
** xNameName - Virtual Method names
** NameNameCreate - "Constructors"
** NameNameDestroy - "Destructors"
** fieldName - Fields of structures
** ENUM_FILED - Fields in enumerations
** PBOX_MACRO_NAME - Macro and Define's names
*  [action][Type][Value Name] - Function names. Parts are optional. (getPixelColor)
*/
