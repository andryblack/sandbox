#! /bin/bash

cd `dirname $0`



xcodebuild -project lua.xcodeproj -sdk iphoneos -configuration Release -target lua && \
xcodebuild -project lua.xcodeproj -sdk iphoneos -configuration Debug -target lua && \
xcodebuild -project lua.xcodeproj -sdk iphonesimulator -configuration Release -target lua && \
xcodebuild -project lua.xcodeproj -sdk iphonesimulator -configuration Debug -target lua && \
lipo ../../lib/liblua-Debug-iphoneos.a ../../lib/liblua-Debug-iphonesimulator.a -create -output ../../lib/liblua-iOS_d.a && \
lipo ../../lib/liblua-Release-iphoneos.a ../../lib/liblua-Release-iphonesimulator.a -create -output ../../lib/liblua-iOS.a && \
rm -f ../../lib/liblua-Debug*
rm -f ../../lib/liblua-Release*