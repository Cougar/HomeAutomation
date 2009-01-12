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
my $reset = "false";

foreach $argnum (0 .. $#ARGV)
{
	if ($ARGV[$argnum] eq "-h")
	{
		$hostname = $ARGV[$argnum+1];
		next;

	}
	elsif ($ARGV[$argnum] eq "-p")
	{
		$port = $ARGV[$argnum+1];
		next;

	}
	elsif ($ARGV[$argnum] eq "-i")
	{
		$hwid = $ARGV[$argnum+1];
		next;

	}
	elsif ($ARGV[$argnum] eq "-f")
	{
		$filename = $ARGV[$argnum+1];
		next;

	}
	elsif ($ARGV[$argnum] eq "-b")
	{
		$bios = "true";
	}
	elsif ($ARGV[$argnum] eq "-r")
	{
		$reset = "true";
	}
}

if ($hwid eq "")
{
	die "You must specify a hardware id!\n";
	exit 1;
}

#0x14f90d1f to 0x1f0df914
@hwidChars = split(//, $hwid);
$hwid = @hwidChars[0]. @hwidChars[1]. @hwidChars[8]. @hwidChars[9]. @hwidChars[6]. @hwidChars[7]. @hwidChars[4]. @hwidChars[5]. @hwidChars[2]. @hwidChars[3];

print "atomDude settings:\n";
print "Hardware id: " . $hwid . "\n";
print "Atom address: " . $hostname . ":" . $port . "\n";
print "Hexfile: " . $filename . "\n";
print "Parameters: " . "\n";

my $sock = new IO::Socket::INET(
				PeerAddr => $hostname, 
				PeerPort => $port, 
				Proto => 'tcp'
				);
die "Could not create socket: $!\n" unless $sock;
print "Connected to Atom\n";

if ($filename ne "")
{
	open(FILE, $filename) || die("Could not open file!");
	@fileData=<FILE>; 
	close(FILE);
	print "Hexfile loaded\n";

	print $sock "programNode('", $hwid, "','";

	foreach $line (@fileData)
	{
		print $sock $line;
	} 

	print $sock "', ", $bios, ");\n";

	my $pLoop = 1;

	while ($pLoop eq 1)
	{
		$sock->recv($text, 128);
		
		if (rindex($text, "STOP") ne -1)
		{
			print $text;
			$pLoop = 0;
		}
		elsif ($text ne '')
		{
			print $text;
		}
		else
		{
			print "Connection closed by other side!";
			close($sock);
			exit 1;
		}
	}
}

if ($reset eq "true")
{
	print $sock "resetNode('", $hwid, "');\n";

	my $rLoop = 1;
	
	while ($rLoop eq 1)
	{
		$sock->recv($text, 128);
		print $text;
		
		if (rindex($text, "Success") ne -1)
		{
			$rLoop = 0;
		}
		elsif (rindex($text, "Failed") ne -1)
		{
			close($sock);
			exit 1;
		}
		elsif ($text eq '')
		{
			print "Connection closed by other side!";
			close($sock);
			exit 1;
		}
	}
}

close($sock);
exit 0;
