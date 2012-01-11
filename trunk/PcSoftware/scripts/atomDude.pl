#!/usr/bin/perl

# Written by Mattias Runge 2008-2010

use Cwd 'abs_path';

$| = 1;

$numArgs = $#ARGV + 1;

my $hostname = "localhost";
my $port = 1201;
my $filename = "";
my $hwid = "";
my $bios = 0;
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
		$filename = abs_path($ARGV[$argnum+1]);
		next;

	}
	elsif ($ARGV[$argnum] eq "-b")
	{
		$bios = 1;
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
$hwid = $hwidChars[0]. $hwidChars[1]. uc($hwidChars[8]. $hwidChars[9]. $hwidChars[6]. $hwidChars[7]. $hwidChars[4]. $hwidChars[5]. $hwidChars[2]. $hwidChars[3]);

print "atomDude settings:\n";
print "Hardware id: " . $hwid . "\n";
print "Atom address: " . $hostname . ":" . $port . "\n";
print "Hexfile: " . $filename . "\n";
print "Parameters: " . "\n";

if ($reset eq "true" && $filename eq "")
{
	my $result = system("atomic -s $hostname -p $port -c \"Node_Reset $hwid\"");
	exit($result);
}
else
{
	my $result = system("atomic -s $hostname -p $port -c \"Node_Program $hwid $bios $filename\"");
        exit($result);
}

exit(0);
