## OpenRISC PLT

Investigation into why the plt relocation is not working in glibc.

 - It does work with LD_BIND_NOW=1
 - It does work with musl

Questions

 - What is the first entry in the PLT for?
 - What is rela plt?

### Dump of the PLT

```
$ or1k-smh-linux-gnu-objdump -j .plt -d ./hello

./hello:     file format elf32-or1k


Disassembly of section .plt:

000022a0 <.plt>:
    22a0:	19 80 00 00 	l.movhi r12,0x0
    22a4:	85 ec 60 10 	l.lwz r15,24592(r12) // 6010 -> 0x0 in got
    22a8:	44 00 78 00 	l.jr r15
    22ac:	85 8c 60 0c 	l.lwz r12,24588(r12) // 600c -> 0x0 in got
    22b0:	19 80 00 00 	l.movhi r12,0x0              <- got[3]
    22b4:	85 8c 60 14 	l.lwz r12,24596(r12) // 6014
    22b8:	44 00 60 00 	l.jr r12
    22bc:	a9 60 00 00 	l.ori r11,r0,0x0
    22c0:	19 80 00 00 	l.movhi r12,0x0              <- got[4]
    22c4:	85 8c 60 18 	l.lwz r12,24600(r12) // 6018
    22c8:	44 00 60 00 	l.jr r12
    22cc:	a9 60 00 0c 	l.ori r11,r0,0xc
    22d0:	19 80 00 00 	l.movhi r12,0x0              <- got[5]
    22d4:	85 8c 60 1c 	l.lwz r12,24604(r12) // 601c
    22d8:	44 00 60 00 	l.jr r12
    22dc:	a9 60 00 18 	l.ori r11,r0,0x18
```

### Dump of the GOT

Contains PLT entries only pretty much, not `.plt.got` or `.got.plt` in OpenRISC.

```
$ or1k-smh-linux-gnu-readelf -x .got ./hello

Hex dump of section '.got':
 NOTE: This section has relocations against it, but these have NOT been applied to this dump.
  0x00006008 00005f10 00000000 00000000 000022b0 .._...........".
  0x00006018 000022c0 000022d0 00000000          .."...".....
```

### Dump of all Headers

Show where things are.

```
$ or1k-smh-linux-gnu-readelf -e ./hello
ELF Header:
  Magic:   7f 45 4c 46 01 02 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  Data:                              2's complement, big endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           OpenRISC 1000
  Version:                           0x1
  Entry point address:               0x22e0
  Start of program headers:          52 (bytes into file)
  Start of section headers:          12244 (bytes into file)
  Flags:                             0x0
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         7
  Size of section headers:           40 (bytes)
  Number of section headers:         34
  Section header string table index: 33

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .hash             HASH            00002114 000114 000028 04   A  3   0  4
  [ 2] .gnu.hash         GNU_HASH        0000213c 00013c 00002c 04   A  3   0  4
  [ 3] .dynsym           DYNSYM          00002168 000168 000050 10   A  4   1  4
  [ 4] .dynstr           STRTAB          000021b8 0001b8 00004a 00   A  0   0  1
  [ 5] .gnu.version      VERSYM          00002202 000202 00000a 02   A  3   0  2
  [ 6] .gnu.version_r    VERNEED         0000220c 00020c 000020 00   A  4   1  4
  [ 7] .rela.dyn         RELA            0000222c 00022c 00000c 0c   A  3   0  4
  [ 8] .rela.plt         RELA            00002238 000238 000024 0c  AI  3  21  4
  [ 9] .init             PROGBITS        0000225c 00025c 000044 00  AX  0   0  1
  [10] .plt              PROGBITS        000022a0 0002a0 000040 04  AX  0   0  4
  [11] .text             PROGBITS        000022e0 0002e0 00023c 00  AX  0   0  4
  [12] .fini             PROGBITS        0000251c 00051c 000014 00  AX  0   0  1
  [13] .rodata           PROGBITS        00002530 000530 00000a 00   A  0   0  4
  [14] .interp           PROGBITS        0000253a 00053a 000018 00   A  0   0  1
  [15] .eh_frame         PROGBITS        00002554 000554 000004 00   A  0   0  4
  [16] .note.ABI-tag     NOTE            00002558 000558 000020 00   A  0   0  4
  [17] .init_array       INIT_ARRAY      00005f08 001f08 000004 04  WA  0   0  4
  [18] .fini_array       FINI_ARRAY      00005f0c 001f0c 000004 04  WA  0   0  4
  [19] .dynamic          DYNAMIC         00005f10 001f10 0000f0 08  WA  4   0  4
  [20] .data             PROGBITS        00006000 002000 000008 00  WA  0   0  4
  [21] .got              PROGBITS        00006008 002008 00001c 04  WA  0   0  4
  [22] .bss              NOBITS          00006024 002024 000004 00  WA  0   0  1
  [23] .comment          PROGBITS        00000000 002024 00002a 01  MS  0   0  1
  [24] .debug_aranges    PROGBITS        00000000 00204e 000038 00      0   0  1
  [25] .debug_info       PROGBITS        00000000 002086 000192 00      0   0  1
  [26] .debug_abbrev     PROGBITS        00000000 002218 000156 00      0   0  1
  [27] .debug_line       PROGBITS        00000000 00236e 0001b2 00      0   0  1
  [28] .debug_frame      PROGBITS        00000000 002520 00004c 00      0   0  4
  [29] .debug_str        PROGBITS        00000000 00256c 000234 01  MS  0   0  1
  [30] .debug_loc        PROGBITS        00000000 0027a0 0000b6 00      0   0  1
  [31] .symtab           SYMTAB          00000000 002858 000460 10     32  53  4
  [32] .strtab           STRTAB          00000000 002cb8 0001eb 00      0   0  1
  [33] .shstrtab         STRTAB          00000000 002ea3 000131 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  p (processor specific)

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  PHDR           0x000034 0x00002034 0x00002034 0x000e0 0x000e0 R   0x4
  INTERP         0x00053a 0x0000253a 0x0000253a 0x00018 0x00018 R   0x1
      [Requesting program interpreter: /lib/ld-linux-or1k.so.1]
  LOAD           0x000000 0x00002000 0x00002000 0x00578 0x00578 R E 0x2000
  LOAD           0x001f08 0x00005f08 0x00005f08 0x0011c 0x00120 RW  0x2000
  DYNAMIC        0x001f10 0x00005f10 0x00005f10 0x000f0 0x000f0 RW  0x4
  NOTE           0x000558 0x00002558 0x00002558 0x00020 0x00020 R   0x4
  GNU_RELRO      0x001f08 0x00005f08 0x00005f08 0x000f8 0x000f8 R   0x1

 Section to Segment mapping:
  Segment Sections...
   00     
   01     .interp 
   02     .hash .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt .init .plt .text .fini .rodata .interp .eh_frame .note.ABI-tag 
   03     .init_array .fini_array .dynamic .data .got .bss 
   04     .dynamic 
   05     .note.ABI-tag 
   06     .init_array .fini_array .dynamic 
```

