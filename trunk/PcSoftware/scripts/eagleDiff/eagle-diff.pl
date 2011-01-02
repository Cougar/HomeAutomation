#!/usr/bin/perl
###########################################################
# Eagle diff script
# Author: Anders Runeson, 2010-07-18, arune @ EFnet
###########################################################


use File::Basename;
use File::Copy;

#setup
#location of imagemagick compare binary
my $compare = "/usr/bin/compare";
#location of eagle binary
my $eagle = "/usr/bin/eagle";
#where to store temporary files
my $tmpdir = "/tmp";
#the dpi parameter to send to eagle for generating images
my $brdresolution = 800;
my $schresolution = 150;
#store the resulting images at the path of argument 1 or 2
my $storeresult=2;

#name of eagle script file, will be created
my $eaglecommands=$tmpdir."/eaglecmd";

#check number of arguments, must be 2
my $argcount = @ARGV;
unless ($argcount eq "2") {
        print "Syntax: perl eagle-compare <file1> <file2>\n";
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
    exit;
  }
unless (-e $file2)
  {
    print "File '$file2' does not exist.\n";
    exit;
  }
#check that both files have the same suffix
unless ($suffix1 eq $suffix2)
  {
    print "File '$file1' does not have the same file type as '$file2'.\n";
    exit;
  }
#check that suffix is a supported one
unless (($suffix1 eq ".brd") || ($suffix1 eq ".sch"))
  {
    print "Only .brd and .sch are supported.\n";
    exit;
  }

my $storedir=$tmpdir;
if ($storeresult == 1)
{
	$storedir = dirname($file1);
}
if ($storeresult == 2)
{
	$storedir = dirname($file2);
}

#create a file name random, the file names will be too long if they are created with the path
$file1unique = $tmpdir."/".generate_random_string(8).$suffix1;
$file2unique = $tmpdir."/".generate_random_string(8).$suffix1;

#copy files to temp area
copy($file1, $file1unique);
copy($file2, $file2unique);

#if suffix is a layout file
if ($suffix1 eq ".brd")
{
	#create eagle script file for first file argument
	open (MYFILE,">".$eaglecommands);
	print MYFILE "SET VECTOR_FONT ON\n";
	print MYFILE "DISPLAY ALL\n";
	print MYFILE "RIPUP @\n";
	print MYFILE "DISPLAY NONE 17 18 ?? 1\n";
	print MYFILE "EXPORT IMAGE ".$file1unique.".1.png ".$brdresolution."\n";
	print MYFILE "DISPLAY NONE 17 18 ?? 2\n";
	print MYFILE "EXPORT IMAGE ".$file1unique.".2.png ".$brdresolution."\n";
	print MYFILE "DISPLAY NONE 17 18 ?? 15\n";
	print MYFILE "EXPORT IMAGE ".$file1unique.".15.png ".$brdresolution."\n";
	print MYFILE "DISPLAY NONE 17 18 ?? 16\n";
	print MYFILE "EXPORT IMAGE ".$file1unique.".16.png ".$brdresolution."\n";
	print MYFILE "QUIT\n";
	close (MYFILE);
	
	#run eagle with the first layout file and the created script file as arguments
	#this will create 4 png files in the temp dir
	system ($eagle, $file1unique, "-S", $eaglecommands);

	#create eagle script file for second file argument
	open (MYFILE,">".$eaglecommands);
	print MYFILE "SET VECTOR_FONT ON\n";
	print MYFILE "DISPLAY ALL\n";
	print MYFILE "RIPUP @\n";
	print MYFILE "DISPLAY NONE 17 18 ?? 1\n";
	print MYFILE "EXPORT IMAGE ".$file2unique.".1.png ".$brdresolution."\n";
	print MYFILE "DISPLAY NONE 17 18 ?? 2\n";
	print MYFILE "EXPORT IMAGE ".$file2unique.".2.png ".$brdresolution."\n";
	print MYFILE "DISPLAY NONE 17 18 ?? 15\n";
	print MYFILE "EXPORT IMAGE ".$file2unique.".15.png ".$brdresolution."\n";
	print MYFILE "DISPLAY NONE 17 18 ?? 16\n";
	print MYFILE "EXPORT IMAGE ".$file2unique.".16.png ".$brdresolution."\n";
	print MYFILE "QUIT\n";
	close (MYFILE);

	#run eagle with the second layout file and the created script file as arguments
	#this will create 4 png files in the temp dir
	system ($eagle, $file2unique, "-S", $eaglecommands);
	
	#compare each png file for the first file argument and the second file argument
	#the resulting compared images will be stored from where the script is run
	system ($compare, $file1unique.".1.png", $file2unique.".1.png", $storedir."/eagle-diff-brd-layer1.png");
	system ($compare, $file1unique.".2.png", $file2unique.".2.png", $storedir."/eagle-diff-brd-layer2.png");
	system ($compare, $file1unique.".15.png", $file2unique.".15.png", $storedir."/eagle-diff-brd-layer15.png");
	system ($compare, $file1unique.".16.png", $file2unique.".16.png", $storedir."/eagle-diff-brd-layer16.png");
	
	#remove all intermediate files
	unlink($file1unique.".1.png");
	unlink($file1unique.".2.png");
	unlink($file1unique.".15.png");
	unlink($file1unique.".16.png");
	unlink($file2unique.".1.png");
	unlink($file2unique.".2.png");
	unlink($file2unique.".15.png");
	unlink($file2unique.".16.png");
	unlink($eaglecommands);
}


if ($suffix1 eq ".sch")
{
	#create eagle script file for first file argument
	open (MYFILE,">".$eaglecommands);
	#print MYFILE "SET VECTOR_FONT ON\n";	# causes a save-dialog-box if not already persistent in this drawing
	print MYFILE "DISPLAY NONE 91 92 94 95 96 97 98\n";
	print MYFILE "EXPORT IMAGE ".$file1unique.".png ".$schresolution."\n";
	print MYFILE "QUIT\n";
	close (MYFILE);

	#run eagle with the first schematic file and the created script file as arguments
	#this will create one png file in the temp dir
	system ($eagle, $file1unique, "-S", $eaglecommands);

	#create eagle script file for second file argument
	open (MYFILE,">".$eaglecommands);
	#print MYFILE "SET VECTOR_FONT ON\n";	# causes a save-dialog-box if not already persistent in this drawing
	print MYFILE "DISPLAY NONE 91 92 94 95 96 97 98\n";
	print MYFILE "EXPORT IMAGE ".$file2unique.".png ".$schresolution."\n";
	print MYFILE "QUIT\n";
	close (MYFILE);

	#run eagle with the second schematic file and the created script file as arguments
	#this will create one png file in the temp dir
	system ($eagle, $file2unique, "-S", $eaglecommands);
	
	#compare png file for the first file argument and the second file argument
	#the resulting compared image will be stored from where the script is run
	system ($compare, $file1unique.".png", $file2unique.".png", $storedir."/eagle-diff-sch.png");
	
	#remove all intermediate files
	unlink($file1unique.".png");
	unlink($file2unique.".png");
	unlink($eaglecommands);

}

unlink($file1unique);
unlink($file2unique);



# This function generates random strings of a given length
sub generate_random_string
{
	my $length_of_randomstring=shift;# the length of 
			 # the random string to generate

	my @chars=('a'..'z','A'..'Z','0'..'9','_');
	my $random_string;
	foreach (1..$length_of_randomstring) 
	{
		# rand @chars will generate a random 
		# number between 0 and scalar @chars
		$random_string.=$chars[rand @chars];
	}
	return $random_string;
}

