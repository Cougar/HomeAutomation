#!/bin/bash

# Author Erik Larsson
# Date 2008-05-12
#
# This script builds default bios and if successful tries to build all the applications

SCRIPTDIR=`dirname $0`

source ${SCRIPTDIR}/settings.sh

# Put log in separate file
exec &> $LOGFILE


STARTDATE=`date`
echo "Starting $STARTDATE"

#Go to personal nodes
cd $PERSONALPATH

if test -e "$TESTNODE"
then
	echo "Directory already exist you fool"
	exit 0
fi

echo "Export template to testing node"
svn export template $TESTNODE

cd $TESTNODE

echo ""
echo "#######################################"
echo "Building BIOS"
echo "#######################################"
echo ""

if make bios
then
	echo ""
	echo "#######################################"
	echo "BIOS successful, continues with applications"
	echo "#######################################"
	echo ""
else
	echo ""
	echo "#######################################"
	echo "BIOS failed, stops testing"
	echo "#######################################"
	echo ""
	
	exit 0
fi

successful=() # Array for successful builds
failed=() # Array for failed builds

for testappl in $APPLPATH*
do
	echo; echo

    #with symlinks
    ln -s "$testappl" src
 
    echo ""
	echo "#######################################"
	echo "Building $testappl"
	echo "#######################################"
	echo ""
    
    #get svn info of the application
    cd src
    svn info
    cd ..
    
    # Build application
    if make
	then
		echo ""
		echo "#######################################"
		echo "$testappl successful, continues with next application"
		echo "#######################################"
		echo ""
		
		successfull=( "${successfull[@]}" "$testappl\n" )

	else
		echo ""
		echo "#######################################"
		echo "$testappl failed, continues with next application"
		echo "#######################################"
		echo ""
		
		failed=( "${failed[@]}" "$testappl\n" )
	fi

	make clean
	rm src
	rm config.inc
	
    continue # On to next application
  echo
done  


# Remove test node after finished building
cd ..
rm -r "$TESTNODE"

ENDDATE=`date`

echo ""
echo "#######################################"
echo "Results"
echo
echo "Started $STARTDATE"
echo "Finished $ENDDATE"
echo
echo "Successfull applications:"
echo  -e ${successfull[@]}
echo "Failed applications:"
echo -e ${failed[@]}
echo "#######################################"
echo ""

#echo "Debug: DOPOSTIRC=$DOPOSTIRC failed=${#failed[@]} POSTLINE=$POSTLINE"

if [ $DOPOSTIRC -eq "1" ] ; then
	if [ ${#failed[@]} -ne "0" ] ; then
#echo "Debug: DOPOSTIRC=$DOPOSTIRC failed=${#failed[@]} POSTLINE=$POSTLINE"
		$POSTLINE \#hobby Tested to build apps, ${#failed[@]} failed, ${#successfull[@]} successfull, http://projekt.auml.se/homeautomation:software:embedded:last-build-test
		$POSTLINE2 Tested to build apps, ${#failed[@]} failed, ${#successfull[@]} successfull, http://projekt.auml.se/homeautomation:software:embedded:last-build-test
#		$POSTLINE \#lekstuga Tested to build apps, failed ${#failed[@]}, successfull: ${#successfull[@]}, http://projekt.auml.se/homeautomation:software:embedded:last-build-test
#echo "POSTLINE exitcode: $?"
	fi
fi

exit 0

