#!/usr/bin/perl

# 
# for use with serial ports, install module with apt-get install libdevice-serialport-perl
# 
# 
# 


$hubport = 1200;
#$udpip = "193.11.254.22";
#$udpport = 1100;


require 5.002;
use IO::Socket;
use IO::Select;
use threads;
use Getopt::Long;

use POSIX qw(setsid);

$binaryname = "canDaemon.pl";
if ( @ARGV > 0 ) {
	GetOptions(	"baudrate=i" 	=> \$baudarg,			#
			"server=s" 	=> \$serverarg,			#
			"port=i"	=> \$portarg, 			#
			"tcpport=i"	=> \$tcpportarg,		#
			"device=s"	=> \$devicearg,			#
			"help"		=> \$help,			#
			"xDaemon"	=> \$daemon,			#
						);
} else {
	$help = 1;
}

if ($help) {
	print "Usage ./$binaryname [options]\n";
	print "Options:\n";
	print "  -x 						Make canDaemon detach as a UNIX daemon\n";
	print "  -d <device> (udp or /dev/ttyXYZ)		Choose hardware for communication\n";
	print "  -b <baudrate> 					Baudrate, for serial devices\n";
	print "  -s <server>					Server, for udp devices\n";
	print "  -p <port>					Port, for udp devices\n";
	print "  -t <tcpport>					Port, for tcp server (default 1200)\n";
	print "  -h 						Shows this usage\n";
	print "\n";
	print "example: ./$binaryname -d /dev/ttyUSB0 -b 38400\n";
	print "example: ./$binaryname -d udp -s 192.168.0.10 -p 1100\n";

	exit 0;
}
if ($serverarg) {
	$udpip = $serverarg;
}
#print $udpip;
if ($portarg) {
	$udpport = $portarg;
}
if ($tcpportarg) {
	$hubport = $tcpportarg;
}

if ($daemon) {
	chdir '/'                 or die "Can't chdir to /: $!";
	umask 0;
	open STDIN, '/dev/null'   or die "Can't read /dev/null: $!";
	open STDOUT, '>/dev/null' or die "Can't write to /dev/null: $!";
	#open STDERR, '>/dev/null' or die "Can't write to /dev/null: $!";
	defined(my $pid = fork)   or die "Can't fork: $!";
	exit if $pid;
	setsid                    or die "Can't start a new session: $!";
}

$C2SSTARTSTR = 253;
$C2SENDSTR = 250;
$C2PINGSTR = 251;

### create a tcp server ###
$linelength=1000;
&tcpServerInit;
$thr = threads->new(\&tcpServerThread);
$thr->detach;

### create a connection to hardware ###
if ($devicearg) {
	if ($devicearg =~ m/^udp$/) {
		### if the hardware is an udp node ###
		&udpServerInit;
		$thr = threads->new(\&udpServerThread);
		$thr->detach;
	}
	### else, the hardware is serial, check for device, this might not work in windows ###
#	elsif (-e $devicearg) {
	else {
		if (!$baudarg) {
			print "For a serial device you need to specifiy baudrate, now using default 19200\n";
			$baudarg = 19200;
		}
		
		eval 'use Device::SerialPort';		### install module with aptitude install libdevice-serialport-perl
		if ($@) {
			eval 'use Win32::SerialPort';
			if ($@) {
				print "Could not find Device::SerialPort or Win32::SerialPort, I quit.\n";
				exit 0;
			} else {
				$usingWin32Serial = 1;
			}
		} else {
			$usingDeviceSerial = 1;
		}
		
		&serialConnInit;
		$thr = threads->new(\&serialConnThread);
		$thr->detach;		
	}
#	} else {
#		print "Argument $devicearg is not correct\n";
#		exit 0;
#	}
}


### loop here until user aborts with ctrl+c ###
while (1) { sleep 1; }


##################### Hardware connection to TCP HUB ############################
sub hardwareConnThread {
	### Check for data from HUB ###
    while ( $newmsg = <$hwCremote> ) {
    	### Convert to can2serial format ### 
    	&stringToc2s($newmsg, $retstring);
    	if ($retstring) {
    		#print length($retstring)."\n";
    		#&c2sToString($retstring, $newretstring);
    		#print "TO SEND: ".$newretstring."\n";
		    #print $udpCsocket $retstring;
		    
		    ### Send data ###
		    if (length($hardwareSendFunction) > 0) {
		    	&{$hardwareSendFunction}($retstring);
		    }
		    
    	}
    }	
}

