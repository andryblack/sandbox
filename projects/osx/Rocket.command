#! /bin/bash

cd `dirname $0`

#rm -Rf build-Rocket

# build fat library
xcodebuild -project Rocket.xcodeproj -sdk iphoneos -configuration Release -target RocketCore 
xcodebuild -project Rocket.xcodeproj -sdk iphoneos -configuration Debug -target RocketCore 
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator -configuration Release -target RocketCore 
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator -configuration Debug -target RocketCore 
lipo ../../lib/libRocketCore-Debug-iphoneos.a ../../lib/libRocketCore-Debug-iphonesimulator.a -create -output ../../lib/libRocketCore-iOS_d.a && \
lipo ../../lib/libRocketCore-Release-iphoneos.a ../../lib/libRocketCore-Release-iphonesimulator.a -create -output ../../lib/libRocketCore-iOS.a && \
rm -f ../../lib/libRocketCore-Debug*
rm -f ../../lib/libRocketCore-Release*

xcodebuild -project Rocket.xcodeproj -sdk iphoneos -configuration Release -target RocketDebugger
xcodebuild -project Rocket.xcodeproj -sdk iphoneos -configuration Debug -target RocketDebugger
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator -configuration Release -target RocketDebugger
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator -configuration Debug -target RocketDebugger 
lipo ../../lib/libRocketDebugger-Debug-iphoneos.a ../../lib/libRocketDebugger-Debug-iphonesimulator.a -create -output ../../lib/libRocketDebugger-iOS_d.a && \
lipo ../../lib/libRocketDebugger-Release-iphoneos.a ../../lib/libRocketDebugger-Release-iphonesimulator.a -create -output ../../lib/libRocketDebugger-iOS.a && \
rm -f ../../lib/libRocketDebugger-Debug*
rm -f ../../lib/libRocketDebugger-Release*

xcodebuild -project Rocket.xcodeproj -sdk iphoneos -configuration Release -target RocketControls
xcodebuild -project Rocket.xcodeproj -sdk iphoneos -configuration Debug -target RocketControls
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator -configuration Release -target RocketControls
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator -configuration Debug -target RocketControls 

lipo ../../lib/libRocketControls-Debug-iphoneos.a ../../lib/libRocketControls-Debug-iphonesimulator.a -create -output ../../lib/libRocketControls-iOS_d.a && \
lipo ../../lib/libRocketControls-Release-iphoneos.a ../../lib/libRocketControls-Release-iphonesimulator.a -create -output ../../lib/libRocketControls-iOS.a && \
rm -f ../../lib/libRocketControls-Debug*
rm -f ../../lib/libRocketControls-Release*
