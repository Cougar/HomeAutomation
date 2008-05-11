#!/usr/bin/perl -w
use IO::Socket;
use integer;
use threads;

@input = ();
@output = ();

#define makros, when an input sting is sent on can this perl-script will send the corresponding output string

# Remote button 1 => desktoplamp toggle
push(@input , "PKT 04000003 1 0 00 00 1e 14");
push(@output, "key up\n");

push(@input , "PKT 04000003 1 0 00 00 1e 15");
push(@output, "key down\n");

push(@input , "PKT 04000003 1 0 00 00 1e 16");
push(@output, "key left\n");

push(@input , "PKT 04000003 1 0 00 00 1e 17");
push(@output, "key right\n");

push(@input , "PKT 04000003 1 0 00 00 1e 25");
push(@output, "key enter\n");

push(@input , "PKT 04000003 1 0 00 00 1e 12");
push(@output, "key return\n");

print "---------------------------------\n";
print "MythTV Link 0.1\n";
print "Link between MythTV and canDeamon\n";
print "---------------------------------\n";

print "Initiating sockets to MythTv and canDaemon...";
$remoteMyth = IO::Socket::INET->new(
                    Proto    => "tcp",
                    PeerAddr => "localhost",
                    PeerPort => "6546",
                )
               or die " FAIL: cannot connect to port 6546 at localhost";

$remote = IO::Socket::INET->new(
                    Proto    => "tcp",
                    PeerAddr => "localhost",
                    PeerPort => "1200",
                )
               or die " FAIL: cannot connect to port 1200 at localhost";

print "Success!\n";

print "Reading Macro Array...";


#get the lengths of the makro arrays:
$input = @input;
$output = @output;
if ($input != $output) {
	print "Input and output makros must be same length\n";
	exit;
}
print "Success!\n";

#read line from socket (blocking)
while ( $line = <$remote> ) {
	if (length($line) > 12) {
		$id = hex(substr($line, 4, 8));
		
		#macro function
		for ($i = 0; $i < $input; $i++) {
			if ($input[$i] eq substr($line, 0, length($input[$i]))) {
				print $remoteMyth $output[$i];
				#print "Sending!!\n";
			}
		}


	}
}
