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

#for sending time msg: set to 1, else set to 0
$sendtime = 0;

#for printing timestamps on printed messages: set to 1, else set to 0
$timestamp = 1;

#define filters for id of messages, passed messages will be printed on stdout
@acceptmask_ones =  ();
@acceptmask_zeros = ();
@denymask_ones = ();
@denymask_zeros = ();

@input = ();
@output = ();

$host = "localhost";
$port = 1200;
$wiki = "http://localhost";
$namespace = "homeautomation:livestats";
$pagename = "default";

require "settings.pl";
require "dokuwiki.pl";
#dokuwikipost("http://www.arune.se/temp/post.php", "homeautomation:livestats:arune", "texten att posta", "med summary");
#dokuwikipost($wiki."/doku.php", $namespace.":".$pagename, "texten att posta", "med summary");

$remote = IO::Socket::INET->new(
                    Proto    => "tcp",
                    PeerAddr => $host,
                    PeerPort => $port,
                )
               or die "cannot connect to port $port at $host";

#read line from socket (blocking)
while ( $line = <$remote> ) {
	if (length($line) > 12) {
		$id = hex(substr($line, 4, 8));
		
		#macro function
		for ($i = 0; $i < $input; $i++) {
			if ($input[$i] eq substr($line, 0, length($input[$i]))) {
				print $remote $output[$i];
			}
		}
		
		$printed = 0;
		$denied = 0;
		#filter function
		for ($i = 0; $i < $acceptmask_ones && !$printed && !$denied; $i++) {
			if ((($id & $acceptmask_ones[$i])==$acceptmask_ones[$i]) && (($id & $acceptmask_zeros[$i])==0)) {
				#check if denied by $denymask
				for ($j = 0; $j < $denymask_ones; $j++) {
					if ((($id & $denymask_ones[$j])==$denymask_ones[$j]) && (($id & $denymask_zeros[$j])==0)) {
						#this message fits the denymask
						$denied = 1;
					}
					
				}
				
				if (!$denied) {
					#print timestamp
					if ($timestamp == 1) {
						print localtime()." ";
					}
					print $line;
					
					$printed = 1;
				}
			}
		}
	}
}

sub sendTime {
	$remote = IO::Socket::INET->new(
                    Proto    => "tcp",
                    PeerAddr => $host,
                    PeerPort => $port,
                )
               or die "cannot connect to port $port at $host";
	
	while (1) {
		select(undef, undef, undef, 0.5);
		print $remote "PKT 00000000 1 0";
	}
}
