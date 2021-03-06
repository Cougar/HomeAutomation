#!/usr/bin/perl

# Written by Mattias Runge 2008-2010

use Cwd 'abs_path';
use File::Basename;
use Time::HiRes qw( usleep );

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

my $path = dirname(abs_path($0));

# Read atom functions from separate file
if (-e $path."/setConfig/AtomLib.pl") {
	require $path."/setConfig/AtomLib.pl";
} else {
	print "Error: No AtomLib.pl was found in ".$path.". \n\n";
	exit 1;
}

#0x14f90d1f to 0x1f0df914
@hwidChars = split(//, $hwid);
$hwid = $hwidChars[0]. $hwidChars[1]. uc($hwidChars[8]. $hwidChars[9]. $hwidChars[6]. $hwidChars[7]. $hwidChars[4]. $hwidChars[5]. $hwidChars[2]. $hwidChars[3]);

print "atomDude settings:\n";
print "Hardware id: " . $hwid . "\n";
print "Atom address: " . $hostname . ":" . $port . "\n";
print "Hexfile: " . $filename . "\n";
#print "Parameters: " . "\n";

############################## RESET ##############################
if ($reset eq "true" && $filename eq "")
{
	$socket = atomd_initialize($hostname, $port);
	if ($port == 1203)
	{
		atomjs_write($socket, "Node_ResetNative('".$hwid."');\n");
		$response = atomjs_read_line($socket);
		if ($response =~ m/Error/)
		{
			die $response;
		}
		print $response;

		$success = 0;
		for ($count = 0; $count < 15; $count++)
		{
			atomjs_write($socket, "Node_ResetPollNative();\n");
			if (atomjs_read_line($socket) =~ m/true/)
			{
				print "\033[32m" . $hwid . " started okay.\033[0m\n";
				$success = 1;
				last;
			}
			usleep(200000);
		}
		if ($success == 0)
		{
			print "\033[31mTimed out waiting for reset.\033[0m\n";
		}
	}
	else
	{
		usleep(100000);
		atomd_kill_promt($socket);
		atomd_send_command($socket, "Node_Reset $hwid");
		print atomd_read_command_response($socket);
		#my $result = system("atomic -s $hostname -p $port -c \"Node_Reset $hwid\"");
		#exit($result);
	}
}
############################## REPROGRAM ##############################
else
{
	open (FP, "+<".$filename) or die "Error: Cannot open file $filename\n";
	@filecontents = <FP>;
	close FP;
	$socket = atomd_initialize($hostname, $port);

	if ($port == 1203)
	{
		atomjs_write($socket, "Node_ClearHex();\n");
		atomjs_read_line($socket);
		my $linenums = 0;
		foreach $filerow (@filecontents) 
		{
			$filerow =~ s/^\s+//;
			$filerow =~ s/\s+$//;
			$linenums += 1;
			$command = "Node_AppendHexNative('".$filerow."');\n";
			atomjs_write($socket, $command);
			$response = atomjs_read_line($socket);
			if ($response =~ m/Error/)
			{
				die $response;
			}
		}
		$command = "Node_ProgramNative('".$hwid."', '".$bios."', '".$linenums."');\n";
		atomjs_write($socket, $command);
		#print $command;
		$response = atomjs_read_line($socket);
		if ($response =~ m/Error/)
		{
			die $response;
		}
		print $response;
#atomd_disconnect($socket);
		$success = 0;
		$alreadyprintfull = 0;
		for ($count = 0; $count < 150; $count++)
		{
			atomjs_write($socket, "Node_ProgramPollNative();\n");
			if (atomjs_read_line($socket) =~ m/true/)
			{
				print progress_bar( 100, 100, 40, '=' );
				print "\n\033[32m" . $hwid . " started okay.\033[0m\n";
				$success = 1;
				last;
			}
			atomjs_write($socket, "Node_GetProgramProgress('".$hwid."');\n");
			$response = atomjs_read_line($socket);
			if ($response =~ m/Error/)
			{
				die $response;
			}
			$response =~ s/^\s+//;
			$response =~ s/\s+$//;
			if ($response > 99)
			{
				$response = 99;
			}
			#print $response."\n";
			print progress_bar( $response, 100, 40, '=' );
			
			usleep(200000);
		}
		if ($success == 0)
		{
			print "\n\033[31mTimed out waiting for program to finish.\033[0m\n";
		}
	}
	else
	{
		usleep(100000);
		atomd_kill_promt($socket);
	
		#print "Sending Node_ClearHex\n";
		atomd_send_command($socket, "Node_ClearHex");
		atomd_kill_promt($socket);
	
		my $linenums = 0;
		foreach $filerow (@filecontents) 
		{
			#print "Sending Node_AppendHex $filerow \n";
			#$filerow =~ s/^\s+//;
			#$filerow =~ s/\s+$//;
			atomd_send_command($socket, "Node_AppendHex $filerow");
			atomd_kill_promt($socket);
			$linenums += 1;
		}
		#print "Sending Node_ProgramHex $hwid $bios $linenums \n";
		atomd_send_command($socket, "Node_ProgramHex $hwid $bios $linenums");

		print atomd_read_command_response($socket);

#		my $result = system("atomic -s $hostname -p $port -c \"Node_Program $hwid $bios $filename\"");
#		exit($result);
	}
}

exit(0);

# wget-style. routine by tachyon
# at http://tachyon.perlmonk.org/
sub progress_bar {
    my ( $got, $total, $width, $char ) = @_;
    $width ||= 25; $char ||= '=';
    my $num_width = length $total;
    sprintf "|%-${width}s| (%.2f%%)\r", 
        $char x (($width-1)*$got/$total). '>', 
        $got, $total, 100*$got/+$total;
}

