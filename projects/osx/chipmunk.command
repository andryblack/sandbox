#! /bin/bash

cd `dirname $0`

# clean old
# if [ -e Chipmunk-iPhone ]; then rm -rf Chipmunk-iPhone; fi

# mkdir Chipmunk-iPhone

# build fat library
xcodebuild -project chipmunk.xcodeproj -configuration Release -target chipmunk && \
xcodebuild -project chipmunk.xcodeproj -configuration Debug -target chipmunk 
