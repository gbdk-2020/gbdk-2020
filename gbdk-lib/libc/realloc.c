/*-------------------------------------------------------------------------
   realloc.c - allocate memory.
   
   Always behaves according to C90 (i.e. does not take advantage of
   undefined behaviour introduced in C2X or implementation-defined
   behaviour introduced in C17.

   Copyright (C) 2015-2020, Philipp Klaus Krause, pkk@spth.de

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
#include <stddef.h>
#include <string.h>

#if defined(__SDCC_mcs51) || defined(__SDCC_ds390) || defined(__SDCC_ds400)
#define HEAPSPACE __xdata
#elif defined(__SDCC_pdk13) || defined(__SDCC_pdk14) || defined(__SDCC_pdk15)
#define HEAPSPACE __near
#else
#define HEAPSPACE
#endif

typedef struct header HEAPSPACE header_t;

struct header
{
	header_t *next;
	header_t *next_free;
};

extern header_t *HEAPSPACE __sdcc_heap_free;

void __sdcc_heap_init(void);

#if defined(__SDCC_mcs51) || defined(__SDCC_ds390) || defined(__SDCC_ds400)
void HEAPSPACE *realloc(void *ptr, size_t size)
#else
void *realloc(void *ptr, size_t size)
#endif
{
	void HEAPSPACE *ret;
	header_t *h, *next_free, *prev_free;
	header_t *HEAPSPACE *f, *HEAPSPACE *pf;
	size_t blocksize, oldblocksize, maxblocksize;

#if defined(__SDCC_mcs51) || defined(__SDCC_ds390) || defined(__SDCC_ds400) || defined(__SDCC_hc08) || defined(__SDCC_s08)
	if(!__sdcc_heap_free)
		__sdcc_heap_init();
#endif

	if(!ptr)
		return(malloc(size));

	if(!size)
	{
		free(ptr);
		return(0);
	}

	prev_free = 0, pf = 0;
	for(h = __sdcc_heap_free, f = &__sdcc_heap_free; h && h < ptr; prev_free = h, pf = f, f = &(h->next_free), h = h->next_free); // Find adjacent blocks in free list
	next_free = h;

	if(size + offsetof(struct header, next_free) < size) // Handle overflow
		return(0);
	blocksize = size + offsetof(struct header, next_free);
	if(blocksize < sizeof(struct header)) // Requiring a minimum size makes it easier to implement free(), and avoid memory leaks.
		blocksize = sizeof(struct header);

	h = (void HEAPSPACE *)((char HEAPSPACE *)(ptr) - offsetof(struct header, next_free));
	oldblocksize = (char HEAPSPACE *)(h->next) - (char HEAPSPACE *)h;

	maxblocksize = oldblocksize;
	if(prev_free && prev_free->next == h) // Can merge with previous block
		maxblocksize += (char HEAPSPACE *)h - (char HEAPSPACE *)prev_free;
	if(next_free == h->next) // Can merge with next block
		maxblocksize += (char HEAPSPACE *)(next_free->next) - (char HEAPSPACE *)next_free;

	if(blocksize <= maxblocksize) // Can resize in place.
	{
		if(prev_free && prev_free->next == h) // Always move into previous block to defragment
		{
			memmove(prev_free, h, blocksize <= oldblocksize ? blocksize : oldblocksize);
			h = prev_free;
			*pf = next_free;
			f = pf;
		}

		if(next_free && next_free == h->next) // Merge with following block
		{
			h->next = next_free->next;
			*f = next_free->next_free;
		}

		if(maxblocksize >= blocksize + sizeof(struct header)) // Create new block from free space
		{
			header_t *const newheader = (header_t *const)((char HEAPSPACE *)h + blocksize);
			newheader->next = h->next;
			newheader->next_free = *f;
			*f = newheader;
			h->next = newheader;
		}

		return(&(h->next_free));
	}

	if(ret = malloc(size))
	{
		size_t oldsize = oldblocksize - offsetof(struct header, next_free);
		memcpy(ret, ptr, size <= oldsize ? size : oldsize);
		free(ptr);
		return(ret);
	}

	return(0);
}

