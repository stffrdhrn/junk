#!/usr/bin/perl

use strict;

#open(FILE, "country_codes.txt");
#while (<FILE>) 
#{
#   chomp $_;
#   @parts = split (/\|/, $_);
#   if ($parts[2]) {
#      { "US",		N_("United States")},
  
#	print ("       { \"" . uc ($parts[2]) . "\", N_(\"" . $parts[3]. "\")},\n");
#   }
#}

#close(FILE);

open(FILE, "iso3166.txt");
while (<FILE>) 
{
   my @parts;
   
   chomp $_;
   @parts = split (/\;/, $_);
   if ($parts[1]) {
#      { "US",		N_("United States")},
	my $cn;
	my $final_name;
	my @cparts;

	$cn = $parts[0];
	$cn = lc $cn;
	
	@cparts = split(/,/, $cn);
	$cn = $cparts[0];
	@cparts = split(/ /, $cn);
	$final_name = "";
	foreach (@cparts)
	  {
	     if ("and" eq $_ || "the" eq $_)
	       {
		  $final_name = $final_name . $_ ." ";
	       }
	     else
	       {
		  $final_name = $final_name . ucfirst $_ ." ";
	       }
	  }
       $final_name	=~ s/\s+$//;
	
	print ("       { \"" . uc ($parts[1]) . "\", N_(\"" . $final_name. "\")},\n");
   }
}

close(FILE);
