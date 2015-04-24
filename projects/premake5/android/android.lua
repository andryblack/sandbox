premake.modules.android = {}

local android = premake.modules.android

-- Shortcuts
local api       = premake.api
local config    = premake.config
local make      = premake.make
local project   = premake.project
local solution  = premake.solution

--
-- Register Android configuration options with Premake.
--

-- Specify android ABIs
api.register {
	name = "android_abis",
	scope = "solution",
	kind = "string",
	list = "true",
	allowed = {
		"all",
		"armeabi",
		"armeabi-v7a",
		"mips",
		"x86"
	}
}

-- Specify android jni API level
api.register {
	name = "android_api_level",
	scope = "solution",
	kind = "number"
}

-- Specify android java API level
api.register {
	name = "android_target_api_level",
	scope = "solution",
	kind = "number"
}

-- Specify android STL support
api.register {
	name = "android_stl",
	scope = "solution",
	kind = "string",
	allowed = {
		"gabi++_static",
		"gabi++_shared",
		"gnustl_static",
		"gnustl_shared",
		"stlport_static",
		"stlport_shared",
		"c++_static",
		"c++_shared",
		"system"
	}
}

-- Specify android package name
api.register {
	name = "android_packagename",
	scope = "solution",
	kind = "string"
}

-- Specify android package version
api.register {
	name = "android_packageversion",
	scope = "solution",
	kind = "integer"
}

api.register {
	name = "android_packageversionname",
	scope = "solution",
	kind = "string"
}

-- Specify android activity name
api.register {
	name = "android_activity",
	scope = "project",
	kind = "string"
}

-- Specify applicaton permissions
api.register {
	name = "android_permissions",
	scope = "solution",
	kind = "string",
	list = true
}

api.register {
	name = "android_metadata",
	scope = 'solution',
	kind = 'string',
	list = true
}

-- Specify applicaton orientation
api.register {
	name = "android_screenorientation",
	scope = "solution",
	kind = "string",
	allowed = {
		'landscape',
		'portrait',
		'any'
	}

}

api.register {
	name = "android_key_store",
	scope = "solution",
	kind = "string"
}
api.register {
	name = "android_key_alias",
	scope = "solution",
	kind = "string"
}

-- Specify applicaton permissions


api.register {
	name = "android_modules_path",
	scope = "solution",
	kind = "string"
}

api.register {
	name = "android_ndk_modules",
	scope = "project",
	kind = "string",
	list = true
}

api.register {
	name = "android_ndk_static_libs",
	scope = "project",
	kind = "string",
	list = true
}

api.register {
	name = "android_libs",
	scope = 'solution',
	kind = 'string',
	list = true
}

newoption {
	trigger = "android-ndk-dir",
	value = "PATH",
	description = "Android NDK path"
}
newoption {
	trigger = "android-sdk-dir",
	value = "PATH",
	description = "Android SDK path"
}

-- Support code
android.ndk = require 'ndk'
local ndk = android.ndk
android.manifest = require 'manifest'
local manifest = android.manifest
android.build = require 'build'
local build = android.build

-- Register the action with Premake.
newaction {
	trigger     = "android",
	shortname   = "Android project",
	description = "Generate files for Android build",

	-- The capabilities of this action
	valid_kinds     = { "WindowedApp", "StaticLib", "SharedLib" },
	valid_languages = { "C", "C++" },
	valid_tools     = {
		cc     = { "gcc" },
	},
	onsolution = function( sln )
		ndk.onsolution(sln)
		manifest.onsolution(sln)
		build.onsolution(sln)
	end,
	onproject = function( prj )
		ndk.onproject(prj)
	end,
	oncleansolution = function( sln )
		ndk.oncleansolution(sln)
		manifest.oncleansolution(sln)
		build.oncleansolution(sln)
	end,
	oncleanproject = function( sln )
		ndk.oncleanproject(sln)
	end
	
}
-- Actions
--require 'makefile'
--require 'manifest'

return android
