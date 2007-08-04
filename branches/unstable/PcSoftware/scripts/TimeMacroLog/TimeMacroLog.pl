#!/usr/bin/perl -w
use IO::Socket;
use integer;
use threads;

#for sending time msg: set to 1, else set to 0
$sendtime = 0;

#for printing timestamps on printed messages: set to 1, else set to 0
$timestamp = 1;

#define filters for id of messages, passed messages will be printed on stdout
@acceptmask_ones =  (0x00080000, 0x00240000, 0x00040000, 0x00280000);
@acceptmask_zeros = (0xfff70000, 0xffdb0000, 0xfffb0000, 0xffd70000);
@denymask_ones = ();
@denymask_zeros = ();

@input = ();
@output = ();

#define makros, when an input sting is sent on can this perl-script will send the corresponding output string

# Remote button 1 => desktoplamp toggle
push(@input , "PKT 0409000a 1 0 00 00 05 01");
push(@output, "PKT 08008c00 1 0 03");

# Remote button 4 down => blinds darker
push(@input , "PKT 0409000a 1 0 00 00 05 04");
push(@output, "PKT 08010200 1 0 00 03");

# Remote button 4 up => blinds stop
push(@input , "PKT 0409000a 1 0 0f 00 05 04");
push(@output, "PKT 08010200 1 0 00 04");

# Remote button 6 down => blinds lighter
push(@input , "PKT 0409000a 1 0 00 00 05 06");
push(@output, "PKT 08010200 1 0 80 03");

# Remote button 6 up => blinds stop
push(@input , "PKT 0409000a 1 0 0f 00 05 06");
push(@output, "PKT 08010200 1 0 00 04");


$remote = IO::Socket::INET->new(
                    Proto    => "tcp",
                    PeerAddr => "localhost",
                    PeerPort => "1200",
                )
               or die "cannot connect to port 1200 at localhost";

if ($sendtime == 1) {
	$thr = threads->new(\&sendTime);
	$thr->detach;
}

$acceptmask_ones = @acceptmask_ones;
$acceptmask_zeros = @acceptmask_zeros;
if ($acceptmask_ones != $acceptmask_zeros) {
	print "Acceptmasks must be same length\n";
	exit;
}

$denymask_ones = @denymask_ones;
$denymask_zeros = @denymask_zeros;
if ($denymask_ones != $denymask_zeros) {
	print "Denymasks must be same length\n";
	exit;
}

#get the lengths of the makro arrays:
$input = @input;
$output = @output;
if ($input != $output) {
	print "Input and output makros must be same length\n";
	exit;
}

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
                    PeerAddr => "localhost",
                    PeerPort => "1200",
                )
               or die "cannot connect to port 1200 at localhost";
	
	while (1) {
		select(undef, undef, undef, 0.5);
		print $remote "PKT 00000000 1 0";
	}
}
