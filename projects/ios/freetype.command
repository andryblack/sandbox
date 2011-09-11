#! /bin/bash

cd `dirname $0`

rm -Rf build-freetype

# build fat library
xcodebuild -project freetype.xcodeproj -sdk iphoneos4.3 -configuration Release -target freetype && \
xcodebuild -project freetype.xcodeproj -sdk iphoneos4.3 -configuration Debug -target freetype && \
xcodebuild -project freetype.xcodeproj -sdk iphonesimulator4.3 -configuration Release -target freetype && \ 
xcodebuild -project freetype.xcodeproj -sdk iphonesimulator4.3 -configuration Debug -target freetype 
