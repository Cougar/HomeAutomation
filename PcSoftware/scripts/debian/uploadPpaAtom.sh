#!/bin/bash

ATOMDIR=/tmp/atomppa
SVNURL=http://svn.arune.se/svn/HomeAutomation/trunk/PcSoftware/Atom1.5
SVNURLCNF=http://svn.arune.se/svn/HomeAutomation/trunk/Configuration
SVNDIR=AtomSvn
SVNDIRCNF=ConfigSvn
CNFDIR=Configuration
REVFILE=./AtomSvnRev
PPA=ppa:arune/auml-atom-daily

RUNDIR=`dirname $0`

CURRREV=`svn info $SVNURL |grep '^Revision:' | sed -e 's/^Revision: //'`
NOSVNDIR=atom-1.5.$CURRREV
ORIGTAR=atom_1.5.$CURRREV.orig.tar.gz
SRCCHFILE=atom_1.5.$CURRREV\_source.changes

# Run script from cron every five minutes
# script will check revision of Atom in SVN and if there is a new version the script will
# download it and create a complete debian source package and upload to launchpad as a ppa


#temporary during development of script
rm -rf $ATOMDIR
rm $REVFILE

# Check if script is already running
if [ -d $ATOMDIR ]; then
  echo "Error: Already running?"
  exit 1
fi

if [ -e $REVFILE ]; then
  # If revision file exists, check current revision
  OLDREV=`cat $REVFILE`
  if [ "$CURRREV" -le "$OLDREV" ]; then
    echo "Error: No new svn revision."
    exit 1
  fi
fi

mkdir $ATOMDIR

# Checkout atom and configuration
svn co $SVNURL $ATOMDIR/$SVNDIR
svn co $SVNURLCNF $ATOMDIR/$SVNDIRCNF

# Export atom to get rid of .svn subfolders
svn export $ATOMDIR/$SVNDIR $ATOMDIR/$NOSVNDIR
# Copy configuration folder
svn export $ATOMDIR/$SVNDIRCNF $ATOMDIR/$NOSVNDIR/$CNFDIR

# Change install-target for data.xml file in CMakeList.txt
###################################################################### TODO!!!

# Create change log file
SVNLOG=`svn log -r $CURRREV $SVNURL`
###################################################################### TODO!!!

# Create orig tar gz
tar -zcf $ATOMDIR/$ORIGTAR $ATOMDIR/$NOSVNDIR

# Create source package with debuild
#cd $ATOMDIR/$NOSVNDIR
#debuild -S

#cd $ATOMDIR
#dput $PPA $SRCCHFILE

echo "the end $RUNDIR"

# Remove all files
#rm -rf $ATOMDIR

# Write the current svn revision to file
echo -n $CURRREV > $RUNDIR/$REVFILE
