#include <stdio.h>

int bits = 32;
unsigned long value = ~0xff;

typedef unsigned long bfd_vma;
typedef long bfd_signed_vma;

int main() {

      if (bits < sizeof (bfd_vma) * 8)
	{
	  bfd_signed_vma lim = (bfd_vma)1 << bits;
	  bfd_signed_vma svalue = value;

	  printf ("lim:%lx(%ld) svalue:%lx(%ld)\n", lim, lim, svalue, svalue);

	  if (svalue < -lim || svalue >= lim)
	    printf ("Signed overflow!\n");
	}
      if (bits < sizeof (bfd_vma) * 8)
	{
	  bfd_vma lim = (bfd_vma)1 << bits;
	  printf ("lim:%lx(%ld) value:%lx(%lu)\n", lim, lim, value, value);
	  if (value >= lim)
	    printf ("Unsigned overflow!\n");
	}
}
