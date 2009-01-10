#!/bin/bash

svn checkout http://v8.googlecode.com/svn/trunk/ v8
cd v8
scons mode=release snapshot=on
cd ..
