

print('hello premake5')


local p = premake
local api = p.api



-- ios

table.insert(premake.option.get('os').allowed,{'ios',"Apple iOS"})

-- flash
table.insert(premake.option.get('os').allowed,{'flash',"Crossbridge flash"})
		
newoption {
	trigger = "flex-sdk-dir",
	value = "PATH",
	description = "FLEX sdk path"
}

newoption {
	trigger = "flascc-sdk-dir",
	value = "PATH",
	description = "CrossBridge sdk path"
}

-- android
table.insert(premake.option.get('os').allowed,{'android',"Google android"})
include( 'android/android.lua' )
