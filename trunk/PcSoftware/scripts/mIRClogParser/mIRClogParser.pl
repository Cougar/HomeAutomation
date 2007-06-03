#!/usr/bin/perl -w

use Encode;
    
$debugmode = 0;

$outputpath = "H:/shared/www/projekt/data/pages/common/irc/hobby/";
$linenrpath = "linenr.txt";
$contentspath = "contents.txt";
$currentfilepath = "currentfile.txt";
$logfilepath = "C:/Prgm/mIRC/logs/EFnet/#hobby.log";
$contentsfileoutput = "H:/shared/www/projekt/data/pages/common/irc/";
$contentsfilename = "hobby.txt";
$contentsheader = "====== Log of #hobby-channel ======\n";
$namespace = "common:irc:hobby:";

$numArgs = $#ARGV + 1;
if ($numArgs == 1) {
	if ($ARGV[0] eq "-r") {
		#reset log system
		
		$fileopenerr = 0;
		open (LINENR, ">".$linenrpath) or $fileopenerr = 1;
		if ($fileopenerr == 0) {
			print LINENR "0";
			close(LINENR);
		}
		
		$fileopenerr = 0;
		open (CONTENTSFILE, ">".$contentspath) or $fileopenerr = 1;
		if ($fileopenerr == 0) {
			print CONTENTSFILE "";
			close(CONTENTSFILE);
		}
		close(CONTENTSFILE);

		$fileopenerr = 0;
		open (CURRFILE, ">".$currentfilepath) or $fileopenerr = 1;
		if ($fileopenerr == 0) {
			print CURRFILE "";
			close(CURRFILE);
		}		
		#print "exit\n";
		exit;
	}
}


$startAtLine = 0;
$currentfile = "";

$fileopenerr = 0;
open (LINENR, "<".$linenrpath) or $fileopenerr = 1;
if ($fileopenerr == 0) {
	$startAtLine = <LINENR>;
	if ($startAtLine =~ /\D/) {
		print "Not a number\n";
		exit;
	} 
	close(LINENR);
} else {
	print "Could not open ".$linenrpath;
	exit;
}

$fileopenerr = 0;
open (CURRFILE, "<".$currentfilepath) or $fileopenerr = 1;
if ($fileopenerr == 0) {
	$currentfile = <CURRFILE>;
	if (!$currentfile && $startAtLine != 0) {
		print "Current file not ok\n";
		exit;
	} 
	close(CURRFILE);
} else {
	print "Could not opeen ".$currentfilepath;
	if ($startAtLine != 0) { 
		exit; 
	}
}

$fileopenerr = 0;
open (LOGFILE, "<".$logfilepath) or $fileopenerr = 1;
if ($fileopenerr == 1) {
	print "Could not open ".$logfilepath;
	exit;
}

if ($currentfile) {
	if ($debugmode == 0) {
		$fileopenerr = 0;
		open (OUTFILE, ">>".$outputpath.$currentfile) or $fileopenerr = 1;
		if ($fileopenerr == 1) {
			print "Could not open ".$outputpath."/".$currentfile;
			exit;
		}
	}
}

