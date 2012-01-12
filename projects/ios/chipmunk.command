#! /bin/bash

cd `dirname $0`

# clean old
# if [ -e Chipmunk-iPhone ]; then rm -rf Chipmunk-iPhone; fi

# mkdir Chipmunk-iPhone

# build fat library
xcodebuild -project chipmunk.xcodeproj -sdk iphoneos -configuration Release -target chipmunk && \
xcodebuild -project chipmunk.xcodeproj -sdk iphoneos -configuration Debug -target chipmunk && \
xcodebuild -project chipmunk.xcodeproj -sdk iphonesimulator -configuration Release -target chipmunk && \ 
xcodebuild -project chipmunk.xcodeproj -sdk iphonesimulator -configuration Debug -target chipmunk 
lipo ../../lib/libchipmunk-Debug-iphoneos.a ../../lib/libchipmunk-Debug-iphonesimulator.a -create -output ../../lib/libchipmunk_d.a && \
lipo ../../lib/libchipmunk-Release-iphoneos.a ../../lib/libchipmunk-Release-iphonesimulator.a -create -output ../../lib/libchipmunk.a && \
rm -f ../../lib/libchipmunk-*