#!/bin/bash

cd build
mkdir build
cmake ../
make
cpack ../
sudo dpkg --install *.deb
cd ..
