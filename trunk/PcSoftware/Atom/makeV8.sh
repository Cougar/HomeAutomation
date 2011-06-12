#!/bin/bash

svn checkout http://v8.googlecode.com/svn/tags/3.2.3.1 src/v8
cd src/v8

arch=`uname -m`

if [ "$arch" = "x86_64" ]; then
	scons arch=x64 mode=debug snapshot=on
else
	scons mode=debug snapshot=on
fi

cd ../..
