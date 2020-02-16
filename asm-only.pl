#!/bin/env perl

use strict;

while (<STDIN>) {
# c0000000 <_s_kernel_ro>:$
# ^I...$
# c0000100:^I19 e0 c0 3b ^Il.movhi r15,0xc03b$
# c0000104:^Ia9 ef c0 00 ^Il.ori r15,r15,0xc000$
  if ($_ =~ /(.+):\t(.. .. .. ..) \t(.+)/) {
     my $asm = $3;
     # l.bf c0087260 <generic_write_checks+0xd0>
     if ($asm =~ /(.+) (.+) <(.+)\+0x(.+)>/) {
        my $insn = $1;
        my $loc = $3;
        print "$insn <$loc+0xxxx>\n";
     } elsif ($asm =~ /(.+) (.+) <(.+)>/) {
        my $insn = $1;
        my $loc = $3;
        print "$insn <$loc>\n";
     } else {
        print $asm . "\n";
     }
  } elsif ($_ =~ /(.+) <(.+)>:/) {
     my $label = $2;
     print "<$label>:\n";
  } else {
     print $_;
  }
}
