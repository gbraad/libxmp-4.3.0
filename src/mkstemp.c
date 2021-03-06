#ifdef __SUNPRO_C
#pragma error_messages (off,E_EMPTY_TRANSLATION_UNIT)
#endif

#ifndef HAVE_MKSTEMP

/* Copyright (C) 1991, 1992, 1996, 1998, 2001 Free Software Foundation, Inc.
   This file is derived from mkstemps.c from the GNU Libiberty Library
   which in turn is derived from the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA. 

*/

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#ifdef _MSC_VER
#include <process.h>
#define getpid _getpid
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifdef _MSC_VER
#define open _open
#endif

/* We need to provide a type for gcc_uint64_t.  */
#ifdef __GNUC__
typedef unsigned long long gcc_uint64_t;
#else
typedef unsigned long gcc_uint64_t;
#endif

#ifndef TMP_MAX
#define TMP_MAX 16384
#endif

/* Generate a unique temporary file name from TEMPLATE.
 *
 * TEMPLATE has the form:
 *
 * <path>/ccXXXXXX
 *
 * The last six characters of TEMPLATE must be "XXXXXX"; they are
 * replaced with a string that makes the filename unique.
 *
 * Returns a file descriptor open on the file for reading and writing.
 */
int mkstemp(char *template)
{
	static const char letters[]
	    = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	static gcc_uint64_t value;
#ifdef HAVE_GETTIMEOFDAY
	struct timeval tv;
#endif
	char *XXXXXX;
	size_t len;
	int count;

	len = strlen(template);

	if ((int)len < 6 || strncmp(&template[len - 6], "XXXXXX", 6)) {
		return -1;
	}

	XXXXXX = &template[len - 6];

#ifdef HAVE_GETTIMEOFDAY
	/* Get some more or less random data.  */
	gettimeofday(&tv, NULL);
	value += ((gcc_uint64_t) tv.tv_usec << 16) ^ tv.tv_sec ^ getpid();
#else
	value += getpid();
#endif

	for (count = 0; count < TMP_MAX; ++count) {
		gcc_uint64_t v = value;
		int fd;

		/* Fill in the random bits.  */
		XXXXXX[0] = letters[v % 62];
		v /= 62;
		XXXXXX[1] = letters[v % 62];
		v /= 62;
		XXXXXX[2] = letters[v % 62];
		v /= 62;
		XXXXXX[3] = letters[v % 62];
		v /= 62;
		XXXXXX[4] = letters[v % 62];
		v /= 62;
		XXXXXX[5] = letters[v % 62];

		fd = open(template, O_RDWR | O_CREAT | O_EXCL | O_BINARY, 0600);
		if (fd >= 0)
			/* The file does not exist.  */
			return fd;

		/* This is a random value.  It is only necessary that the next
		   TMP_MAX values generated by adding 7777 to VALUE are different
		   with (module 2^32).  */
		value += 7777;
	}

	/* We return the null string if we can't find a unique file name.  */
	template[0] = '\0';
	return -1;
}

#endif /* HAVE_MKSTEMP */
