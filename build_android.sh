#!/bin/bash
set -e # fail on first error
DIRNAME=`dirname $0`
BOOST_ROOT=/usr
cd $DIRNAME
mkdir -p build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../android.toolchain.cmake  -DANDROID_NATIVE_API_LEVEL=9 -DLIBRARY_OUTPUT_PATH_ROOT=. -DBOOST_ROOT=$BOOST_ROOT -DCMAKE_INSTALL_PREFIX=../install
cmake . -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_INSTALL_PREFIX=../install/arm_debug
make install
cmake . -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=../install/arm_release
make install


