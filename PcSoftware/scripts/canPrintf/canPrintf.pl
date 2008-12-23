#!/usr/bin/perl -w
################################################
#
# This file should not be edited for settings
# use settings.pl for that
#
# author: arune @ efnet
#
################################################

//Set autoflush on ( http://www.perlmonks.org/?node_id=20590 )
$| = 1;

use IO::Socket;
use integer;
use threads;

#for printing timestamps on printed messages: set to 1, else set to 0
$timestamp = 1;

$host = "localhost";
$port = 1200;

require "settings.pl";

$remote = IO::Socket::INET->new(
                    Proto    => "tcp",
                    PeerAddr => $host,
                    PeerPort => $port,
                )
               or die "cannot connect to port $port at $host";

print "Connected to canDaemon. Printf strings on can will be printed below.\n"; 
print "Don't forget to terminate your strings with '\\n'.\n";

$newline = 1;
#read line from socket (blocking)
while ( $line = <$remote> ) {
	if (length($line) > 12) {
		#$idhex = substr($line, 4, 8);
		$id = hex(substr($line, 4, 8));
		if ($id == hex("1f000001")) {
			#@data = ();
			if ($newline == 1) {
				if ($timestamp == 1) {
					print localtime()." ";
				}
				$newline = 0;
			}
			for ($i = 17; $i < length($line); $i=$i+3) {
				print pack("c", hex(substr($line, $i, 2)));
				if (hex(substr($line, $i, 2)) == 10) {
					$newline = 1;
				}
			}
		}
	}
}

