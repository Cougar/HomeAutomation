#!/bin/bash

svn checkout http://v8.googlecode.com/svn/trunk/ src/v8
cd src/v8
scons mode=release snapshot=on
scons mode=debug snapshot=on
cd ../..
