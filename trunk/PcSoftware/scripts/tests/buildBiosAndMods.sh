#!/bin/bash

# Author Erik Larsson, Anders Runeson
# Date 2008-05-12, 2008-07-03
#
# This script builds default bios and if successful tries to build all the modules


SCRIPTDIR=`dirname $0`

source ${SCRIPTDIR}/settings-modules.sh

PYTHON=/usr/bin/python

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
svn export template_module $TESTNODE

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
	echo "BIOS successful, continues with modules"
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

MODLIST=$($PYTHON ModuleManager -c)
for module in $MODLIST; do

	echo; echo

	echo ""
	echo "#######################################"
	echo "Testing module $module"
	echo "#######################################"
	echo ""
	echo "Adding module:"
	$PYTHON ModuleManager --add=$module
    
	#get svn info of the application
	echo "Svn info:"
	cd ../../module/$module
	svn info
	cd ../../personal/$TESTNODE
    
	# Build
	echo "Building:"
	if make
	then
		echo ""
		echo "#######################################"
		echo "$module successful, continues with next module"
		echo "#######################################"
		echo ""
		
		successfull=( "${successfull[@]}" "$module\n" )

	else
		echo ""
		echo "#######################################"
		echo "$module failed, continues with next module"
		echo "#######################################"
		echo ""
		
		failed=( "${failed[@]}" "$module\n" )
	fi

	make clean
	$PYTHON ModuleManager --del=$module
	#rm src
	#rm config.inc
	#rm sources.inc
	
    continue # On to next module
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
echo "Successfull modules:"
echo  -e ${successfull[@]}
echo "Failed modules:"
echo -e ${failed[@]}
echo "#######################################"
echo ""

if [ $DOPOSTIRC -eq "1" ] ; then
	if [ ${#failed[@]} -ne "0" ] ; then
		$POSTLINE \#hobby Tested to build modules, failed ${#failed[@]}, successfull: ${#successfull[@]}, http://projekt.auml.se/homeautomation:software:embedded:last-build-mod-test
	fi
fi

exit 0

