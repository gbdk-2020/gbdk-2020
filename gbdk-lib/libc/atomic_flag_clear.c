/*-------------------------------------------------------------------------
   atomic_flag_clear.c

   Philipp Klaus Krause, pkk@spth.de 2020

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

#include <stdatomic.h>

void atomic_flag_clear(volatile atomic_flag *object)
{
#if defined(__SDCC_z80) || defined(__SDCC_z180) || defined(__SDCC_ez80_z80) || defined(__SDCC_gbz80) || defined(__SDCC_r2k) || defined(__SDCC_r3ka) || defined(__SDCC_stm8) || defined(__SDCC_hc08) || defined(__SDCC_s08)
	object->flag = 1;
#elif defined(__SDCC_mcs51)
	object->flag = 0;
#else
#error Support for atomic_flag not implemented
#endif
}

