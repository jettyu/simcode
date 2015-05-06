#!/bin/sh
echo $PWD
mkdir -p build
TARGET_PATH=$PWD/build
MAKE_FLAGS="-j4 DEBUG=y TARGET_PATH=$TARGET_PATH"
cd base;make $MAKE_FLAGS;cd -;
cd thread;make $MAKE_FLAGS;cd -;
cd net;make $MAKE_FLAGS;cd -;
cd redis;make $MAKE_FLAGS;cd -;
cd mysql;make $MAKE_FLAGS;cd -;
cd cache/redis;make $MAKE_FLAGS;cd -;

cd base;make clean;cd -;
cd thread;make clean;cd -;
cd net;make clean;cd -;
cd redis;make clean;cd -;
cd mysql;make clean;cd -;
cd cache/redis;make clean;cd -;

exit