sub hardwareConnInit {
	print "Connecting hardware to tcpServer\n";
    $hwCremote = IO::Socket::INET->new(Proto    => "tcp", PeerAddr => "localhost", PeerPort => $hubport,)
                  or die "cannot connect to $hubport port at localhost";
}

##################### Serial Connection ############################
sub serialConnThread {
	$serialBuffer = "";
	while(1) {
		my ($count,$saw)=$serialPort->read(1); # will read _up to_ 1 chars
		if ($count > 0) {
			$serialBuffer.=$saw;
			serialConnProcessBuffer($serialBuffer);
		}
	}
}

sub serialConnProcessBuffer {
	my $input = $_[0];
	my $output = $input;
	my $newmsg;
	my $retstring;
		
	### Cut all leading chars that is not a start-char ###
	for ($i = 0; $i < length($input); $i++) {
		if (ord(substr($input, 0, 1)) != $C2SSTARTSTR) {
			if (ord(substr($input, $i, 1)) == $C2SSTARTSTR) {
				$input = substr($input, $i);
				$output = $input;
				#print "debug: cutting character\n";
				break;
			} 
			if (ord(substr($input, $i, 1)) == $C2PINGSTR) {
				print "Got pong from hardware\n";
				$input = substr($input, $i+1);
				$output = $input;
			}
		} else {
			break;
		}
	}
	### Check for possible complete packets ###
	for ($i = 0; $i < length($input)-16; $i++) {
		my $newmsg = substr($input, $i, 17);
		$retstring = "";
		&c2sToString($newmsg, $retstring);
		if (length($retstring)>0) {
			if ($hwCremote) {
				### Send to tcp hub ###
				print $hwCremote $retstring . "\n";
			}
			#print $retstring."\n";
			$output = substr($input, $i+17);
			$i+=16;
		}
	}
	
	### Store modified buffer ###
	$_[0] = $output;
}

sub serialConnInit {
	print "Connecting to hardware over serial port\n";
	$quiet=0;
	if ($usingDeviceSerial == 1) {
		$serialPort = new Device::SerialPort($devicearg, $quiet) || die "failed to open serial port, $devicearg is not valid?";
	} elsif ($usingWin32Serial == 1) {
		$serialPort = new Win32::SerialPort($devicearg, $quiet) || die "failed to open serial port, $devicearg is not valid?";
	}
	$serialPort->databits(8)			|| die "failed setting databits";
	$serialPort->baudrate($baudarg)		|| die "failed setting baudrate";
	$serialPort->parity("none")			|| die "failed setting parity";
	$serialPort->stopbits(1)			|| die "failed setting stopbits";
	$serialPort->handshake("none")		|| die "failed setting handshake";
	$serialPort->write_settings    		|| die "no settings";
	
	$serialPort->read_char_time(0);     # don't wait for each character
	$serialPort->read_const_time(1000); # 1 second per unfulfilled "read" call

	print "Testing connection to hardware: ping (hardware should answer with a pong)\n";
	serialConnSend(chr($C2PINGSTR));
	
	### create a connection between the hardware and the tcp-server ###
	&hardwareConnInit;
	$hardwareSendFunction = "serialConnSend";		### set up callback for a 'send data' function
	$thr = threads->new(\&hardwareConnThread);
	$thr->detach;
}

sub serialConnSend {
	$cnt = $serialPort->write($_[0]);
}

##################### UDP Connection ############################
sub udpServerThread {
	$MAXLEN = 1024;
	my $newmsg;
	my $retstring;
	while ($udpSsocket->recv($newmsg, $MAXLEN)) {
		my($udpport, $ipaddr) = sockaddr_in($udpSsocket->peername);
		$peer_addr = inet_ntoa($ipaddr);
		
		### check ipaddr to ip in commandlinearg ###
		if ($udpip == $peer_addr) {
			#check len of incomming data
			if (length($newmsg) > 16 && length($newmsg) < 20) {
				$retstring = "";
				&c2sToString($newmsg, $retstring);
				if (length($retstring) > 0 && $hwCremote) {
					print $hwCremote $retstring . "\n";
				}
			}
		}
	} 
	
}

