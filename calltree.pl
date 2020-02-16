#!/usr/bin/perl

use strict;
use File::Basename;

# Uses globals to find a function, then parses that file to generate a call tree
# Recursive


my $maxdepth = 7;
my $func = $ARGV[0];
my $arch = $ARGV[1];

my %ignore = (
 BUG_ON => 1,
 WARN_ON => 1,
 WARN_ON_ONCE => 1,
 DEBUG_LOCKS_WARN_ON => 1,
 IS_ENABLED => 1,
 BUG => 1,
 BUILD_BUG_ON => 1,
 bug_at => 1,
 sizeof => 1,
 BIT => 1,
 pr_err => 1,
 ENTRY => 1,
 strcmp => 1,
 strcpy => 1,
 memcpy => 1,
 memcmp => 1,
 unlikely => 1,
);

my %seen = ();

sub get_id_type {
  my ($identifier) = @_;

  my @defs = `global -x $identifier`;
  foreach (@defs) {
    if (/ $identifier\(/) {
      return 1;
    } elsif (/define $identifier\(/) {
      return 2;
    }
  }
  return 0;
}

sub __find_func_calls {
  my ($decl, $body) = @_;

  my %seen;
  my $call_count = 0;
  my $res = [];
  foreach (@$body) {
    # search for an function call
    if (/([a-zA-Z0-9_]+)\(/) {
      while (/([a-zA-Z0-9_]+)/g) {
      #if (!$ignore{$1}) {
        if (!$seen{$1}) {
          my $type = get_id_type($1);
          if ($type) {
            $call_count++;
            push @$res, $1;
          }
          $seen{$1} = 1;
        }
      }
    }
    if ($call_count > 20) {
      print "Found more than 20 calls in $decl\n";
      last;
    }
  }
  return $res;
}

sub find_func_calls {

  my ($file, $line) = @_;

  my $fh;
  my $cline = 0;

  my $decl;
  my $body = [];
  my $comment = 0;

  open ($fh, $file) or die ("Failed to open in find callees for $file: $!");
  while (<$fh>) {
    chomp;
    $cline++;
    next if ($cline < $line);

    if ($line == $cline) {
      $decl = $_;
      # parsing defines is a bit more difficult ignore for now
      if (/define/) {
        # print "ignoring define: $_\n";
        last;
      }
      last if (/ENTRY/);
      next;
    }

    if (scalar @$body == 0) {
      if (/^{/) {
        push @$body, $_;
      } else {
        $decl .= $_;
      }
    }

    # Remove single line comments
    s/\/\*.*\*\///;
    if (/\/\*/) {
      $comment = 1;
    } elsif ($comment && /\*\//) {
      $comment = 0;
    } elsif ($comment) {
      # ignore comment body
    } else {
      push @$body, $_;
    }
    if (/^}/) {
      #print "$decl\n";
      return __find_func_calls($decl, $body);
    }

  }
  close $fh;
  return [];
}

sub find_tree {

  my ($func, $depth, $dir) = @_;

  my $indent = ("   " x $depth);

  if ($depth > $maxdepth) {
    print ($indent . "$func <max depth>\n");
    return;
  }

  # First search in current directly, then do wide search
  my @defs;
  if ($dir) {
    @defs = `global -x $func -S $dir`;
  }
  if (scalar @defs == 0) {
    @defs = `global -x $func -S arch/$arch`;
  }
  if (scalar @defs == 0) {
    @defs = `global -x $func`;
  }
  my $defcnt = 0;
  # arch_jump_label_transform   15 arch/powerpc/kernel/jump_label.c void arch_jump_label_transform(struct jump_entry *entry,
  foreach my $def (@defs) {
    $defcnt++;
    chomp $def;
    my @parts = split(/[ ]+/, $def);
    my $line = $parts[1];
    my $file = $parts[2];

    # only scan for the arch we are interested in
    if ($file =~ /^arch\//) {
      next unless ($file =~ /arch\/$arch\//);
    }
    # skip tools
    next if ($file =~ /^tools/);
    next if ($file =~ /^net/);
    next if ($file =~ /^drivers/);

    print $indent . "$defcnt:$func ($file:$line) {\n";

    my $child_funcs = find_func_calls($file, $line);
    foreach my $child_func (@$child_funcs) {
      #print $indent .' o '. $child_func . "\n";
      if (!$seen{$child_func}) {
        find_tree($child_func, $depth+1, dirname($file));
        $seen{$child_func} = 1;
      } else {
        printf $indent."   $child_func <seen>\n";
      }
    }

    print $indent . "}\n";
  }
}

find_tree($func, 0);
