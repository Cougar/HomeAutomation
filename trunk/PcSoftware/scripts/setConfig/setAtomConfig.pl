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

use IO::Select;
use Fcntl ':seek';

$host = "localhost";
$port = 1201;

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
$return = atomd_read_command_response($socket);

my @lines = split(/\n/, $return);

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
		$devicename = substr($line, index($line, ":") + 1);
	
		$devicename =~ s/^\s+//;
		$devicename =~ s/\s+$//;
	
		@device = $devices[0];
		foreach (@devices)
		{
			if (@{$_}[0] eq $devicename)
			{
				@device = @{$_};
			}
		}
		
		if ($device[0] eq "unknown") 
		{
			print "\nWarning: Unknown device type, will not set MCU settings\n\n";
		}
		else
		{
			print "Setting device type to ".$device[0]."\n";
		}
	}
}

if ($hwid eq "")
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


# TODO: Move all atomic-protocol functions to a separate file/lib

sub atomd_connect
{
	($host, $port) = @_;
	$socket = IO::Socket::INET->new(
			Proto    => "tcp",
			PeerAddr => $host,
			PeerPort => $port,
			Blocking => 1,
		)
		or die "Error: Cannot connect to port $port at $host\n";

	return $socket;
}

sub atomd_read_packet
{
	($socket) = @_;
	$command="";
	#while (length($command)<4)
	#{
		#sysread blocks and reads at least 1 byte before returning
	#	$bytes = sysread($socket, $buf, 1);
	#	$command .= $buf;
	#}
	read($socket, $command, 4);

	$payload_length="";
	#while (length($payload_length)<4)
	#{
	#	$bytes = sysread($socket, $buf, 1);
	#	$payload_length .= $buf;
	#}
	read($socket, $payload_length, 4);

	$payload_length = int($payload_length);
	$payload="";
	if ($payload_length > 0)
	{
		#while (length($payload) < $payload_length)
		#{
		#	$bytes = sysread($socket, $buf, 1);
		#	$payload .= $buf;
		#}
		read($socket, $payload, $payload_length);
	}
	
	$payload_length = sprintf("%04d", $payload_length);
	
	#print "areadpacket ".$command . $payload_length . $payload."\n";
	return $command . $payload_length . $payload;
}

sub atomd_write_packet
{
	($socket, $command, $payload) = @_;
	
	$payload_length = sprintf("%04d", length($payload)+1);
	$packet = $command.$payload_length.$payload.chr(0);

	#print "awritepacket ".$packet."\n";
	print $socket $packet;
}


sub atomd_data_available
{
	($socket) = @_;
	#Get the current buffer position
	$position = tell($socket);

	#Set the current position to end of buffer
	seek($socket, -1, SEEK_END);

	#Compare end postion to stored position
	$has_data = tell($socket) > $position;

	#Restore buffer position
	seek($socket, $position, SEEK_SET);

	return $has_data;
}


sub atomd_initialize
{
	($host, $port) = @_;
	$socket = atomd_connect($host, $port);
	
	#while (!atomd_data_available($socket)) {}
	
	#while (atomd_data_available($socket))
	#{
	#	$packet = atomd_read_packet($socket); # Read initial prompt
	#}
	
	# Read initial prompt
	$s = IO::Select->new();
	$s->add($socket);
	$s->can_read(1);
	
	sysread($socket, $data, 9000);
	$data="";
	#print "ainit".$data."\n";
	return $socket;
}


sub atomd_send_command
{
	($socket, $command) = @_;
	atomd_write_packet($socket, "RESP", $command);
}


sub atomd_read_command_response
{
	($socket) = @_;

	$response = "";

	while (1)
	{
		$packet = atomd_read_packet($socket);

		if (substr($packet, 0, 4) ne "TEXT")
		{
			last;
		}

		$response .= substr($packet, 8) . "\n";
	}

	return $response;
}