sub udpServerInit {
	print "Starting udpServer\n";
	$udpSsocket = IO::Socket::INET->new(LocalPort => $udpport, Proto => 'udp')
	    or die "socket: $@";

	$udpCsocket = IO::Socket::INET->new(PeerPort => $udpport, Proto => 'udp', PeerAddr => $udpip) 
    	or die "socket: $@";
	
	#här ska keep-alive/start-paket skickas till ipt enligt commandline-arg

	### create a connection between the hardware and the tcp-server ###
	&hardwareConnInit;
	$hardwareSendFunction = "udpServerSend";		### set up callback for a 'send data' function
	$thr = threads->new(\&hardwareConnThread);
	$thr->detach;
	
}

sub udpServerSend {
	print $udpCsocket $_[0];
}

##################### TCP HUB ############################
sub tcpServerThread {
	while(@ready = $tcpSselect->can_read) {
		for $tcpSsocket (@ready) {
			if($tcpSsocket == $tcpSlisten) {
				&tcpServerConn;
			} else {
				$tcpSsocket->recv($line,$linelength);
				if ($line eq "") {
					&tcpServerClientDis;
				} else {
					$line =~ s/\n.*//gm;	#remove newline and everything after it
					$line =~ s/\r.*//gm;	#remove linefeed and everything after it
					print "Got packet '".$line."' from client ".$tcpSsocket->fileno."\n";
					
					if ($line eq "PING") {
						print "Sending PONG to client ".$tcpSsocket->fileno."\n";
						
						$tcpSsocket->send("PONG\n");
					}
					else
					{
						$rxerror = 0;
						testPacket($line, 0, $rxerror);
						
						if ($rxerror == 0) {
							#Add newline to each packet
							$line .= "\n";
												
							&tcpServerBroadcastExcept($line, $tcpSsocket);
						}
					}
				}
			}
		}
	}
}

#initiate the tcp server
sub tcpServerInit {
	$tcpSlisten = IO::Socket::INET->new(Proto => "tcp", LocalPort => $hubport, Listen => 1, Reuse => 1)
				or die $!;

	$tcpSselect = IO::Select->new($tcpSlisten);
	print "Starting tcpServer\n";
}

#a client connected
sub tcpServerConn {
	$new = $tcpSlisten->accept;
	$tcpSselect->add($new);
	print $new->fileno . ": connected\n";
}

#a client disconnected
sub tcpServerClientDis {
	print $tcpSsocket->fileno . ": disconnected\n";
	$tcpSselect->remove($tcpSsocket);
	$tcpSsocket->close;
}

#subroutine sends data to all connected clients except for one specified 
sub tcpServerBroadcastExcept {
	my $first = 0;
	my $data = $_[0];
	my $exception = $_[1];
	for $eachsocket ($tcpSselect->handles) {
		if ($eachsocket==$tcpSlisten) {		#dont send to self
			next;
		} elsif ($eachsocket==$exception) {		#dont send to the specified exception
			next;
		} else {
			if ($first==0) {
				$first = 1;
				print "Sending to client ".$eachsocket->fileno;
			} else {
				print ", ".$eachsocket->fileno;
			}
			$eachsocket->send($data) or do {
				&tcpServerClientDis;
			}
		}
	}
	if ($first==1) {
		print "\n";
	}
}

##################### General functions ############################

