#!/usr/bin/perl -w
################################################
#
# author: arune @ efnet
#
################################################

use IO::Socket;
use integer;
use threads;
use Cwd qw(abs_path);
use File::Basename;
use Getopt::Long;

$host = "localhost";
$port = 1200;

# Get hostname and port from command line arguments
if ( @ARGV > 0 ) {
	GetOptions(	"host=s" 	=> \$host,			#
			"port=i"	=> \$port, 			#
						);
} 

my $path = dirname(abs_path($0));

# Read devices from separate file
if (-e $path."/devices.pl") {
	require $path."/devices.pl";
} else {
	print "Error: No devices.pl was found in ".$path.". \n\n";
	exit 1;
}

# Length of devices array
$numdevices=@devices;

# Connect to candaemon
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

# Define variable to know if a device was found
my $devsettings=0;

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
						
						$devtype = hex(substr($line, 26, 2));
						if ($devtype == 0) 
						{
							print "\nWarning: Unknown device type, will not set MCU settings\n\n";
						}
						elsif ($devtype < $numdevices) 
						{
							print "Setting device type to ".$devices[$devtype][0]."\n";
							$devsettings = 1;
						}

						open (FP, "<".$biosfile) or die "Cannot open file $biosfile\n";
						@file = <FP>;
						close FP;
						open (FP, ">".$biosfile) or die "Cannot open file $biosfile\n";
						foreach $file (@file) {
							$file =~ s/^NODE_HW_ID=.*/NODE_HW_ID=0x$hwidhex/g;
							if ($devsettings==1)
							{
								$file =~ s/^MCU=.*/MCU=$devices[$devtype][0]/g;
								$file =~ s/^HIGHFUSE=.*/HIGHFUSE=$devices[$devtype][1]/g;
								$file =~ s/^LOWFUSE=.*/LOWFUSE=$devices[$devtype][2]/g;
								$file =~ s/^EXTFUSE=.*/EXTFUSE=$devices[$devtype][3]/g;
								$file =~ s/^BOOT_START=.*/BOOT_START=$devices[$devtype][4]/g;
							}
							print FP $file;
						}
						close FP;
						
						exit 0;
						
	                }
                }
                
	}
}

