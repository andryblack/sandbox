#! /bin/bash

cd `dirname $0`

#rm -Rf build-freetype

# build fat library
xcodebuild -project freetype.xcodeproj -sdk iphoneos -configuration Release -target freetype && \
xcodebuild -project freetype.xcodeproj -sdk iphoneos -configuration Debug -target freetype && \
xcodebuild -project freetype.xcodeproj -sdk iphonesimulator -configuration Release -target freetype && \
xcodebuild -project freetype.xcodeproj -sdk iphonesimulator -configuration Debug -target freetype 
lipo ../../lib/libfreetype-Debug-iphoneos.a ../../lib/libfreetype-Debug-iphonesimulator.a -create -output ../../lib/libfreetype_d.a 
lipo ../../lib/libfreetype-Release-iphoneos.a ../../lib/libfreetype-Release-iphonesimulator.a -create -output ../../lib/libfreetype.a 
rm -f ../../lib/libfreetype-*