/*-------------------------------------------------------------------------
   malloc.c - allocate memory.

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
	header_t *next; // Next block. Linked list of all blocks, terminated by pointer to end of heap (or to the byte beyond the end of the heap).
	header_t *next_free; // Next free block. Used in free blocks only. Overlaps with user data in non-free blocks. Linked list of free blocks, 0-terminated.
};

header_t *HEAPSPACE __sdcc_heap_free; // First free block, 0 if no free blocks.

extern header_t __sdcc_heap;
#define HEAP_START &__sdcc_heap

#if defined(__SDCC_mcs51) || defined(__SDCC_ds390) || defined(__SDCC_ds400) || defined(__SDCC_hc08) || defined(__SDCC_s08)

extern const unsigned int __sdcc_heap_size;
#define HEAP_END (struct header HEAPSPACE *)((char HEAPSPACE *)&__sdcc_heap + (__sdcc_heap_size - 1)) // -1 To be sure that HEAP_END is bigger than HEAP_START.

#else

extern header_t __sdcc_heap_end; // Just beyond the end of the heap. Must be higher in memory than _sdcc_heap_start.
#define HEAP_END &__sdcc_heap_end

#endif

void __sdcc_heap_init(void)
{
	__sdcc_heap_free = HEAP_START;
	__sdcc_heap_free->next = HEAP_END;
	__sdcc_heap_free->next_free = 0;
}

#if defined(__SDCC_mcs51) || defined(__SDCC_ds390) || defined(__SDCC_ds400)
void HEAPSPACE *malloc(size_t size)
#else
void *malloc(size_t size)
#endif
{
	header_t *h;
	header_t *HEAPSPACE *f;

#if defined(__SDCC_mcs51) || defined(__SDCC_ds390) || defined(__SDCC_ds400) || defined(__SDCC_hc08) || defined(__SDCC_s08)
	if(!__sdcc_heap_free)
		__sdcc_heap_init();
#endif

	if(!size || size + offsetof(struct header, next_free) < size)
		return(0);
	size += offsetof(struct header, next_free);
	if(size < sizeof(struct header)) // Requiring a minimum size makes it easier to implement free(), and avoid memory leaks.
		size = sizeof(struct header);

	for(h = __sdcc_heap_free, f = &__sdcc_heap_free; h; f = &(h->next_free), h = h->next_free)
	{
		size_t blocksize = (char HEAPSPACE *)(h->next) - (char HEAPSPACE *)h;
		if(blocksize >= size) // Found free block of sufficient size.
		{
			if(blocksize >= size + sizeof(struct header)) // It is worth creating a new free block
			{
				header_t *const newheader = (header_t *const)((char HEAPSPACE *)h + size);
				newheader->next = h->next;
				newheader->next_free = h->next_free;
				*f = newheader;
				h->next = newheader;
			}
			else
				*f = h->next_free;

			return(&(h->next_free));
		}
	}

	return(0);
}

