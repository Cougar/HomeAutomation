#!/usr/bin/perl -w
################################################
#
# author: arune @ efnet
#
################################################

use integer;
use threads;
use Cwd qw(abs_path);
use File::Basename;
use Getopt::Long;

$host = "localhost";
$port = 1201;

# Get hostname and port from command line arguments
if ( @ARGV > 0 ) {
	GetOptions(	"host=s" 	=> \$host,			#
			"port=i"	=> \$port, 			#
						);
} 

my $path = dirname(abs_path($0));

# Read atom functions from separate file
if (-e $path."/AtomLib.pl") {
	require $path."/AtomLib.pl";
} else {
	print "Error: No AtomLib.pl was found in ".$path.". \n\n";
	exit 1;
}

# Read devices from separate file
if (-e $path."/devices.pl") {
	require $path."/devices.pl";
} else {
	print "Error: No devices.pl was found in ".$path.". \n\n";
	exit 1;
}

$biosfile = "bios.inc";
open (FP, "+<".$biosfile) or die "Error: Cannot open file $biosfile\n";
@file = <FP>;
close FP;


# Check bios.inc for the settings that should be updated
my $foundidrow = 0;
my $foundmcurow = 0;
my $foundhfrow = 0;
my $foundlfrow = 0;
my $foundefrow = 0;
my $foundbootrow = 0;
foreach $file (@file) {
	if ($file =~ m/^NODE_HW_ID=/) {
		print "\nCurrent id-setting: ".$file."\n";
		$foundidrow = 1;
	}
	if ($file =~ m/^MCU=/) {
		$foundmcurow = 1;
	}
	if ($file =~ m/^HIGHFUSE=/) {
		$foundhfrow = 1;
	}
	if ($file =~ m/^LOWFUSE=/) {
		$foundlfrow = 1;
	}
	if ($file =~ m/^EXTFUSE=/) {
		$foundefrow = 1;
	}
	if ($file =~ m/^BOOT_START=/) {
		$foundbootrow = 1;
	}
}
if ($foundidrow == 0) {
	print "Error: Did not find NODE_HW_ID= in file $biosfile\n";
	exit 1;
}
if ($foundmcurow == 0) {
	print "Error: Did not find MCU= in file $biosfile\n";
	exit 1;
}
if ($foundhfrow == 0) {
	print "Error: Did not find HIGHFUSE= in file $biosfile\n";
	exit 1;
}
if ($foundlfrow == 0) {
	print "Error: Did not find LOWFUSE= in file $biosfile\n";
	exit 1;
}
if ($foundefrow == 0) {
	print "Error: Did not find EXTFUSE= in file $biosfile\n";
	exit 1;
}
if ($foundbootrow == 0) {
	print "Error: Did not find BOOT_START= in file $biosfile\n";
	exit 1;
}

my $hwid = "";
my @device = ();

#print "/usr/bin/atomic -s $host -p $port -c \"Node_WaitForInformation\"\n";
#my @lines = qx{/usr/bin/atomic -s $host -p $port -c \"Node_GetInformation 0xB23F54EA\"};
#my @lines = qx{/usr/bin/atomic -s $host -p $port -c \"Node_WaitForInformation\"};

$socket = atomd_initialize($host, $port);
atomd_send_command($socket, "Node_WaitForInformation");
#atomd_send_command($socket, "Node_GetInformation 0x7DD44E0A");
$return = atomd_read_command_response($socket);

my @lines = split(/\n/, $return);

my $foundDeviceType = 0;
foreach (@lines)
{
	my $line = $_;
	print($line."\n");
	chomp($line);
	
	if (index($line, "Id") != -1)
	{
		my $temp = substr($line, index($line, ":") + 1);
	
		$temp =~ s/^\s+//;
		$temp =~ s/\s+$//;

		my @hwidChars = split(//, $temp);
		$hwid = $hwidChars[0]. $hwidChars[1]. $hwidChars[8]. $hwidChars[9]. $hwidChars[6]. $hwidChars[7]. $hwidChars[4]. $hwidChars[5]. $hwidChars[2]. $hwidChars[3];

		print "Setting HWID in bios.inc to ".$hwid."\n";
	}
	elsif (index($line, "Device Type") != -1)
	{
		$devicename = substr($line, index($line, ":") + 1, -1);
	
		$devicename =~ s/^\s+//;
		$devicename =~ s/\s+$//;
		
		@device = $devices[0];
		foreach (@devices)
		{
			if (@{$_}[0] eq $devicename)
			{
				@device = @{$_};
				$foundDeviceType = 1;
			}
		}
		
		if ($foundDeviceType == 0) 
		{
			print "\nWarning: Unknown device type, will not set MCU settings\n\n";
		}
		else
		{
			print "Setting device type to ".$device[0]."\n";
		}
	}
}

if ($hwid eq "" || $foundDeviceType == 0)
{
	exit(1);
}

# Open bios.inc and replace all settings
open (FP, "<".$biosfile) or die "Cannot open file $biosfile\n";
@file = <FP>;
close FP;
open (FP, ">".$biosfile) or die "Cannot open file $biosfile\n";
foreach $file (@file) {
	$file =~ s/^NODE_HW_ID=.*/NODE_HW_ID=$hwid/g;
	if ($device[0] ne "unknown") 
	{
		$file =~ s/^MCU=.*/MCU=$device[0]/g;
		$file =~ s/^HIGHFUSE=.*/HIGHFUSE=$device[1]/g;
		$file =~ s/^LOWFUSE=.*/LOWFUSE=$device[2]/g;
		$file =~ s/^EXTFUSE=.*/EXTFUSE=$device[3]/g;
		$file =~ s/^BOOT_START=.*/BOOT_START=$device[4]/g;
	}
	print FP $file;
}
close FP;

exit(0);

