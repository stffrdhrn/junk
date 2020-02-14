This was put together to figure out
why I was getting garbage relocations added from
.eh_frame.  In the end it was a missing macro in GCC in or1k.h (*_EH_DATA_*)

Located by test failure `elf/check-textrel` from glibc.
