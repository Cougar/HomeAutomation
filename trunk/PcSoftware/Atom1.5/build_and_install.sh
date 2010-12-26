#!/bin/bash

mkdir build
cd build
cmake -DUSE_PLUGIN_XORG=true ../
make
cpack -DUSE_PLUGIN_XORG=true ../
sudo dpkg --install Atom-1.5-x86_64.deb
cd ..
