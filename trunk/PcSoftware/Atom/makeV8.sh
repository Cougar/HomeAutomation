#!/bin/bash

svn checkout http://v8.googlecode.com/svn/tags/3.2.3.1 src/v8
cd src/v8

arch=`uname -m`

if [ "$arch" = "x86_64" ]; then
	ARCH='arch=x64'
else
	ARCH=''
fi

patch SConstruct -o AumlSConstruct < ../atom.v8.diff
scons -f AumlSConstruct $ARCH mode=release snapshot=on
rm AumlSConstruct

cd ../..

