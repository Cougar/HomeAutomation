#!/usr/bin/perl

# Written by Mattias Runge 2008

use XML::Twig;
use Getopt::Long;

$binaryname = "makeCanId.pl";
if (@ARGV > 0)
{
	GetOptions(	"xmlfile=s" 	=> \$xmlfile,
			"cfile=s" 	=> \$cfile);
}
else
{
	$help = 1;
}

if ($help)
{
	print "Usage ./$binaryname [options]\n";
	print "Options:\n";
	print "  -x <xmlfile>			Choose xmlfile to parse\n";
	print "  -c <headerfile> 		Choose header file to write\n";
	print "  -h 				Shows this usage\n";
	print "\n";
	print "example: ./$binaryname -x canId.xml -c canId.h\n";

	exit 0;
}

my $now = localtime time;
my $twig = XML::Twig->new();

$twig->parsefile($xmlfile);

my $root = $twig->root;

open (MYFILE, '>>' . $cfile);

print MYFILE "// Built on $now by makeCanIdHeader.pl\n\n";

print MYFILE "//------------------ //\n";
print MYFILE "// Class definitions //\n";
print MYFILE "//------------------ //\n";
print MYFILE "\n";

foreach my $class ($root->first_child('classes')->children('class'))
{
	my $className = "\U" . $class->att('name');
	$className =~ tr/a-z/A-Z/;
	print MYFILE "#define CAN_MODULE_CLASS_$className = " . $class->att('id') . "\n";
}

print MYFILE "\n";
print MYFILE "//-------------------- //\n";
print MYFILE "// Command definitions //\n";
print MYFILE "//-------------------- //\n";
print MYFILE "\n";

foreach my $command ($root->first_child('commands')->children('command'))
{
	my $commandName = "\U" . $command->att('name');
	$commandName =~ tr/a-z/A-Z/;
	my $commandType = "\U" . $command->att('type');
	$commandType =~ tr/a-z/A-Z/;
	print MYFILE "#define CAN_MODULE_CMD_" . $commandType . "_" . $commandName . " = " . $command->att('id') . "\n";
}

print MYFILE "\n";
print MYFILE "//------------------- //\n";
print MYFILE "// Module definitions //\n";
print MYFILE "//------------------- //\n";
print MYFILE "\n";

foreach my $module ($root->first_child('modules')->children('module'))
{
	my $className = "\U" . $module->att('class');
	$className =~ tr/a-z/A-Z/;
	my $moduleName = "\U" . $module->att('name');
	$moduleName =~ tr/a-z/A-Z/;
	print MYFILE "#define CAN_MODULE_TYPE_" . $className . "_" . $moduleName . " = " . $module->att('id') . "\n";
}

print MYFILE "\n";
print MYFILE "//----------------- //\n";
print MYFILE "// Misc definitions //\n";
print MYFILE "//----------------- //\n";
print MYFILE "\n";

foreach my $define ($root->first_child('defines')->children('define'))
{
	my $groupName = "\U" . $define->att('group');
	$groupName =~ tr/a-z/A-Z/;
	my $defineName = "\U" . $define->att('name');
	$defineName =~ tr/a-z/A-Z/;
	print MYFILE "#define " . $groupName . "_" . $defineName . " = " . $define->att('id') . "\n";
}

print MYFILE "\n";

close (MYFILE);

print "Successfully wrote " . $cfile . "\n";

