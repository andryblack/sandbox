#! /bin/bash

cd `dirname $0`

# clean old
# if [ -e Chipmunk-iPhone ]; then rm -rf Chipmunk-iPhone; fi

# mkdir Chipmunk-iPhone

# build fat library
xcodebuild -project lua.xcodeproj -sdk iphoneos4.3 -configuration Release -target lua && \
xcodebuild -project lua.xcodeproj -sdk iphoneos4.3 -configuration Debug -target lua && \
xcodebuild -project lua.xcodeproj -sdk iphonesimulator4.3 -configuration Release -target lua && \ 
xcodebuild -project lua.xcodeproj -sdk iphonesimulator4.3 -configuration Debug -target lua 
