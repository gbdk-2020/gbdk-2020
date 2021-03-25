/*-------------------------------------------------------------------------
   free.c - deallocate memory.

   Copyright (C) 2015, Philipp Klaus Krause, pkk@spth.de

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

void free(void *ptr)
{
	header_t *h, *next_free, *prev_free;
	header_t *HEAPSPACE *f;

	if(!ptr)
		return;

	prev_free = 0;
	for(h = __sdcc_heap_free, f = &__sdcc_heap_free; h && h < ptr; prev_free = h, f = &(h->next_free), h = h->next_free); // Find adjacent blocks in free list
	next_free = h;

	h = (void HEAPSPACE *)((char HEAPSPACE *)(ptr) - offsetof(struct header, next_free));

	// Insert into free list.
	h->next_free = next_free;
	*f = h;

	if(next_free == h->next) // Merge with next block
	{
		h->next_free = h->next->next_free;
		h->next = h->next->next;
	}

	if (prev_free && prev_free->next == h) // Merge with previous block
	{
		prev_free->next = h->next;
		prev_free->next_free = h->next_free;
	}
}

