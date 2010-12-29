#!/bin/bash

mkdir build
cd build
#cmake ../
cmake -DUSE_PLUGIN_XORG=ON ../
make
#cpack ../
cpack -DUSE_PLUGIN_XORG=ON ../
sudo dpkg --install *.deb
cd ..
