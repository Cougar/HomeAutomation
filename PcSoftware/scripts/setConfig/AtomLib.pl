use IO::Select;
use IO::Socket;


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

sub atomd_disconnect
{
	($socket) = @_;
	close($socket);
}

# ----------------- Atom JS functions -----------------
sub atomjs_read_line
{
	($socket) = @_;
	# read line
	defined( $result = <$socket> ) or die "Readline failed: $! \n";
	
	return $result;
}

sub atomjs_write
{
	($socket, $command) = @_;
	print $socket $command;
}

#function atomjs_data_available($socket)
#{
#	$read   = array($socket);
#	$write  = NULL;
#	$except = NULL;
	
#	if (false === ($num_changed_streams = stream_select($read, $write, $except, 0)))
#	{
#		throw new Exception("could not do select on socket");
#	}

#	return $num_changed_streams > 0;
#}


# ----------------- Atomic functions -----------------

sub atomd_read_packet
{
	($socket) = @_;
	$command="";
	read($socket, $command, 4);

	$payload_length="";
	read($socket, $payload_length, 4);

	$payload_length = int($payload_length);
	$payload="";
	if ($payload_length > 0)
	{
		read($socket, $payload, $payload_length-1);
	}

	#Pad payload length with 0s
	$payload_length = sprintf("%04d", $payload_length-1);
	
	#print "areadpacket ".$command . $payload_length . $payload."\n";
	return $command . $payload_length . $payload;
}

sub atomd_write_packet
{
	($socket, $command, $payload) = @_;
	
	#Pad payload length with 0s
#	$payload_length = sprintf("%04d", length($payload)+1);
#	$packet = $command.$payload_length.$payload.chr(0);
	$payload_length = sprintf("%04d", length($payload));
	$packet = $command.$payload_length.$payload;

	#print "awritepacket ".$packet."\n";
	print $socket $packet;
}


sub atomd_data_available
{
	($socket) = @_;

	$s = IO::Select->new();
	$s->add($socket);
	@handles = $s->can_read(0.005);

	$has_data = 0;
	if (@handles)
	{
		$has_data = 1;
	}
	
	return $has_data;
}

sub atomd_kill_promt
{
	($socket) = @_;

	while (atomd_data_available($socket))
	{
		$packet = atomd_read_packet($socket); # Read prompt
	}
}

sub atomd_initialize
{
	($host, $port) = @_;
	$socket = atomd_connect($host, $port);
	
	atomd_kill_promt($socket);
	
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

		$packet =~ s/\n//g;
#		$response .= substr($packet, 8, -1); 
		$response .= substr($packet, 8); 
		$response .= "\n";
	}

	return $response;
}


# Perl trim function to remove whitespace from the start and end of the string
sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}
# Left trim function to remove leading whitespace
sub ltrim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	return $string;
}
# Right trim function to remove trailing whitespace
sub rtrim($)
{
	my $string = shift;
	$string =~ s/\s+$//;
	return $string;
}



# "return" 1 to not generate an error when loading file
1;

