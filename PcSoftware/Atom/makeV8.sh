#!/bin/bash

svn checkout http://v8.googlecode.com/svn/tags/3.2.3.1 src/v8
cd src/v8

cp SConstruct SConstruct.bak
patch SConstruct < ../atom.v8.diff

arch=`uname -m`

if [ "$arch" = "x86_64" ]; then
	scons -Wall arch=x64 mode=release snapshot=on
else
	scons -Wall mode=release snapshot=on
fi

mv SConstruct.bak SConstruct

cd ../..
