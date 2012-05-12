#!/bin/bash

mkdir debian
svn export http://svn.arune.se/svn/HomeAutomation/trunk/PcSoftware/scripts/debian/files/atom.conf debian/atom.conf


mkdir build
cd build
rm *.deb

cmake ../

if [ $? -ne 0 ] ; then
echo "Failure cmake"
cd ..
exit 1
fi


make

if [ $? -ne 0 ] ; then
echo "Failure make"
cd ..
exit 1
fi


cpack ../

if [ $? -ne 0 ] ; then
echo "Failure cpack"
cd ..
exit 1
fi


sudo dpkg --install *.deb

if [ $? -ne 0 ] ; then
echo "Failure dpkg"
cd ..
exit 1
fi

echo "Success!"
cd ..
