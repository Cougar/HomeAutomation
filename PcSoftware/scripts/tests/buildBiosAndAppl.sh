#!/bin/bash

# Author Erik Larsson
# Date 2008-05-12
#
# This script builds default bios and if successful tries to build all the applications



PERSONALPATH=$HOME"/svn/HomeAutomation/trunk/EmbeddedSoftware/AVR/personal"
APPLPATH="../../application/"
TESTNODE="nightTestNode"
LOGFILE="results.log"


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
#	rm src

	#copy whole application
    rm -rf src
    
    #with symlinks
 #   ln -s "$testappl" src
 
	#copy whole application
	cp -r "$testappl" src 

    #with symlinks
#    if test -e "src/config.inc"
#	then
#		CONF_EXIST=true
#		echo "config.inc already exist"
#	else
#		cp src/config.inc.template src/config.inc
#		CONF_EXIST=false
#	fi

	#copy whole application
	cp src/config.inc.template src/config.inc
    
    echo ""
	echo "#######################################"
	echo "Building $testappl"
	echo "#######################################"
	echo ""
    
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
	    
	#with symlinks
#	if CONF_EXIST
#	then
#		echo "config.inc existed already, dont remove"
#	else
#		echo "removing config.inc"
#		rm src/config.inc
#	fi
	
    continue # On to next application
  echo
done  


# Remove test node after finished building
cd ..
rm -rf "$TESTNODE"

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

exit 0


