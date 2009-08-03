#!/usr/bin/perl -w

$debugmode = 0;

#$outputpath = "outp/";
#$linenrpath = "linenr.txt";
#$contentspath = "contents.txt";
#$currentfilepath = "currentfile.txt";
#$logfilepath = "#hobby_small.log";
#$contentsfileoutput = "outp/";
#$contentsfilename = "hobby.txt";
$contentsheader = "====== Log of #hobby-channel ======\n";
#$namespace = "common:irc:hobby:";

#require "settings.pl";
use File::Copy;

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

if ($numArgs == 2) {
	if ($ARGV[0] eq "-s") {
		require "$ARGV[1]";
	}
}

$startAtLine = 0;
$currentfile = "";

$fileopenerr = 0;
open (LINENR, "<".$linenrpath) or $fileopenerr = 1;
if ($fileopenerr == 0) {
	$startAtLine = <LINENR>;
	if ($startAtLine =~ /\D/) {
		print "Not a number: $startAtLine\n";
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

@colors = ("#2F4F4F","#696969","#A0522D","#D2691E","#DAA520","#191970","#0000CD","#4682B4","#7FEFD4","#556B2F","#008B8B","#808000","#2E8B57","#32CD32","#483D8B","#6A5ACD","#8A2BE2","#BDB76B","#EF4500","#C71585","#8B0000","#DC143C","#E9967A","#CD5C5C","#B0E0E6","#E0E0C0");
$nrofcolors = @colors;

$linecnt = 0;
while (<LOGFILE>) {
	#print $linecnt ." ";
	$line = $_;
	#$store = "";
	if ($linecnt >= $startAtLine) {
		if ($line =~ m/^Session Time:/ || $line =~ m/^Session Start:/) {
			#print $line;
			$datetimenice = $line;
			$datetimenice =~ s/Session Time: //g;
			$datetimenice =~ s/Session Start: //g;
			$datetimenice =~ s/\n//g;
			$datetimenice =~ s/\r//g;
			
			$datetime = $line;
			$datetime =~ s/Session Time: //g;
			$datetime =~ s/Session Start: //g;
			$datetime =~ s/://g;
			$datetime =~ s/\n//g;
			$datetime =~ s/\r//g;
			$datetime =~ s/ /_/g;
			$datetime = lc($datetime);

			if ($currentfile) {
				if ($debugmode == 0) {
					print OUTFILE "\n\\\\\nNext page: [[".$datetime."]] \\\\ \n";
					print OUTFILE "Back to index: [[".$namespace."]] \\\\ \n";
					close(OUTFILE);
					
					$fileopenerr = 0;
					copy($outputpath.$currentfile, $outputpath."yesterday.txt") or $fileopenerr = 1;
					if ($fileopenerr == 1) {
					        print "Could not copy yesterday.txt-log";
					}
				}				
			}

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
			if ($currentfile) {
				if ($debugmode == 0) {
					print OUTFILE "====== ".$datetimenice." ======\n";
				}				
			}
			#print $datetime;
		}
		
		elsif ($currentfile) {
			$parsedLine = $line;
			$parsedLine =~ s/\n//g;
			$parsedLine =~ s/\r//g;
			#$parsedLine = $parsedLine;

			#<color #aaaaaa>
			#</color>	
			
			$nickLine = $parsedLine;
			if ($nickLine =~ m/.*<(\w{1,9})>.*/) {
				$nick = $1;
				$nickVal = 0;
				my @chars = split //, $nick;
				foreach my $char (@chars) {
				#for ($i = 0; $i < length($nick); $i++) {
					#$charval = ord(substr($nick, $i, 1));
					$nickVal += ord($char);
					#$nickVal += $charval;
					
				}
				#$temp = $nickVal;
				$nickVal = ($nickVal) % $nrofcolors;
				#$nickB = sprintf("%x", ((($nickVal & 7)**2)*2.3+5));
				#$nickG = sprintf("%x", (((($nickVal >> 3) & 7)**2.3)*2+5));
				#$nickR = sprintf("%x", (((($nickVal >> 6) & 7)**2.3)*2+5));
				#if (length($nickB) == 1) {
				#	$nickB = "0".$nickB;
				#}
				#if (length($nickG) == 1) {
				#	$nickG = "0".$nickG;
				#}
				#if (length($nickR) == 1) {
				#	$nickR = "0".$nickR;
				#}
				
				#print "$nick $temp $nickVal $colors[$nickVal] \n";
				#$color = "#$nickR$nickG$nickB";
				$parsedLine =~ s/<$nick>/<color $colors[$nickVal]><$nick><\/color>/;
				#print $nick." ".$nickVal."\n";
				#print $parsedLine ."\n";
			}
			
			#$parsedLine =~ s/\/\//\//g;
			$parsedLine =~ s/''/'/g;
			$parsedLine =~ s/\[\[/\[/g;
			$parsedLine =~ s/\]\]/\]/g;
			$parsedLine =~ s/\(\(/\(/g;
			$parsedLine =~ s/\)\)/\)/g;
			$parsedLine =~ s/____/_/g;
			$parsedLine =~ s/___/_/g;
			$parsedLine =~ s/__/_/g;
			$parsedLine =~ s/\*\*/\*/g;

			$parsedLine =~ s/\x{e5}/å/g;
			$parsedLine =~ s/\x{e4}/ä/g;
			$parsedLine =~ s/\x{f6}/ö/g;
			$parsedLine =~ s/\x{c5}/Å/g;
			$parsedLine =~ s/\x{c4}/Ä/g;
			$parsedLine =~ s/\x{d6}/Ö/g;

			$parsedLine =~ s/\[wiki\]/**[wiki]**/g;
			$parsedLine =~ s/\[svn\]/**[svn]**/g;

			$parsedLine .= " ";
			$parsedLine =~ s/http:\/\/projekt\.auml\.se\/(\S+)/[[:$1]] /g;
			
			$parsedLine .= "\\\\"."\n";
			if ($debugmode == 0) {
				print OUTFILE $parsedLine;
			}
		}
		#print $store;
		
	}
	$linecnt++;
}
close(OUTFILE);

$fileopenerr = 0;
copy($outputpath.$currentfile, $outputpath."today.txt") or $fileopenerr = 1;
if ($fileopenerr == 1) {
	print "Could not copy today.txt-log";
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
