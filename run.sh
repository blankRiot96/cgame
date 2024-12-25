#!/usr/bin/env bash

cd build
./premake5 gmake2
cd ..
make &&./bin/Debug/cgame