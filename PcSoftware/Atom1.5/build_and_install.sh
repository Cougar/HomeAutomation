#!/bin/bash

mkdir build
cd build
rm *.deb
cmake ../
make
cpack ../
sudo dpkg --install *.deb
cd ..
