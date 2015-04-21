#!/bin/sh

ROOT=$(git rev-parse --show-toplevel)
BIN=$ROOT/bin
UTILS=$ROOT/utils
MAKE=make

BOOTSTRAPPREMAKE=$BIN/premake5

if [ ! -f "$BOOTSTRAPPREMAKE" ]; then
	BOOTSTRAPPREMAKE=$BIN/premake4
fi

if [ ! -f "$BOOTSTRAPPREMAKE" ]; then
	echo "not found any premake for bootstrap, place it to $BIN"
	exit 1
fi

echo "rebuild premake5"
cd $UTILS/premake5
$BOOTSTRAPPREMAKE embed || exit 1
$BOOTSTRAPPREMAKE gmake || exit 1
$MAKE config=release || exit 1
cp $UTILS/premake5/bin/release/* $BIN
PREMAKE5=$BIN/premake5
cd $UTILS

cd $UTILS/assetsbuilder
$PREMAKE5 --scripts=$ROOT/projects/premake5 gmake || exit 1
$MAKE config=release || exit 1
cp $UTILS/assetsbuilder/bin/release/* $BIN
cd $UTILS