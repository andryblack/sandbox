#! /bin/bash

cd `dirname $0`



xcodebuild -project lua.xcodeproj -configuration Release -target lua && \
xcodebuild -project lua.xcodeproj -configuration Debug -target lua 