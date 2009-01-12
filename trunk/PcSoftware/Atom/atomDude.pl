#!/usr/bin/perl

# Written by Mattias Runge 20089

use IO::Socket;

$| = 1;

$numArgs = $#ARGV + 1;

my $hostname = "localhost";
my $port = 1202;
my $filename = "";
my $hwid = "";
my $bios = "false";

foreach $argnum (0 .. $#ARGV)
{
	if ($ARGV[$argnum] eq "-h")
	{
		$hostname = $ARGV[$argnum+1];
	}
	elsif ($ARGV[$argnum] eq "-p")
	{
		$port = $ARGV[$argnum+1];
	}
	elsif ($ARGV[$argnum] eq "-i")
	{
		$hwid = $ARGV[$argnum+1];
	}
	elsif ($ARGV[$argnum] eq "-f")
	{
		$filename = $ARGV[$argnum+1];
	}
	elsif ($ARGV[$argnum] eq "-b")
	{
		$bios = "true";
	}
}

if ($filename eq "")
{
	print "You must specify a file!\n";
	exit 1;
}
elsif ($hwid eq "")
{
	die "You must specify a hardware id!\n";
	exit 1;
}

#0x14f90d1f to 0x1f0df914
@hwidChars = split(//, $hwid);
$hwid = @hwidChars[0]. @hwidChars[1]. @hwidChars[8]. @hwidChars[9]. @hwidChars[6]. @hwidChars[7]. @hwidChars[4]. @hwidChars[5]. @hwidChars[2]. @hwidChars[3];

open(FILE, $filename) || die("Could not open file!");
@fileData=<FILE>; 
close(FILE);

my $sock = new IO::Socket::INET(
				PeerAddr => $hostname, 
				PeerPort => $port, 
				Proto => 'tcp'
				);
die "Could not create socket: $!\n" unless $sock;

sleep(1);

print $sock "programNode('", $hwid, "','";

foreach $line (@fileData)
{
	print $sock $line;
} 

print $sock "', ", $bios, ");\n";

while (1)
{
	$sock->recv($text, 128);
	
	if (rindex($text, "STOP") ne -1)
	{
		print "\n";
		exit 0;
	}
	elsif ($text ne '')
	{
		print $text;
	}
	else
	{
		print "Connection closed by other side!";
		exit 0;
	}
}

close($sock);
