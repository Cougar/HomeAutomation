#!/bin/bash

mkdir build
cd build
#cmake ../
cmake -DUSE_PLUGIN_XORG=true ../
make
#cpack ../
cpack -DUSE_PLUGIN_XORG=true ../
sudo dpkg --install *.deb
cd ..
