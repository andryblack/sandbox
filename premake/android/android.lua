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

api.register {
	name = "android_toolchain",
	scope = "solution",
	kind = "string"
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

-- Specify android java build API level
api.register {
	name = "android_build_api_level",
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
	scope = "project",
	kind = "string"
}

-- Specify android package version
api.register {
	name = "android_packageversion",
	scope = "project",
	kind = "integer"
}

api.register {
	name = "android_packageversionname",
	scope = "project",
	kind = "string"
}

api.register {
	name = "android_repository",
	scope = "solution",
	kind = "string",
	list = true
}

api.register {
	name = "android_build_dependencies",
	scope = "solution",
	kind = "string",
	list = true
}



api.register {
	name = "android_build_task",
	scope = 'project',
	kind = 'table',
	list = true
}

api.register {
	name = "android_build_plugin",
	scope = 'project',
	kind = 'string',
	list = true
}

api.register {
	name = "android_build_gradle",
	scope = 'project',
	kind = 'string'
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
	scope = "project",
	kind = "string",
	list = true
}

api.register {
	name = "android_scheme",
	scope = 'project',
	kind = 'string',
	list = true
}

api.register {
	name = "android_metadata",
	scope = 'project',
	kind = 'string',
	list = true
}

api.register {
	name = "android_provider",
	scope = 'project',
	kind = 'table',
	list = true
}

-- Specify applicaton orientation
api.register {
	name = "android_screenorientation",
	scope = "project",
	kind = "string",
	allowed = {
		'landscape',
		'reverseLandscape',
		'sensorLandscape',
		'portrait',
		'reversePortrait',
		'sensorPortrait',
		'fullSensor',
		'sensor',
		'user',
		'behind',
		'unspecified'
	}

}

api.register {
	name = "android_key_store",
	scope = "config",
	kind = "path"
}
api.register {
	name = "android_key_alias",
	scope = "config",
	kind = "string"
}
api.register {
	name = "android_keystore_pwd",
	scope = "config",
	kind = "string"
}
api.register {
	name = "android_key_pwd",
	scope = "config",
	kind = "string"
}


-- Specify applicaton permissions


api.register {
	name = "android_modules_path",
	scope = "solution",
	kind = "string"
}

api.register {
	name = "android_assets_path",
	scope = "config",
	kind = "path"
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
	scope = 'project',
	kind = 'string',
	list = true
}

api.register {
	name = "android_aidl",
	scope = 'project',
	kind = 'string',
	list = true
}

api.register {
	name = "android_dependencies",
	scope = 'project',
	kind = 'string',
	list = true
}

api.register {
	name = "android_receiver",
	scope = 'project',
	kind = 'table',
	list = true
}

api.register {
	name = "android_service",
	scope = 'project',
	kind = 'table',
	list = true
}

api.register {
	name = "android_module",
	scope = 'solution',
	kind = 'string',
	keyed = true
}

api.register {
	name = "android_manifest",
	scope = 'solution',
	kind = 'path',
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
		build.onsolution(sln)
		manifest.onsolution(sln)
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
