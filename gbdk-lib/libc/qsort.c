/*---------------------------------------------------------------------
   qsort() - sort an array

   Copyright (C) 2018, Philipp Klaus Krause . krauseph@informatik.uni-freiburg.de

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   As a special exception, if you link this library with other files,
   some of which are compiled with SDCC, to produce an executable,
   this library does not by itself cause the resulting executable to
   be covered by the GNU General Public License. This exception does
   not however invalidate any other reasons why the executable file
   might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/

#include <stdlib.h>

// Despite the name, this is an insertion sort, since it tends to be smaller in code size.

static void swap(void *restrict dst, void *restrict src, size_t n)
{
	unsigned char *restrict d = dst;
	unsigned char *restrict s = src;

	while(n--)
	{
		unsigned char tmp = *d;
		*d = *s;
		*s = tmp;
		d++;
		s++;
	}
}

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *) __reentrant)
{
	unsigned char *b = base;

	if(nmemb <= 1)
		return;

	for(unsigned char *i = base; i < b + nmemb * size; i += size)
	{
		for(unsigned char *j = i; (j > b) && (*compar)(j, j - size) < 0; j -= size)
			swap(j, j - size, size);
	}
}

