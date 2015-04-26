
math.randomseed(1234)

local utils = require 'utils'

local use = UseModules or {}
sandbox_dir = _WORKING_DIR .. '/../..'

if SandboxRoot then
	sandbox_dir = _WORKING_DIR .. '/' .. SandboxRoot
end

flex_sdk_dir = nil
flascc_sdk_dir = nil

if not AndroidConfig then
	AndroidConfig = {}
end


use_network = use.Network

swf_size = SwfSize or { Width = 800, Height = 600}

solution( ProjectName )
	configurations { 'Debug', 'Release' }

	if os.is('android') then
		android_stl('gnustl_static')
		android_activity('com.sandboxgames.NativeActivity')
		android_libs(path.getabsolute(path.join(sandbox_dir,'projects/android/libs','sandbox_lib')))
		android_api_level(AndroidConfig.api_level or 9)
		android_target_api_level(AndroidConfig.target_api_level or 14)
		android_packagename( AndroidConfig.package or 'com.sandbox.example')
		android_screenorientation( AndroidConfig.screenorientation or 'landscape' )
		if AndroidConfig.keystore and AndroidConfig.keyalias then
			android_key_store(AndroidConfig.keystore)
			android_key_alias(AndroidConfig.keyalias)
		end
		android_packageversion( AndroidConfig.versioncode or 1)
		android_packageversionname( AndroidConfig.versionname or "1.0" )
		if use.AndroidGooglePlayService then
			android_modules_path( path.getabsolute(sandbox_dir) )
			local sdk_dir = assert(_OPTIONS['android-sdk-dir'])
			android_libs(path.join(sdk_dir,'extras/google/google_play_services/libproject','google-play-services_lib'))
			flags{ "C++11" }
			android_metadata {
				'com.google.android.gms.games.APP_ID=@string/app_id',
				'com.google.android.gms.version=@integer/google_play_services_version'
			}
		end
	end

	local hide_options = {
		'-fvisibility-inlines-hidden'
	}

	platform_dir = unknown
	local os_map = { 
		macosx = 'osx' 
	}
	local platform_id = os.get()


	if platform_id == 'flash' then
		flex_sdk_dir = _OPTIONS['flex-sdk-dir']
		flascc_sdk_dir = _OPTIONS['flascc-sdk-dir']
		if flex_sdk_dir == nil then
			error('must specify flex sdk dir')
		end
		if flascc_sdk_dir == nil then
			error('must specify crossbridge sdk dir')
		end
		gccprefix ( flascc_sdk_dir .. '/usr/bin/' )
		buildoptions {'-Wno-write-strings', '-Wno-trigraphs' }
	else
		table.insert(hide_options,'-fvisibility=hidden')
	end

	buildoptions( hide_options )

	if platform_id == 'ios' then
		defines { 'GHL_PLATFORM_IOS' }
		xcodebuildsettings { 
			SDKROOT = 'iphoneos' , 
			CODE_SIGN_IDENTITY='iPhone Developer', 
			ARCHS='$(ARCHS_STANDARD)',
			IPHONEOS_DEPLOYMENT_TARGET='7.0'
		}
	end


	defines {'GHL_STATIC_LIB'}

	configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
    if platform_id == 'flash' then
    	buildoptions { '-g' }
    end

 
    configuration "Release"
         defines { "NDEBUG" }
    if platform_id == 'flash' then
    	buildoptions { '-O4' }
    else
    	flags { "OptimizeSpeed" }    
   	end

	configuration {}

	platform_dir = os_map[platform_id] or platform_id

	

	local append_path = utils.append_path

	print('platform_dir:',platform_dir)

	local loc = _WORKING_DIR .. '/projects/' .. platform_dir
	location ( loc ) 
	
	objdir( _WORKING_DIR .. '/build/' .. platform_dir )

	language "C++"

	

	dofile('ghl.lua')

   	if use.Chipmunk then
		project 'chipmunk'
			kind 'StaticLib'

			targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)

			targetname ('chipmunk_' .. platform_dir)

			buildoptions {'-std=c99' }

			files {
				sandbox_dir .. '/chipmunk/include/**.h',
				sandbox_dir .. '/chipmunk/src/**.c'
			}

			includedirs {
				sandbox_dir .. '/chipmunk/include/chipmunk'
			}

			configuration "Debug"
	   			targetsuffix "_d"
	end

	project 'pugixml'
		kind 'StaticLib'
		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)
		targetname ('pugixml_' .. platform_dir)
		files(append_path(sandbox_dir .. '/pugixml/src/',{'*.cpp','*.hpp'}))
		configuration "Debug"
   			targetsuffix "_d"

    dofile('lua.lua')
  --  	project 'lua'
  --  		kind 'StaticLib'

		-- targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)

		-- targetname ('lua_' .. platform_dir)

		-- local lua_files = {
		-- 	'lapi.c', 'lauxlib.c', 'lbaselib.c', 'lbitlib.c', 'lcode.c', 'lcorolib.c', 'lctype.c',
		-- 	'ldblib.c', 'ldebug.c', 'ldo.c', 'ldump.c', 'lfunc.c', 'lgc.c', 'llex.c', 'lmathlib.c',
		-- 	'lmem.c', 'loadlib.c', 'lobject.c', 'lopcodes.c', 'lparser.c', 'lstate.c', 'lstring.c',
		-- 	'lstrlib.c', 'ltable.c', 'ltablib.c', 'ltm.c', 'lundump.c', 'lvm.c', 'lzio.c'
		-- }
		-- files(append_path(sandbox_dir .. '/lua/src/',lua_files))


		-- configuration "Debug"
  --  			targetsuffix "_d"

   	project 'yajl'
   		kind 'StaticLib'
   		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)
   		targetname ('yajl_' .. platform_dir)
   		local lua_files = {
			'yajl.c', 'yajl_alloc.c', 'yajl_buf.c', 'yajl_encode.c', 'yajl_gen.c', 'yajl_lex.c', 'yajl_parser.c',
			'yajl_tree.c', 
		}
		includedirs {
				sandbox_dir .. '/include',
		}
		if not os.is('windows') then
			prebuildcommands {
				'mkdir -p ' .. path.getabsolute(sandbox_dir..'/include/yajl'),
				'cp ' .. path.getabsolute(sandbox_dir..'/yajl/src/api') .. '/*.h ' .. path.getabsolute(sandbox_dir..'/include/yajl/')
			}
		end
		files(append_path(sandbox_dir .. '/yajl/src/',lua_files))

		configuration "Debug"
   			targetsuffix "_d"

   	if use.Freetype then
	   	project 'freetype'
	   		kind 'StaticLib'
	   		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)

			targetname ('freetype_' .. platform_dir)

			local freetype_files = {
				'autofit/afangles.c',
	            'autofit/afcjk.c',
	            'autofit/afdummy.c',
	            'autofit/afglobal.c',
	            'autofit/afhints.c',
	            'autofit/afindic.c',
	            'autofit/aflatin.c',
	            'autofit/afloader.c',
	            'autofit/afmodule.c',
	            'autofit/afpic.c',
	            'autofit/afwarp.c',

				'base/ftbase.c',
				'base/ftbitmap.c',
				'base/ftinit.c',
				'base/ftsystem.c',
				'base/ftwinfnt.c',

				'cff/cff.c',
				'raster/raster.c',
				'sfnt/sfnt.c',
				'smooth/smooth.c',
				'truetype/truetype.c',
				'type42/type42.c',
				'winfonts/winfnt.c'
			}
			files(append_path(sandbox_dir .. '/freetype/src/',freetype_files))

			defines 'FT2_BUILD_LIBRARY'
			defines 'DARWIN_NO_CARBON'

			includedirs {
				sandbox_dir .. '/include',
				sandbox_dir .. '/freetype/include'
			}

			configuration "Debug"
	   			targetsuffix "_d"
	end

	if use.MyGUI then
		project 'MyGUI'

			kind 'StaticLib'

			targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)

			targetname ('MyGUI_' .. platform_dir)

			files {
				sandbox_dir .. '/MyGUI/MyGUIEngine/**.h',
				sandbox_dir .. '/MyGUI/MyGUIEngine/**.cpp'
			}

			includedirs {
				sandbox_dir .. '/MyGUI/MyGUIEngine/include',
				sandbox_dir .. '/include',
				sandbox_dir .. '/freetype/include'
			}

			defines 'MYGUI_CONFIG_INCLUDE="<../../../sandbox/mygui/sb_mygui_config.h>"'

			configuration "Debug"
	   			targetsuffix "_d"
	end

	project 'Sandbox'

		kind 'StaticLib'

		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)

		targetname ('Sandbox_' .. platform_dir)

		
		files {
			sandbox_dir .. '/include/**.h',
			sandbox_dir .. '/sandbox/*.h',
			sandbox_dir .. '/sandbox/*.cpp',
			sandbox_dir .. '/sandbox/luabind/**.h',
			sandbox_dir .. '/sandbox/luabind/**.cpp',
			sandbox_dir .. '/sandbox/meta/**.h',
			sandbox_dir .. '/sandbox/meta/**.cpp',
			sandbox_dir .. '/sandbox/json/**.h',
			sandbox_dir .. '/sandbox/json/**.cpp',
		}

		includedirs {
			sandbox_dir .. '/GHL/include',
			sandbox_dir .. '/include',
			sandbox_dir .. '/sandbox',
			sandbox_dir .. '/freetype/include',
			sandbox_dir .. '/yajl/src/api',
			sandbox_dir .. '/pugixml/src',
			sandbox_dir .. '/lua/src'
		}

		if use.MyGUI then
			includedirs {
				sandbox_dir .. '/MyGUI/MyGUIEngine/include',
			}
			defines 'MYGUI_CONFIG_INCLUDE="<mygui/sb_mygui_config.h>"'
		end

		if use.Chipmunk then
			files {
				sandbox_dir .. '/sandbox/chipmunk/*.cpp',
				sandbox_dir .. '/sandbox/chipmunk/*.h',
			}
		end

		if use_network then
			files {
				sandbox_dir .. '/sandbox/net/**.cpp',
				sandbox_dir .. '/sandbox/net/**.h',
			}
			defines 'SB_USE_NETWORK'
		end


		configuration "Debug"
   			targetsuffix "_d"
   			defines 'SB_DEBUG'

	project( ProjectName )

		kind 'WindowedApp'

		targetdir (_WORKING_DIR .. '/bin/' .. platform_dir)

		libdirs { _WORKING_DIR .. '/lib/' .. platform_dir }

		local libs_postfix = ''
		if os.is('macosx') then
			libs_postfix = '-OSX'
		end

		links( {
			'Sandbox', 
			'pugixml',
			'lua', 
			'yajl',
			'GHL'
		} )

		if use.MyGUI then
			links { 'MyGUI' }
		end
		if use.Freetype then
			links { 'freetype' }
		end
		if use.Chipmunk then
			links { 'chipmunk' }
		end
		if os.is('ios') then
			files { sandbox_dir .. '/projects/osx/main.mm',
					sandbox_dir .. '/projects/osx/*.cpp',
					sandbox_dir .. '/projects/osx/*.h' }
			links {
				'Foundation.framework', 
				'QuartzCore.framework', 
				'AVFoundation.framework', 
				'UIKit.framework',  
				'OpenGLES.framework', 
				'OpenAL.framework',
				'AudioToolbox.framework',
				'CoreMotion.framework' }
		elseif os.is('macosx') then
			files { sandbox_dir .. '/projects/osx/main.mm',
					sandbox_dir .. '/projects/osx/*.cpp',
					sandbox_dir .. '/projects/osx/*.h' }
			links { 
				'OpenGL.framework', 
				'OpenAL.framework',
				'Cocoa.framework',
				'AudioToolbox.framework' }
		elseif os.is('flash') then
			files { sandbox_dir .. '/projects/flash/*.cpp' }
			links {
				'AS3++',
				'Flash++'
			}
		elseif os.is('windows') then
			files { sandbox_dir .. '/projects/windows/*.cpp' }
			links {
				'OpenGL32',
				'WinMM'
			}
		elseif os.is('android') then
			files { sandbox_dir .. '/projects/android/main.cpp',
					sandbox_dir .. '/projects/android/sb_android_extension.cpp' }
			links {
				'android',
				'log',
				'EGL',
				'OpenSLES',
				'GLESv1_CM',
				'GLESv2'
			}
			if use.AndroidGooglePlayService then
				files { sandbox_dir .. '/projects/android/gps_extension.cpp' }
				includedirs { sandbox_dir .. '/gpg-cpp-sdk/android/include' }
				android_ndk_static_libs {
					'gpg-1',
				}
				android_ndk_modules {
					'gpg-cpp-sdk/android'
				}
			end
		end


		-- files {
		-- 	_WORKING_DIR .. '/src/**.h',
		-- 	_WORKING_DIR .. '/src/**.cpp'
		-- }

		-- resourcefolders {
		-- 	_WORKING_DIR .. '/data'
		-- }

		-- if os.is('macosx') then
		-- 	files { 
		-- 		_WORKING_DIR .. '/projects/osx/main.mm',
		-- 		_WORKING_DIR .. '/projects/osx/' .. ProjectName .. '_Mac-Info.plist'
		-- 	}
		-- 	prebuildcommands { "touch " .. path.getabsolute( _WORKING_DIR .. '/data') }
		-- elseif os.is('ios') then
		-- 	files { 
		-- 		_WORKING_DIR .. '/projects/ios/main.mm',
		-- 		_WORKING_DIR .. '/projects/ios/'..ProjectName..'_iOS-Info.plist',
		-- 		_WORKING_DIR .. '/projects/ios/Default@2x.png',
		-- 		_WORKING_DIR .. '/projects/ios/Default-568h@2x.png',
		-- 	}
		-- 	prebuildcommands { "touch " .. path.getabsolute(_WORKING_DIR .. '/data') }
		-- elseif os.is('flash') then
		-- 	targetextension( '.swf' )
		-- 	files { 
		-- 		_WORKING_DIR .. '/projects/flash/main.cpp',
		-- 	}
		-- 	prelinkcommands { 
		-- 		'rm -f ' .. path.getabsolute( _WORKING_DIR .. '/bin' ) .. '/flash/' .. ProjectName .. '.swf',
		-- 		flascc_sdk_dir .. '/usr/bin/genfs --type=embed ' .. path.getabsolute('data') .. ' ' .. path.getabsolute(_WORKING_DIR ..'/build/' .. platform_dir .. '/data') ,
		-- 		[[java $(JVMARGS) -jar ]] .. flascc_sdk_dir .. [[/usr/lib/asc2.jar -merge -md \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/builtin.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/playerglobal.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/BinaryData.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/ISpecialFile.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/IBackingStore.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/IVFS.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/InMemoryBackingStore.abc \
		-- 			-import ]]..flascc_sdk_dir..[[/usr/lib/PlayerKernel.abc \
		-- 			 ]]..flascc_sdk_dir..[[/usr/share/LSOBackingStore.as \
		-- 			 ]]..path.getabsolute( _WORKING_DIR .. '/' .. ghl_src ) .. [[/flash/Console.as \
		-- 			 ]]..path.getabsolute('build/' .. platform_dir .. '/data')..[[*.as -outdir ]]..path.getabsolute(_WORKING_DIR .. '/build/' .. platform_dir )..[[ -out Console
		-- 		]]
		-- 	}


		-- 	linkoptions  {
		-- 			'-jvmopt="-Xmx4096M"',
		-- 			'-emit-swf',
		-- 			'-swf-size=1024x768',
		-- 			'-flto-api=exports.txt',
		-- 			flascc_sdk_dir .. '/usr/lib/AlcVFSZip.abc',
		-- 			'-symbol-abc=' .. path.getabsolute(_WORKING_DIR .. '/build') .. '/' .. platform_dir .. '/Console.abc'
		-- 		}
			
			
		-- end

		includedirs {
			sandbox_dir .. '/GHL/include',
			sandbox_dir .. '/include',
			sandbox_dir .. '/sandbox',
		}

		if use.Chipmunk then
			includedirs { sandbox_dir .. '/chipmunk/include' }
		end

		if use.MyGUI then
			includedirs { sandbox_dir .. '/MyGUI/MyGUIEngine/include' }
			defines 'MYGUI_CONFIG_INCLUDE="<mygui/sb_mygui_config.h>"'
		end

		if use_network then
			defines 'SB_USE_NETWORK'
		end

		linkoptions( hide_options )

		configuration "Release"
			if os.is('flash') then
				linkoptions  {
					'-O4'
				}
			end
		
		configuration "Debug"
   			defines 'SB_DEBUG'
   			if os.is('flash') then
				linkoptions  {
					'-g'
				}
			end
		configuration {}
