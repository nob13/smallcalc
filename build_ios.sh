#!/bin/bash
set -e # fail on first error
DIRNAME=`dirname $0`
cd $DIRNAME
mkdir -p build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../iOS.toolchain.cmake -DBOOST_ROOT=$HOME/libs/boost -GXcode
cmake . -DCMAKE_INSTALL_PREFIX=../install/arm_debug
xcodebuild -target install -configuration Debug
cmake . -DCMAKE_INSTALL_PREFIX=../install/simulator_debug
xcodebuild -target install -configuration Debug -sdk iphonesimulator -arch i386
cmake . -DCMAKE_INSTALL_PREFIX=../install/arm_release
xcodebuild -target install -configuration Release
cmake . -DCMAKE_INSTALL_PREFIX=../install/simulator_release
xcodebuild -target install -configuration Release -sdk iphonesimulator -arch i386

