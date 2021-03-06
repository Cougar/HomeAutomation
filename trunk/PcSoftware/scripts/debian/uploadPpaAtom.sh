#!/bin/bash

if [ $# -lt 1 ]
then
  echo "Error: need to run script with ubuntu releasename as argument"
  exit 1
fi

#exit 1

ATOMDIR=/tmp/atomppa
SVNURL=https://svn.auml.se/svn/HomeAutomation/trunk/PcSoftware/Atom1.5
SVNURLCNF=https://svn.auml.se/svn/HomeAutomation/trunk/Configuration
SVNDIR=AtomSvn
SVNDIRCNF=ConfigSvn
CNFDIR=Configuration
PPA=ppa:arune/auml-atom-daily
DEBUILDFLG="-S -sa -k7EA270B4"
PKGNAME=atom
PKGMAINT="Anders Sandblad <runeson@gmail.com>"

# Dist name from command line argument
DIST=$1
RUNDIR=`dirname $0`
CHLOGDATE=`date -R`
REVFILE=$RUNDIR/AtomSvnRev-$DIST
TMPFILE=/tmp/atompparunning

CURRREV=`svn info $SVNURL |grep '^Last Changed Rev:' | sed -e 's/^Last Changed Rev: //'`
VERSION=1.5.${CURRREV}ppa1~${DIST}
# Dir name of atom svn export (no svn folders)
NOSVNDIR=atom-$VERSION
# File name to use for tarball with original source, NOSVNDIR will be compressed to this file
ORIGTAR=atom_$VERSION.orig.tar.gz
# File with source changes (?)
SRCCHFILE=atom_$VERSION\_source.changes
# Dput is called with these flags
DPUTFLG="auml-atom-daily $SRCCHFILE"

case $DIST in
  oneiric)
    DEPS="libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libboost-thread1.46.1, libboost-system1.46.1, libboost-signals1.46.1, libboost-filesystem1.46.1, libboost-date-time1.46.1, libboost-program-options1.46.1, libv8-3.1.8.22, libmysqlclient16"
    BDEPS="debhelper (>= 7.0.50~), cmake, libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libv8-dev (>= 2.2.18), libboost-date-time1.46-dev, libboost-filesystem1.46-dev, libboost-program-options1.46-dev, libboost-signals1.46-dev, libboost-thread1.46-dev, libreadline-dev, libx11-dev (>= 2:1.3), libmysqlclient-dev"
    ;;
  precise)
    DEPS="libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libboost-thread1.48.0, libboost-system1.48.0, libboost-signals1.48.0, libboost-filesystem1.48.0, libboost-date-time1.48.0, libboost-program-options1.48.0, libv8-3.7.12.22, libmysqlclient18"
    BDEPS="debhelper (>= 7.0.50~), cmake, libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libv8-dev (>= 2.2.18), libboost-date-time1.48-dev, libboost-filesystem1.48-dev, libboost-program-options1.48-dev, libboost-signals1.48-dev, libboost-thread1.48-dev, libreadline-dev, libx11-dev (>= 2:1.3), libmysqlclient-dev"
    ;;
  quantal)
    DEPS="libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libboost-thread1.49.0, libboost-system1.49.0, libboost-signals1.49.0, libboost-filesystem1.49.0, libboost-date-time1.49.0, libboost-program-options1.49.0, libv8-3.8.9.20, libmysqlclient18"
    BDEPS="debhelper (>= 7.0.50~), cmake, libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libv8-dev (>= 2.2.18), libboost-date-time1.49-dev, libboost-filesystem1.49-dev, libboost-program-options1.49-dev, libboost-signals1.49-dev, libboost-thread1.49-dev, libreadline-dev, libx11-dev (>= 2:1.3), libmysqlclient-dev"
    ;;
  raring)
    DEPS="libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libboost-thread1.53.0, libboost-system1.53.0, libboost-signals1.53.0, libboost-filesystem1.53.0, libboost-date-time1.53.0, libboost-program-options1.53.0, libv8-3.8.9.20, libmysqlclient18"
    BDEPS="debhelper (>= 7.0.50~), cmake, libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libv8-dev (>= 2.2.18), libboost-date-time1.53-dev, libboost-filesystem1.53-dev, libboost-program-options1.53-dev, libboost-signals1.53-dev, libboost-thread1.53-dev, libreadline-dev, libx11-dev (>= 2:1.3), libmysqlclient-dev"
    ;;
  saucy)
    DEPS="libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libboost-thread1.54.0, libboost-system1.54.0, libboost-signals1.54.0, libboost-filesystem1.54.0, libboost-date-time1.54.0, libboost-program-options1.54.0, libv8-3.14.5, libmysqlclient18"
    BDEPS="debhelper (>= 7.0.50~), cmake, libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libv8-dev (>= 2.2.18), libboost-date-time1.54-dev, libboost-filesystem1.54-dev, libboost-program-options1.54-dev, libboost-signals1.54-dev, libboost-thread1.54-dev, libreadline-dev, libx11-dev (>= 2:1.3), libmysqlclient-dev"
    ;;
  trusty)
    DEPS="libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libboost-thread1.54.0, libboost-system1.54.0, libboost-signals1.54.0, libboost-filesystem1.54.0, libboost-date-time1.54.0, libboost-program-options1.54.0, libv8-3.14.5, libmysqlclient18, libx11-6"
    BDEPS="debhelper (>= 7.0.50~), cmake, libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libv8-dev (>= 2.2.18), libboost-date-time1.54-dev, libboost-filesystem1.54-dev, libboost-program-options1.54-dev, libboost-signals1.54-dev, libboost-thread1.54-dev, libreadline-dev, libx11-dev (>= 2:1.3), libmysqlclient-dev"
    ;;
  utopic)
    DEPS="libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libboost-thread1.55.0, libboost-system1.55.0, libboost-signals1.55.0, libboost-filesystem1.55.0, libboost-date-time1.55.0, libboost-program-options1.55.0, libv8-3.14.5, libmysqlclient18"
    BDEPS="debhelper (>= 7.0.50~), cmake, libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libv8-dev (>= 2.2.18), libboost-date-time1.55-dev, libboost-filesystem1.55-dev, libboost-program-options1.55-dev, libboost-signals1.55-dev, libboost-thread1.55-dev, libreadline-dev, libx11-dev (>= 2:1.3), libmysqlclient-dev"
    ;;
  xenial)
    DEPS="libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libboost-thread1.58.0, libboost-system1.58.0, libboost-signals1.58.0, libboost-filesystem1.58.0, libboost-date-time1.58.0, libboost-program-options1.58.0, libv8-3.14.5, libmysqlclient20, libx11-6"
    BDEPS="debhelper (>= 7.0.50~), cmake, libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libv8-dev (>= 2.2.18), libboost-date-time1.58-dev, libboost-filesystem1.58-dev, libboost-program-options1.58-dev, libboost-signals1.58-dev, libboost-thread1.58-dev, libreadline-dev, libx11-dev (>= 2:1.3), libmysqlclient-dev"
    ;;
