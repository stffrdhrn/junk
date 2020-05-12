/* Copyright (C) 2003-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2003.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/* Check alignment, overlapping and layout of TLS variables.  */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "tst-tls5.h"

struct tls_obj tls_registry[64];

static int
tls_addr_cmp (const void *a, const void *b)
{
  if (((struct tls_obj *)a)->addr < ((struct tls_obj *)b)->addr)
    return -1;
  if (((struct tls_obj *)a)->addr > ((struct tls_obj *)b)->addr)
    return 1;
  return 0;
}

int main (int argc, char ** argv)
{
  size_t cnt, i;
  int res = 0;

  for (cnt = 0; tls_registry[cnt].name; ++cnt);
  tls_registry[cnt].name = NULL;
  tls_registry[cnt].addr = (uintptr_t) pthread_self ();
  tls_registry[cnt].size = 4;
  tls_registry[cnt++].align = 4;

  qsort (tls_registry, cnt, sizeof (struct tls_obj), tls_addr_cmp);

  for (i = 0; i < cnt; ++i)
    {
      char * bytes;

      printf ("%s%s = %p, size %zd, align %zd",
	      tls_registry[i].name ? "&" : "",
	      tls_registry[i].name ?: "pthread_self ()",
	      (void *) tls_registry[i].addr,
	      tls_registry[i].size, tls_registry[i].align);
      if (tls_registry[i].addr & (tls_registry[i].align - 1))
	{
	  fputs (", WRONG ALIGNMENT", stdout);
	  res = 1;
	}
      if (i > 0
	  && (tls_registry[i - 1].addr + tls_registry[i - 1].size
	      > tls_registry[i].addr))
	{
	  fputs (", ADDRESS OVERLAP", stdout);
	  res = 1;
	}
      fputs("\n", stdout);
      bytes = (char *) tls_registry[i].addr;
      for (int c = 0; c < tls_registry[i].size; c++) {
         printf ("%p: %x\n", (void *) &bytes[c], bytes[c]);
      }

    }

  return res;
}