### Dump of the dynamic table

First entry of PLT points here?

```
$ or1k-smh-linux-gnu-readelf -d ./hello

Dynamic section at offset 0x1f10 contains 25 entries:
  Tag        Type                         Name/Value
 0x00000001 (NEEDED)                     Shared library: [libc.so.6]
 0x0000000c (INIT)                       0x225c
 0x0000000d (FINI)                       0x251c
 0x00000019 (INIT_ARRAY)                 0x5f08
 0x0000001b (INIT_ARRAYSZ)               4 (bytes)
 0x0000001a (FINI_ARRAY)                 0x5f0c
 0x0000001c (FINI_ARRAYSZ)               4 (bytes)
 0x00000004 (HASH)                       0x2114
 0x6ffffef5 (GNU_HASH)                   0x213c
 0x00000005 (STRTAB)                     0x21b8
 0x00000006 (SYMTAB)                     0x2168
 0x0000000a (STRSZ)                      74 (bytes)
 0x0000000b (SYMENT)                     16 (bytes)
 0x00000015 (DEBUG)                      0x0
 0x00000003 (PLTGOT)                     0x6008
 0x00000002 (PLTRELSZ)                   36 (bytes)
 0x00000014 (PLTREL)                     RELA
 0x00000017 (JMPREL)                     0x2238
 0x00000007 (RELA)                       0x222c
 0x00000008 (RELASZ)                     12 (bytes)
 0x00000009 (RELAENT)                    12 (bytes)
 0x6ffffffe (VERNEED)                    0x220c
 0x6fffffff (VERNEEDNUM)                 1
 0x6ffffff0 (VERSYM)                     0x2202
 0x00000000 (NULL)                       0x0
```

### Dump of .real.plt

What is this?

```
$ or1k-smh-linux-gnu-readelf -x .rela.plt ./hello

Hex dump of section '.rela.plt':
  0x00002238 00006014 00000214 00000000 00006018 ..`...........`.
  0x00002248 00000114 00000000 0000601c 00000414 ..........`.....
  0x00002258 00000000                            ....
```

### Dump of replocations

```
$ or1k-smh-linux-gnu-readelf -r ./hello

Relocation section '.rela.dyn' at offset 0x22c contains 1 entry:
 Offset     Info    Type            Sym.Value  Sym. Name + Addend
00006020  00000113 R_OR1K_GLOB_DAT   00000000   __gmon_start__ + 0

Relocation section '.rela.plt' at offset 0x238 contains 3 entries:
 Offset     Info    Type            Sym.Value  Sym. Name + Addend
00006014  00000214 R_OR1K_JMP_SLOT   000022b0   puts@GLIBC_2.0 + 0
00006018  00000114 R_OR1K_JMP_SLOT   00000000   __gmon_start__ + 0
0000601c  00000414 R_OR1K_JMP_SLOT   000022d0   __libc_start_main@GLIBC_2.0 + 0
```
