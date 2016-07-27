--
-- manifest.lua
-- Android NDK makefile generator for Premake.
-- Copyright (c) 2014 Will Vale and the Premake project
--

local ndk       = premake.modules.android.ndk
local project   = premake.project
local config    = premake.config
local solution 	= premake.solution
local fileconfig = premake.fileconfig

local manifest = {}



function manifest.onsolution( sln )
	premake.escaper(nil)

	for prj in solution.eachproject(sln) do
		if prj.kind == premake.WINDOWEDAPP then
			local function gen(sln)
				manifest.generateManifest(sln,prj)
			end
			premake.generate(sln, manifest.getManifestFilename( sln,prj ), gen)
		end
	end

end

function manifest.oncleansolution( sln )

end

-- Manifest goes in project dirctory
function manifest.getManifestFilename(sln,prj)
	return path.join(sln.location, prj.shortname or prj.name , ndk.MANIFEST )
end

function manifest.generateManifest(sln,prj)
	local packagename = prj.android_packagename or 'com.example.app'
	local activity = prj.android_activity or 'android.app.NativeActivity'
	local attributes = 'android:hasCode="true"'
	
	local mainLibName = prj.name
	local debugVal = 'false'
	
	_p('<?xml version="1.0" encoding="utf-8"?>')
	_p('<!-- Android application manifest autogenerated by Premake -->')
	_x(0, '<manifest xmlns:android="http://schemas.android.com/apk/res/android"')
		_x(1, 'package="%s"', packagename)
		_x(1, 'android:versionCode="%d"', prj.android_packageversion or 0)
		_x(1, 'android:versionName="%s" >', prj.android_packageversionname or "0")

-- SDK version
	if sln.android_api_level then
		_x(1, '<uses-sdk android:minSdkVersion="%d" />', sln.android_api_level)
	end
		-- Application tag
		_x(1, '<application android:label="@string/app_name"  android:icon="@drawable/icon"')
			_x(2, attributes)
			_x(2, 'android:theme="@android:style/Theme.NoTitleBar.Fullscreen">')

			-- Contains activity
			_x(2, '<activity android:name="%s"',activity)
				_x(3,'android:label="@string/app_name"')
				_x(3,'android:screenOrientation="%s"',prj.android_screenorientation or 'landscape')
				_x(3,'android:configChanges="orientation|keyboardHidden"')
				_x(3,'android:windowSoftInputMode="adjustResize">')
				_x(3,'<!-- Tell NativeActivity the name of or .so -->')
            	_x(3,'<meta-data android:name="android.app.lib_name"')
                	_x(4,'android:value="%s" />', mainLibName)

				-- Launcher intent
				_x(3, '<intent-filter>')
					_p(4, '<action android:name="android.intent.action.MAIN" />')
					_p(4, '<category android:name="android.intent.category.LAUNCHER" />')
				_p(3, '</intent-filter>')
			
			_p(2, '</activity>')

			for _,v in ipairs(prj.android_metadata or {}) do
				local name = nil
				local val = nil
				local p = v:find('=')
				if p then
					name = v:sub(1,p-1)
					val = v:sub(p+1)
				else
					name = v
					val = ''
				end
				_x(2,'<meta-data android:name="%s"',name)
        			_x(3,'android:value="%s" />',val)
			end
		
		

		_x(1, '</application>')

	

	-- Features
	local gles_version = ndk.getGlesVersion(sln)
	if gles_version then
		_x(1, '<uses-feature android:glEsVersion="0x%08x" />', gles_version)
	end

	-- Permissions
	if prj.android_permissions then
		for _,v in ipairs(prj.android_permissions) do
		     _x(1, '<uses-permission android:name="%s" />', v)
		end
	end

	 
	_p('</manifest>')
end





return manifest