$linecnt = 0;
while (<LOGFILE>) {
	#print $linecnt ." ";
	$line = $_;
	#$store = "";
	if ($linecnt >= $startAtLine) {
		if ($line =~ m/^Session Time:/) {
			#print $line;
			$datetimenice = $line;
			$datetimenice =~ s/Session Time: //g;
			$datetimenice =~ s/\n//g;
			$datetimenice =~ s/\r//g;
			
			$datetime = $line;
			$datetime =~ s/Session Time: //g;
			$datetime =~ s/://g;
			$datetime =~ s/\n//g;
			$datetime =~ s/\r//g;
			$datetime =~ s/ /_/g;
			$datetime = lc($datetime);

			$currentfile = $datetime .".txt";
			# skriv $datetimenice och $datetime till $contentspath
			$fileopenerr = 0;
			open (CONTENTSFILE, "<".$contentspath) or $fileopenerr = 1;
			if ($fileopenerr == 0) {
				@contentsfilecontents = <CONTENTSFILE>;
				close(CONTENTSFILE);
				if ($debugmode == 0) {
					open (CONTENTSFILE, ">".$contentspath) or $fileopenerr = 1;
					if ($fileopenerr == 0) {
						print CONTENTSFILE "[[$namespace$datetime|$datetimenice]] \\\\ \n";
						foreach (@contentsfilecontents) {
							print CONTENTSFILE $_;
						}
						close(CONTENTSFILE);
					}
				}
			} else {
				exit;
			}

			if ($debugmode == 0) {
				$fileopenerr = 0;
				open (OUTFILE, ">>".$outputpath.$currentfile) or $fileopenerr = 1;
				if ($fileopenerr == 1) {
					print "Could not open ".$outputpath."/".$currentfile;
					exit;
				}
			}

			#print $datetime;
		}
		
		if ($currentfile) {
			$parsedLine = $line;
			$parsedLine =~ s/\n//g;
			$parsedLine =~ s/\r//g;
			#$parsedLine = $parsedLine;

			$nickLine = $parsedLine;
			if ($nickLine =~ m/.*<(\w{1,9})>.*/) {
				$nick = $1;
				$nickVal = 0;
				for ($i = 0; $i < length($nick); $i++) {
					$nickVal += ord(substr($nick, $i, 1));
				}
				$nickVal = ($nickVal) % 512;
				$nickB = sprintf("%x", ((($nickVal & 7)**2)*2.3+5));
				$nickG = sprintf("%x", (((($nickVal >> 3) & 7)**2.3)*2+5));
				$nickR = sprintf("%x", (((($nickVal >> 6) & 7)**2.3)*2+5));
				if (length($nickB) == 1) {
					$nickB = "0".$nickB;
				}
				if (length($nickG) == 1) {
					$nickG = "0".$nickG;
				}
				if (length($nickR) == 1) {
					$nickR = "0".$nickR;
				}
				
				#print "$nick $nickR$nickG$nickB\n";
				$color = "#$nickR$nickG$nickB";
				$parsedLine =~ s/<$nick>/<color $color><$nick><\/color>/;
				#print $nick." ".$nickVal."\n";
				#print $parsedLine ."\n";
			}
			
			$parsedLine =~ s/\x{e5}/å/g;
			$parsedLine =~ s/\x{e4}/ä/g;
			$parsedLine =~ s/\x{f6}/ö/g;
			$parsedLine =~ s/\x{c5}/Å/g;
			$parsedLine =~ s/\x{c4}/Ä/g;
			$parsedLine =~ s/\x{d6}/Ö/g;

			$parsedLine =~ s/\[wiki\]/**[wiki]**/g;
			$parsedLine =~ s/\[svn\]/**[svn]**/g;
			
			$parsedLine .= "\\\\"."\n";
			if ($debugmode == 0) {
				print OUTFILE $parsedLine;
			}
		}
		#print $store;
		
	}
	$linecnt++;
}

if ($debugmode == 0) {
	$fileopenerr = 0;
	open (LINENR, ">".$linenrpath) or $fileopenerr = 1;
	if ($fileopenerr == 0) {
		print LINENR $linecnt;
		close(LINENR);
	}
}

if ($debugmode == 0) {
	$fileopenerr = 0;
	open (CURRFILE, ">".$currentfilepath) or $fileopenerr = 1;
	if ($fileopenerr == 0) {
		print CURRFILE $currentfile;
		close(CURRFILE);
	} else {
		print "Could not open ".$currentfilepath;
		exit;
	}
}

if ($debugmode == 0) {
	$fileopenerr = 0;
	open (CONTENTSFILE, "<".$contentspath) or $fileopenerr = 1;
	if ($fileopenerr == 0) {
		open (NICECONTENTSFILE, ">".$contentsfileoutput.$contentsfilename) or $fileopenerr = 1;
		if ($fileopenerr == 0) {
			@contentsfilecontents = <CONTENTSFILE>;
			print NICECONTENTSFILE $contentsheader;
			foreach (@contentsfilecontents) {
							print NICECONTENTSFILE $_;
			}
			close(NICECONTENTSFILE);
		} else {
			print "Could not open ".$contentsfileoutput.$contentsfilename;
			exit;
		}
		close(CONTENTSFILE);
	} else {
		print "Could not open ".$contentspath;
		exit;
	}
}

#print $linecnt-1 ." - ". $line;







