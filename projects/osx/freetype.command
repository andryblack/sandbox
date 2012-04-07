#! /bin/bash

cd `dirname $0`

#rm -Rf build-freetype

# build fat library
xcodebuild -project freetype.xcodeproj -configuration Release -target freetype && \
xcodebuild -project freetype.xcodeproj -configuration Debug -target freetype 