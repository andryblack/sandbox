---


	
--
-- Register supporting actions and options.
--
	local utils = require 'utils'

	local _M = {}

	ghl_disable_media = true
	use_network = false
	lua_build_all = true

	function _M.configure_solution( data )

		defines {
			'GHL_STATIC_LIB','GHL_BUILD_TOOLS'
		}


		flags       { "No64BitChecks", "ExtraWarnings", "StaticRuntime" }

		if data and data.flags then
			flags(data.flags)
		end

		if data and data.xcodebuildsettings then
			xcodebuildsettings(data.xcodebuildsettings)
		end

		configuration "Debug"
			targetdir   "bin/debug"
			defines     { "_DEBUG" , "SB_DEBUG" }
			flags       { "Symbols" }

		configuration "Release"
			targetdir   "bin/release"
			defines     "NDEBUG"
			defines 	"SB_SILENT"
			defines 	"GHL_SILENT"
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

		--configuration "macosx"
		--	links       { "CoreServices.framework" , "Foundation.framework"}

		configuration { "macosx", "gmake" }
			toolset "clang"
			buildoptions { "-mmacosx-version-min=10.8" }
			linkoptions  { "-mmacosx-version-min=10.8" }

		configuration { "linux" }
			links {"pthread"}
		
		configuration{}

	

		dofile('lua.lua')
		dofile('ghl.lua')
		dofile('spine-runtime.lua')
		dofile('pugixml.lua')
		dofile('yajl.lua')
		dofile('tlsf.lua')
	end

	function _M.premake_files( ... )
		
		files(utils.append_path(sandbox_dir .. '/utils/premake5/src/host/',{
			"path_*.c",
			"string_*.c",
			"os_chdir.c",
			"os_chmod.c",
			"os_copyfile.c",
			"os_pathsearch.c",
			"os_match.c",
			"os_stat.c",
			"os_rmdir.c",
			"os_getcwd.c",
			"os_isfile.c",
			"os_uuid.c",
			"os_realpath.c",
			"os_mkdir.c",
			"os_getversion.c",
			"os_is64bit.c",
			"os_isdir.c",
			"os_islink.c",
		}))
		
	end

	function _M.configure_project()
	
	project 'AssetsBuilder'

		targetname  "assetsbuilder"
		language    "C++"
		kind        "ConsoleApp"
		
		includedirs { 
			
			
			path.join(sandbox_dir,'sandbox'),
			
			
		}


		sysincludedirs {
			path.join(sandbox_dir,'include'),
			path.join(sandbox_dir,'include/lua'),
			path.join(sandbox_dir,'GHL','include'),
			path.join(sandbox_dir,'GHL','src','sound','libogg','include'),
			path.join(sandbox_dir,'GHL','src','sound','libvorbis','include'),
			
			path.join(sandbox_dir,'external/pugixml','src'),
			path.join(sandbox_dir,'external/spine-runtime-c','include'),
			path.join(sandbox_dir,'sandbox'),
			path.join(sandbox_dir,'external/tlsf'),
			path.join(sandbox_dir,'GHL','src'),
			path.join(sandbox_dir,'external/rg_etc1'),
		}
		
		files(utils.append_path(sandbox_dir .. '/utils/assetsbuilder/',
		{
			"*.txt", "**.lua",
			"src/**.h", "src/**.c",
			"src/**.cpp",
		}))

		files{
			path.join(sandbox_dir,'GHL','src/winlib/winlib_posix_time.cpp'),
		}

		files{
			path.join(sandbox_dir,'external','rg_etc1','*.cpp'),
			path.join(sandbox_dir,'external','rg_etc1','*.h'),
		}

		_M.premake_files()

		files(utils.append_path(sandbox_dir .. '/sandbox/',{
			'sb_lua.*',
			'sb_lua_io.*',
			'sb_log.*',
			'sb_time.*',
			'sb_data.*',
			'sb_color.*',
			'sb_lua_context.*',
            'sb_resources.*',
            'sb_texture.*',
            'sb_rendertarget.*',
            'sb_shader.*',
            'sb_bitmask.*',
			'sb_memory_mgr.*',
			'sb_file_provider.*',
			'sb_luabind_utils.*',
			'sb_luabind_math.*',
			'sb_rect.*',
			'sb_vector*.*',
			'sb_matrix*.*',
			'json/sb_json.*',
			'meta/**',
			'luabind/**',
			'utils/**',
			}))

		links {
			'GHL',
			'lua',
			'spine-runtime',
			'pugixml',
			'yajl',
			'tlsf'
		} 
	end
	
--

	if _ACTION == "clean" then
		os.rmdir("bin")
		os.rmdir("lib")
		os.rmdir("build")
	end

	return _M