#this function should do a number of tests on $_[0] and print messages if packet is malformed and return 1 in $_[2]
#messages can be suppressed by having $_[1] as 1
sub testPacket {
	my $input = $_[0];
	my $messages = $_[1];
	
	#line should be parsed somewhat here, check PKT, AUT and so on
	$testErr = 0;
	if (length($input) < 16) {
		$testErr = 1;
		if ($messages!=1) {print "Packet was malformed, not long enough\n";}
	}
	if ($testErr == 0 && substr($input, 0, 4) != "PKT ") {
		$testErr = 1;
		if ($messages!=1) {print "Packet was malformed, should start with PKT\n";}
	}
	#if (substr($line, 12, 1) != " " || substr($line, 14, 1) != " ") {
	#	$testErr = 1;
	#	print "Packet was malformed, spaces not in the correct place\n";
	#}
	@splitinput = split(/ +/, $input);
	$inputarrlen = @splitinput;
	if ($testErr == 0 && ($inputarrlen < 4 || $inputarrlen > 12)) {
		$testErr = 1;
		if ($messages!=1) {print "Packet was malformed, to many or to few parts\n";}
	}
	if ($testErr == 0 && length($splitinput[1]) != 8) {
		$testErr = 1;
		if ($messages!=1) {print "Packet was malformed, id part must be 8 digits (padded with zeros)\n";}
	}
	if ($testErr == 0 && length($splitinput[2]) != 1) {
		$testErr = 1;
		if ($messages!=1) {print "Packet was malformed, ext part must be 1 digit\n";}
	}
	if ($testErr == 0 && length($splitinput[3]) != 1) {
		$testErr = 1;
		if ($messages!=1) {print "Packet was malformed, rtr part must be 1 digit\n";}
	}
	for ($i = 4; $i < $inputarrlen; $i++) {
		if ($testErr == 0 && length($splitinput[$i]) != 2) {
			$testErr = 1;
			if ($messages!=1) {print "Packet was malformed, data parts must be 2 digits (padded with zero)\n";}
		}
	}
	$checkstrlen = length($input)-4;
	$checkstr = substr($input, 4, $checkstrlen);
	if ($testErr == 0 && $checkstr !~ m/^[0-9a-fA-F ]*$/) {
		$testErr = 1;
		if ($messages!=1) {print "Packet was malformed, not hex\n";}
	}
	
	$_[2] = $testErr;
}

#this function should parse $_[0] (can2serial-data) and save in $_[1] as string
sub c2sToString {
	my $input = $_[0];
	my $output = "";
	#if not correct input data, then dont save anything in $_[1]
	if (ord(substr($input, 0, 1))==$C2SSTARTSTR && ord(substr($input, 16, 1))==$C2SENDSTR) {
		#$output = "boundaries=ok ";
		$id = ord(substr($input, 1, 1)) + ord(substr($input, 2, 1))*256 + ord(substr($input, 3, 1))*256*256 + ord(substr($input, 4, 1))*256*256*256;
		$ext = ord(substr($input, 5, 1));
		$rtr = ord(substr($input, 6, 1));
		$dl = ord(substr($input, 7, 1));
		if ($ext < 2 && $rtr < 2 && $dl < 9) {
			$output .= sprintf "%08x ", $id;
			$output .= $ext . " " . $rtr;
			for ($i = 0; $i < $dl; $i++) {
				$data = ord(substr($input, 8+$i, 1));
				$output .= sprintf " %02x", $data;
			}
			$_[1] = "PKT ".$output;
		} else {
			#print "debug: incorrect ext, rtr or dl\n";

		}
	} else {
		#print "debug: incorrect boundaries\n";

	}
}

#this function should parse $_[0] (string) and save in $_[1] as can2serial-data
sub stringToc2s {
	my $input = $_[0];
	my $output = "";
	#if not correct input data, then dont save anything in $_[1]
	if (length($input) > 15) {
		#minor checks, the data from the tcp-server should be valid
		@splitinput = split(/ +/, $input);
		$inputarrlen = @splitinput;
		if (substr($input, 0, 4) == "PKT " && substr($input, 12, 1) == " " && substr($input, 14, 1) == " " && $inputarrlen > 3 && $inputarrlen < 13) {
			$id = hex($splitinput[1]);
			$ext = hex($splitinput[2]);
			$rtr = hex($splitinput[3]);
			@data = @splitinput[4..($inputarrlen-1)];
			$dl = @data;
			
			$output .= chr($C2SSTARTSTR);
			$output .= chr($id & 0xff);
			$output .= chr(($id>>8) & 0xff);
			$output .= chr(($id>>16) & 0xff);
			$output .= chr(($id>>24) & 0xff);
			$output .= chr($ext);
			$output .= chr($rtr);
			$output .= chr($dl);
			for ($i = 0; $i < 8; $i++) {
				if ($i < $dl) {
					$output .= chr(hex($data[$i]));
				} else {
					$output .= chr(" ");
				}
			}
			$output .= chr($C2SENDSTR);
			$_[1] = $output;
		}
	}
}
