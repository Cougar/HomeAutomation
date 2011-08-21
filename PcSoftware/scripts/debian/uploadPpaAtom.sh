#!/bin/bash

ATOMDIR=/tmp/atomppa
SVNURL=http://svn.arune.se/svn/HomeAutomation/trunk/PcSoftware/Atom1.5
SVNURLCNF=http://svn.arune.se/svn/HomeAutomation/trunk/Configuration
SVNDIR=AtomSvn
SVNDIRCNF=ConfigSvn
CNFDIR=Configuration
PPA=ppa:arune/auml-atom-daily
DEBUILDFLG="-S -sa -k7EA270B4"
PKGNAME=atom
DIST="maverick natty"
PKGMAINT="Anders Runeson <runeson@gmail.com>"

RUNDIR=`dirname $0`
CHLOGDATE=`date -R`
REVFILE=$RUNDIR/AtomSvnRev
TMPFILE=/tmp/atompparunning

CURRREV=`svn info $SVNURL |grep '^Last Changed Rev:' | sed -e 's/^Last Changed Rev: //'`
VERSION=1.5.$CURRREV
NOSVNDIR=atom-$VERSION
ORIGTAR=atom_$VERSION.orig.tar.gz
SRCCHFILE=atom_$VERSION\_source.changes
DPUTFLG="auml-atom-daily $SRCCHFILE"

# Run script from cron every hour
# script will check revision of Atom in SVN and if there is a new version the script will
# download it and create a complete debian source package and upload to launchpad as a ppa

# Newline and date in log
echo ""
date

# Run next line to force, even if no new svn revision
#rm $REVFILE

# Check if script is already running
if [ -e $TMPFILE ]; then
  echo "Error: Already running?"
  exit 1
fi

# Check that revision variable is set
if [ -z $CURRREV ]; then
  echo "Error: Could not find SVN revision"
  exit 1
fi

# Check that revision is sane
if [ ! $CURRREV -gt 0 ]; then
  echo "Error: SVN revision is not greater than 0"
  exit 1
fi

# Set old revision, in case no file is found
OLDREV=$CURRREV
if [ -e $REVFILE ]; then
  # If revision file exists, check current revision
  OLDREV=`cat $REVFILE`
  if [ "$CURRREV" -le "$OLDREV" ]; then
    echo "Error: No new svn revision."
    exit 1
  fi
fi

# Temporary during development of script, this is needed if tmp dir is kept at the end of the script
rm -rf $ATOMDIR

# Now running script
touch $TMPFILE

mkdir $ATOMDIR

# Export atom to get rid of .svn subfolders
svn export $SVNURL $ATOMDIR/$NOSVNDIR
# Export configuration folder into the exported Atom-folder
svn export $SVNURLCNF $ATOMDIR/$NOSVNDIR/$CNFDIR

# Add a svn revision file, atom need the svn revision when building
echo -n $CURRREV > $ATOMDIR/$NOSVNDIR/AtomSvnRev

# Change install-target for data.xml file in CMakeList.txt, not needed, done in CMakeList.txt
#cat $ATOMDIR/$NOSVNDIR/CMakeLists.txt | sed -e 's/\/..\/..\/Configuration\/data.xml/\/Configuration\/data.xml/' > $ATOMDIR/$NOSVNDIR/CMakeLists.txt

# Create the debian changelog file from svn log
echo "$PKGNAME ($VERSION) $DIST; urgency=low" > $ATOMDIR/$NOSVNDIR/debian/changelog
echo "" >> $ATOMDIR/$NOSVNDIR/debian/changelog
OLDIFS="$IFS"
IFS=$'\n'
REVCNT=$CURRREV
# Loop through the revisions
while [ $REVCNT -gt $OLDREV ]; do
  # Add change entries by reading log from svn
  SVNLOG=`svn log -r $REVCNT $SVNURL`
  for LOGENTRY in $SVNLOG; do
    if [[ ! $LOGENTRY == *--------------* ]]; then
      echo "  * $LOGENTRY" >> $ATOMDIR/$NOSVNDIR/debian/changelog
    fi
  done
  let REVCNT=REVCNT-1
done
IFS="$OLDIFS"
echo "" >> $ATOMDIR/$NOSVNDIR/debian/changelog
echo " -- $PKGMAINT  $CHLOGDATE" >> $ATOMDIR/$NOSVNDIR/debian/changelog

# Create orig tar gz
cd $ATOMDIR
tar -zcf $ORIGTAR $NOSVNDIR

# Create source package with debuild
cd $ATOMDIR/$NOSVNDIR
debuild $DEBUILDFLG

# Upload to launchpad with dput
cd $ATOMDIR
dput $DPUTFLG
# Requires ubuntu 9.10 or newer
#dput $PPA $SRCCHFILE

# Remove all temporary files
#rm -rf $ATOMDIR

# Write the current svn revision to file
echo -n $CURRREV > $REVFILE

cd $RUNDIR

# Remove temprary running-file
rm $TMPFILE
