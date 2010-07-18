#!/usr/bin/perl

use File::Basename;
use Cwd 'abs_path';

$defaultbin = "/usr/bin/meld";

@extensionbin = (
		"brd", "/home/arune/Documents/HomeAutomation/PcSoftware/scripts/eagleDiff/eagle-diff.pl", 
		"sch", "/home/arune/Documents/HomeAutomation/PcSoftware/scripts/eagleDiff/eagle-diff.pl"
		);

#check number of arguments, must be 2
my $argcount = @ARGV;
if (not $argcount eq "2") {
        print "Syntax: compare-bin-chooser.pl <file1> <file2>\n";
        exit;
}

#read arguments and find file suffix
my $file1 = $ARGV[0];
my $file2 = $ARGV[1];
my(undef, undef, $suffix1) = fileparse($file1,qr"\..*");
my(undef, undef, $suffix2) = fileparse($file2,qr"\..*");

#check that files exists
unless (-e $file1)
  {
    print "File '$file1' does not exist.\n";
  }
unless (-e $file2)
  {
    print "File '$file2' does not exist.\n";
  }
#check that both files have the same suffix
unless ($suffix1 eq $suffix2)
  {
    print "File '$file1' does not have the same file type as '$file2'.\n";
  }

my $extbinlen = @extensionbin;
my $foundbin = false;
for (my $i; $i < $extbinlen; $i +=2)
{
	if ($suffix1 eq ".".@extensionbin[$i])
	{
		unless (-e @extensionbin[$i+1])
		{
			print "Binary ".@extensionbin[$i+1]." does not exist.\n";
		}
		system (@extensionbin[$i+1], $file1, $file2);
		$foundbin = true;
	}
}
if ($foundbin eq false)
{
	system ($defaultbin, $file1, $file2);
}


