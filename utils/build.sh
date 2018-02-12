#!/bin/bash

ROOT=$(git rev-parse --show-toplevel)
BIN=$ROOT/bin
UTILS=$ROOT/utils
MAKE="make -j4"

mkdir -p $BIN

if [[ "x$PLATFORM" == "x" ]]; then
	PLATFORM=`uname -s`
fi

if [[ "$PLATFORM" == "mingw"* ]]; then
	PLATFORM="windows"
elif [[ "$PLATFORM" == "MINGW32"* ]]; then
	PLATFORM="windows"
elif [[ "$PLATFORM" == "MINGW64"* ]]; then
	PLATFORM="windows"
elif [[ "$PLATFORM" == "Darwin" ]]; then
	PLATFORM="osx"
fi

echo "platform: $PLATFORM"

TARGET=gmake
EXE=""

if [[ "$PLATFORM" == "windows" ]]; then
	EXE=".exe"
	MAKE=echo
	TARGET=vs2015
fi

mkdir -p $BIN/$PLATFORM

BOOTSTRAPPREMAKE=$BIN/$PLATFORM/premake5$EXE

if [ ! -f "$BOOTSTRAPPREMAKE" ]; then
	BOOTSTRAPPREMAKE=$BIN/$PLATFORM/premake4$EXE
fi

if [ ! -f "$BOOTSTRAPPREMAKE" ]; then
	echo "not found any premake for bootstrap, place it to $BIN/$PLATFORM"
	if [[ "$PLATFORM" == "osx" ]]; then
		BOOTSTRAPPREMAKE=$BIN/$PLATFORM/premake5
		echo "try get it"
		cd $BIN/$PLATFORM && curl -L "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha8/premake-5.0.0-alpha8-macosx.tar.gz" | tar xz 
	elif [[ "$PLATFORM" != "windows" ]]; then
		BOOTSTRAPPREMAKE=$BIN/$PLATFORM/premake5
		echo "try get it"
		cd $BIN/$PLATFORM && wget -O - "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha8/premake-5.0.0-alpha8-linux.tar.gz" | tar xz 
	fi
	exit 1
fi

echo "rebuild premake5"
cd $UTILS/premake5
rm -rf src/host/scripts.c
rm -rf *.make
rm -rf Makefile
rm -Rf obj
$BOOTSTRAPPREMAKE clean
$BOOTSTRAPPREMAKE embed || exit 1
$BOOTSTRAPPREMAKE --no-curl $TARGET || exit 1
$MAKE config=release -j8 || exit 1
cp $UTILS/premake5/bin/release/* $BIN/$PLATFORM
PREMAKE5=$BIN/$PLATFORM/premake5$EXE
cd $UTILS

echo "rebuild assetsbuilder"
cd $UTILS/assetsbuilder
rm -rf Makefile obj *.make
$PREMAKE5 --scripts=$ROOT/premake $TARGET || exit 1
$MAKE config=release -j8 || exit 1
cp $UTILS/assetsbuilder/bin/release/* $BIN/$PLATFORM
cd $UTILS