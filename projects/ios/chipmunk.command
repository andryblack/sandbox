#! /bin/bash

cd `dirname $0`

# clean old
# if [ -e Chipmunk-iPhone ]; then rm -rf Chipmunk-iPhone; fi

# mkdir Chipmunk-iPhone

# build fat library
xcodebuild -project chipmunk.xcodeproj -sdk iphoneos4.3 -configuration Release -target chipmunk && \
xcodebuild -project chipmunk.xcodeproj -sdk iphoneos4.3 -configuration Debug -target chipmunk && \
xcodebuild -project chipmunk.xcodeproj -sdk iphonesimulator4.3 -configuration Release -target chipmunk && \ 
xcodebuild -project chipmunk.xcodeproj -sdk iphonesimulator4.3 -configuration Debug -target chipmunk 
