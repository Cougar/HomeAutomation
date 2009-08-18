#!/bin/bash

#
# This is a script that installs needed software and initiates default configs
# Prerequisites: SVN repos 'HomeAutomation' downloaded and running srript from folder 'trunk'
# See more at http://projekt.auml.se/homeautomation:guides:getting_started_with_starterkit

# Make script exit if any command fails
set +e

# Have a counter displayed at each step
CNT=1

echo -e "* ${CNT}. Installing gcc-avr avr-libc mono-gmcs libdevice-serialport-perl scons build-essential"
CNT=$(expr $CNT + 1)
sudo apt-get install gcc-avr avr-libc mono-gmcs libdevice-serialport-perl scons build-essential
echo "Done"

echo -e "\n* ${CNT}. Compiling canDude"
CNT=$(expr $CNT + 1)
cd PcSoftware/canDude/
make
cd ../..
echo "Done"

echo -e "\n* ${CNT}. Creating system.inc from system.inc.template"
CNT=$(expr $CNT + 1)
cp -v EmbeddedSoftware/AVR/personal/system.inc.template EmbeddedSoftware/AVR/personal/system.inc
echo "Done"

echo -e "\n* ${CNT}. Compiling Atom, may take a while"
CNT=$(expr $CNT + 1)
cd PcSoftware/Atom/
make
cd ../..
echo "Done"

echo -e "\n* ${CNT}. Creating atom.conf from atom.conf.template"
CNT=$(expr $CNT + 1)
cp -v PcSoftware/Atom/atom.conf.template PcSoftware/Atom/atom.conf
echo "Done"

