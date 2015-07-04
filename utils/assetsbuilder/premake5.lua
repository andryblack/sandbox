---
-- Premake 5.x build configuration script
-- Use this script to configure the project with Premake5.
---

--
-- Remember my location; I will need it to locate sub-scripts later.
--

	local corePath = _SCRIPT_DIR

	sandbox_dir = path.join(_SCRIPT_DIR,'../..')
--
-- Register supporting actions and options.
--
	local utils = require 'utils'



	newoption {
		trigger = "to",
		value   = "path",
		description = "Set the output location for the generated files"
	}



--
-- Define the project. Put the release configuration first so it will be the
-- default when folks build using the makefile. That way they don't have to
-- worry about the /scripts argument and all that.
--
-- TODO: defaultConfiguration "Release"
--

	solution "AssetsBuilder"
		configurations { "Release", "Debug" }
		location ( _OPTIONS["to"] )
		defines {'GHL_STATIC_LIB','SB_SILENT' , 'GHL_SILENT'}

		flags       { "No64BitChecks", "ExtraWarnings", "StaticRuntime" }

		configuration "Debug"
			targetdir   "bin/debug"
			defines     { "_DEBUG" , "SB_DEBUG" }
			flags       { "Symbols" }

		configuration "Release"
			targetdir   "bin/release"
			defines     "NDEBUG"
			flags       { "OptimizeSize" }

		configuration "vs*"
			defines     { "_CRT_SECURE_NO_WARNINGS" }

		configuration "vs2005"
			defines	{"_CRT_SECURE_NO_DEPRECATE" }

		configuration "windows"
			links { "ole32" }

		configuration "linux or bsd or hurd"
			links       { "m" }
			linkoptions { "-rdynamic" }

		configuration "linux or hurd"
			links       { "dl" }

		configuration "macosx"
			links       { "CoreServices.framework" , "Foundation.framework"}

		configuration { "macosx", "gmake" }
			toolset "clang"
			buildoptions { "-mmacosx-version-min=10.4" }
			linkoptions  { "-mmacosx-version-min=10.4" }

		configuration { "solaris" }
			linkoptions { "-Wl,--export-dynamic" }

		configuration "aix"
			links       { "m" }

		configuration( nil )

	ghl_disable_media = true
	use_network = false
	lua_build_all = true

	dofile('lua.lua')
	dofile('ghl.lua')
	dofile('spine-runtime.lua')
	dofile('pugixml.lua')
	dofile('yajl.lua')

	project "AssetsBuilder"
		targetname  "assetsbuilder"
		language    "C++"
		kind        "ConsoleApp"
		
		includedirs { 
			path.join(sandbox_dir,'lua','src'),
			path.join(sandbox_dir,'include'),
			path.join(sandbox_dir,'ghl','include'),
			path.join(sandbox_dir,'sandbox'),
			path.join(sandbox_dir,'spine-runtime-c','include'),
			path.join(sandbox_dir,'pugixml','src'),
		}

		
		files
		{
			"*.txt", "**.lua",
			"src/**.h", "src/**.c",
			"src/**.cpp",
			"../premake5/src/host/path_*.c",
			"../premake5/src/host/string_*.c",
			"../premake5/src/host/os_*.c",
		}

		

		files(utils.append_path(sandbox_dir .. '/sandbox/',{
			'sb_lua.*',
			'sb_log.*',
			'sb_data.*',
			'sb_lua_context.*',
            'sb_resources.*',
            'sb_texture.*',
            'sb_rendertarget.*',
            'sb_shader.*',
            'sb_bitmask.*',
			'sb_memory_mgr.*',
			'sb_base64.*',
			'sb_file_provider.*',
			'json/sb_json.*',
			'meta/**',
			'luabind/**',
			'skelet/sb_skelet_data.*',
			}))

		links {
			'GHL',
			'lua',
			'spine-runtime',
			'pugixml',
			'yajl',
		} 
		-- excludes
		-- {
		-- 	"src/host/lua-5.1.4/src/lauxlib.c",
		-- 	"src/host/lua-5.1.4/src/lua.c",
		-- 	"src/host/lua-5.1.4/src/luac.c",
		-- 	"src/host/lua-5.1.4/src/print.c",
		-- 	"src/host/lua-5.1.4/**.lua",
		-- 	"src/host/lua-5.1.4/etc/*.c"
		-- }

		



--
-- A more thorough cleanup.
--

	if _ACTION == "clean" then
		os.rmdir("bin")
		os.rmdir("lib")
		os.rmdir("build")
	end
