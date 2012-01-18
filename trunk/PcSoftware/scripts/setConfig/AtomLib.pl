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
		read($socket, $payload, $payload_length);
	}
	
	#Pad payload length with 0s
	$payload_length = sprintf("%04d", $payload_length);
	
	#print "areadpacket ".$command . $payload_length . $payload."\n";
	return $command . $payload_length . $payload;
}

sub atomd_write_packet
{
	($socket, $command, $payload) = @_;
	
	#Pad payload length with 0s
	$payload_length = sprintf("%04d", length($payload)+1);
	$packet = $command.$payload_length.$payload.chr(0);

	#print "awritepacket ".$packet."\n";
	print $socket $packet;
}


sub atomd_data_available
{
	($socket) = @_;

	$s = IO::Select->new();
	$s->add($socket);
	@handles = $s->can_read(0.001);

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
		$response .= substr($packet, 8); 
		$response .= "\n";
	}

	return $response;
}

# "return" 1 to not generate an error when loading file
1;

