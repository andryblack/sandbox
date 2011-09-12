#! /bin/bash

cd `dirname $0`

#rm -Rf build-Rocket

# build fat library
xcodebuild -project Rocket.xcodeproj -sdk iphoneos4.3 -configuration Release -target RocketCore 
xcodebuild -project Rocket.xcodeproj -sdk iphoneos4.3 -configuration Debug -target RocketCore 
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator4.3 -configuration Release -target RocketCore 
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator4.3 -configuration Debug -target RocketCore 

xcodebuild -project Rocket.xcodeproj -sdk iphoneos4.3 -configuration Release -target RocketDebugger
xcodebuild -project Rocket.xcodeproj -sdk iphoneos4.3 -configuration Debug -target RocketDebugger
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator4.3 -configuration Release -target RocketDebugger
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator4.3 -configuration Debug -target RocketDebugger 

xcodebuild -project Rocket.xcodeproj -sdk iphoneos4.3 -configuration Release -target RocketControls
xcodebuild -project Rocket.xcodeproj -sdk iphoneos4.3 -configuration Debug -target RocketControls
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator4.3 -configuration Release -target RocketControls
xcodebuild -project Rocket.xcodeproj -sdk iphonesimulator4.3 -configuration Debug -target RocketControls 