esac

# Run script from cron every hour
# script will check revision of Atom in SVN and if there is a new version the script will
# download it and create a complete debian source package and upload to launchpad as a ppa

# Newline and date in log
echo ""
date

# Run next line to force, even if no new svn revision
#rm $REVFILE

# Check if script is already running
if [ -e $TMPFILE ]
then
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

echo "******** Exporting SVN ********"
# Export atom to get rid of .svn subfolders
svn export $SVNURL $ATOMDIR/$NOSVNDIR
# Export configuration folder into the exported Atom-folder
svn export $SVNURLCNF $ATOMDIR/$NOSVNDIR/$CNFDIR

echo "******** Creating debian dir ********"
mkdir -p $ATOMDIR/$NOSVNDIR/debian

echo "******** Adding SVN revision file ********"
# Add a svn revision file, atom need the svn revision when building
echo -n $CURRREV > $ATOMDIR/$NOSVNDIR/AtomSvnRev

# Change install-target for data.xml file in CMakeList.txt, not needed, done in CMakeList.txt
#cat $ATOMDIR/$NOSVNDIR/CMakeLists.txt | sed -e 's/\/..\/..\/Configuration\/data.xml/\/Configuration\/data.xml/' > $ATOMDIR/$NOSVNDIR/CMakeLists.txt

echo "******** Copying debian files ********"
cp $RUNDIR/files/atom.conf $ATOMDIR/$NOSVNDIR/debian/
cp $RUNDIR/files/compat $ATOMDIR/$NOSVNDIR/debian/
cp $RUNDIR/files/copyright $ATOMDIR/$NOSVNDIR/debian/
cp $RUNDIR/files/rules $ATOMDIR/$NOSVNDIR/debian/
cp $RUNDIR/files/control $ATOMDIR/$NOSVNDIR/debian/

echo "******** Set the correct depends and build depends ********"
# Set the correct depends and build depends, different for different distributions
sed -i "s/^Build-Depends/Build-Depends: ${BDEPS}/g" $ATOMDIR/$NOSVNDIR/debian/control
sed -i "s/^Depends/Depends: ${DEPS}/g" $ATOMDIR/$NOSVNDIR/debian/control

echo "******** Create debian changelog from SVN log ********"
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

echo "******** Create orig tar.gz ********"
# Create orig tar gz
cd $ATOMDIR
tar -zcf $ORIGTAR $NOSVNDIR

echo "******** Create source package with debuild ********"
# Create source package with debuild
cd $ATOMDIR/$NOSVNDIR
debuild $DEBUILDFLG

echo "******** Upload to launchpad with dput ********"
# Upload to launchpad with dput
cd $ATOMDIR
dput $DPUTFLG
# Requires ubuntu 9.10 or newer
#dput $PPA $SRCCHFILE

# Remove all temporary files
#rm -rf $ATOMDIR

echo "******** Write current SVN rev to file ********"
# Write the current svn revision to file
echo -n $CURRREV > $REVFILE

cd $RUNDIR

# Remove temprary running-file
rm $TMPFILE
