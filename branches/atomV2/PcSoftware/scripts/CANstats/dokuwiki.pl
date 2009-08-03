#!/usr/bin/perl

# 
# Post data to a page in DokuWiki, the page must be writeable without loggin
# Author: Anders Runeson, arune@users.sf.net
# 

require LWP::UserAgent;
use HTTP::Request::Common;

sub dokuwikipost 
{
	$url = $_[0]; $page = $_[1]; $wikitext = $_[2]; $summary = $_[3];
	$ua = LWP::UserAgent->new;
	$ua->request(POST $url, [ 
	"id" 		=> $page,		"rev" 		=> '', 
	"date"		=> '', 			"prefix"	=> '', 
	"suffix"	=> '', 			"wikitext"	=> $wikitext,
	"summary"	=> $summary,	  	"do[save]"	=> 'Save'  
	]);
}

#dokuwikipost("http://projekt.auml.se/doku.php", "homeautomation:livestats:arune", "texten att posta", "med summary");
#dokuwikipost("http://www.arune.se/temp/post.php", "homeautomation:livestats:arune", "texten att posta", "med summary");
