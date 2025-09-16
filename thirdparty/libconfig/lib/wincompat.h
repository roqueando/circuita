/* ----------------------------------------------------------------------------
   libconfig - A library for processing structured configuration files
   Copyright (C) 2005-2025  Mark A Lindner

   This file is part of libconfig.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, see
   <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------------
*/

#ifndef __wincompat_h
#define __wincompat_h


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) \
  || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#define LIBCONFIG_WINDOWS_OS
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
#define LIBCONFIG_MINGW_OS
#endif


#include <limits.h>

#ifdef LIBCONFIG_WINDOWS_OS

/* Prevent warnings about redefined malloc/free in generated code. */
#ifndef _STDLIB_H
#define _STDLIB_H
#endif

#include <io.h>
#include <malloc.h>

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define posix_fileno _fileno
#define posix_write  _write

/* Might be able to replace this with:
#define posix_fsync  _commit
*/
extern int posix_fsync(int fd);

#if _MSC_VER <= 1800
#define snprintf  _snprintf
#endif

#if !defined(LIGCONFIG_MINGW_OS) && _MSC_VER < 1800
#define atoll     _atoi64
#define strtoull  _strtoui64
#define strtoll   _strtoi64
#endif

#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#endif

#if defined(LIBCONFIG_WINDOWS_OS) || defined(LIBCONFIG_MINGW_OS)

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

#define INT64_FMT "%I64d"
#define UINT64_FMT "%I64u"

#define INT64_HEX_FMT "%I64X"

#define FILE_SEPARATOR "\\"

#else /* defined(LIBCONFIG_WINDOWS_OS) || defined(LIBCONFIG_MINGW_OS) */

#define INT64_FMT "%lld"
#define UINT64_FMT "%llu"

#define INT64_HEX_FMT "%llX"

#define FILE_SEPARATOR "/"

#endif /* defined(LIBCONFIG_WINDOWS_OS) || defined(LIBCONFIG_MINGW_OS) */

#if defined(LIBCONFIG_WINDOWS_OS) && !defined(LIBCONFIG_MINGW_OS)

#define INT64_CONST(I)  (I ## i64)
#define UINT64_CONST(I) (I ## Ui64)

#ifndef INT32_MAX
#define INT32_MAX (2147483647)
#endif

#ifndef INT32_MIN
#define INT32_MIN (-2147483647-1)
#endif

#ifndef UINT32_MAX
#define UINT32_MAX (4294967295U)
#endif

#include <Shlwapi.h>
#define IS_RELATIVE_PATH(P) \
  (PathIsRelativeA(P))

#else /* defined(LIBCONFIG_WINDOWS_OS) && !defined(LIBCONFIG_MINGW_OS) */

#include <unistd.h> /* for fsync() */

#define posix_fileno fileno
#define posix_fsync  fsync
#define posix_write  write

#define INT64_CONST(I)  (I ## LL)
#define UINT64_CONST(I) (I ## ULL)

#define IS_RELATIVE_PATH(P) \
  ((P)[0] != '/')

#endif /* defined(LIBCONFIG_WINDOWS_OS) && !defined(LIBCONFIG_MINGW_OS) */

#endif /* __wincompat_h */
