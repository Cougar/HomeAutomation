#!/usr/bin/perl -w
################################################
#
# This file should not be edited for settings
# use settings.pl for that
#
# author: arune @ efnet
#
################################################

use IO::Socket;
use integer;
use threads;
use Cwd qw(abs_path);
use File::Basename;

$host = "localhost";
$port = 1200;

my $path = dirname(abs_path($0));

if (-e $path."/settings.pl") {
	require $path."/settings.pl";
} else {
	print "Warning: No settings.pl was found in ".$path.". \nUsing default settings localhost:1200 instead.\n\n";
}

$remote = IO::Socket::INET->new(
                    Proto    => "tcp",
                    PeerAddr => $host,
                    PeerPort => $port,
                )
               or die "Error: Cannot connect to port $port at $host\n";

$biosfile = "bios.inc";
open (FP, "+<".$biosfile) or die "Error: Cannot open file $biosfile\n";
@file = <FP>;
close FP;
$foundrow = 0;
foreach $file (@file) {
	if ($file =~ m/^NODE_HW_ID=<GENERATE_HW>/) {
		$foundrow = 1;
	}
}
if ($foundrow == 0) {
	print "Error: Did not find NODE_HW_ID=<GENERATE_HW> in file $biosfile\n";
	exit 1;
}

#print "\n";
print "Connect your node to the CAN-network (disconnect it first if it's already connected)\n";
print "Abort with Ctrl+c\n";
#read line from socket (blocking)
while ( $line = <$remote> ) {
#PKT 00087f00 1 0 39 13 01 20 78 56 34 12
        if (length($line) > 12) {
                $id = hex(substr($line, 4, 8));
                $class = ($id & 0x1E000000)>>25;
                if ($class == 0) {
	                $nmttype = ($id & 0x00FF0000)>>16;
	                if ($nmttype == 8) {
						print $line."";
						#print $class." ".$nmttype."\n";
						if (length($line) < 40) {
							print "Error: A node was started but it did not have a HWID\n";
							exit 1;
						}
						$hwid = (hex(substr($line, 38, 2))<<24)+(hex(substr($line, 35, 2))<<16)+
						(hex(substr($line, 32, 2))<<8)+(hex(substr($line, 29, 2))<<0);
						$hwidhex = sprintf("%08x", $hwid);
						print "Setting HWID in bios.inc to 0x".$hwidhex."\n";
						
						open (FP, "<".$biosfile) or die "Cannot open file $biosfile\n";
						@file = <FP>;
						close FP;
						open (FP, ">".$biosfile) or die "Cannot open file $biosfile\n";
						foreach $file (@file) {
							$file =~ s/^NODE_HW_ID=<GENERATE_HW>/NODE_HW_ID=0x$hwidhex/g;
							print FP $file;
						}
						close FP;
						
						exit 0;
						
	                }
                }
                
	}
}

