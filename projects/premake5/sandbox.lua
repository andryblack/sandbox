
math.randomseed(1234)



local use = UseModules or {}
local sandbox_dir = _WORKING_DIR .. '/../..'

if SandboxRoot then
	sandbox_dir = _WORKING_DIR .. '/' .. SandboxRoot
end

flex_sdk_dir = nil
flascc_sdk_dir = nil

if not AndroidConfig then
	AndroidConfig = {}
end


local use_network = use.Network

swf_size = SwfSize or { Width = 800, Height = 600}

solution( ProjectName )
	configurations { 'Debug', 'Release' }

	android_stl('gnustl_static')
	android_api_level(AndroidConfig.api_level or 9)
	android_target_api_level(AndroidConfig.target_api_level or 14)
	android_packagename( AndroidConfig.package or 'com.sandbox.example')
	android_screenorientation( AndroidConfig.screenorientation or 'landscape' )
	
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

	local function append_path( path, files ) 
		local f = {}
		for _,v in ipairs(files) do
			f[#f+1] = path .. v
		end
		return f
	end

	

	print('platform_dir:',platform_dir)

	location ( _WORKING_DIR .. '/projects/' .. platform_dir ) 
	
	objdir( _WORKING_DIR .. '/build/' .. platform_dir )

	language "C++"

	project 'GHL'
		kind 'StaticLib'

		local ghl_src = sandbox_dir .. '/GHL/src/'

		targetdir ('lib/' .. platform_dir)

		targetname ('GHL_' .. platform_dir)

		local zlib_files = { 'inffixed.h', 'inftrees.c', 'inftrees.h', 'adler32.c', 'crc32.c', 'crc32.h', 'crypt.h',
							 'deflate.c', 'deflate.h', 'inffast.c', 'inffast.h', 'inflate.c', 'inflate.h', 'ioapi.h',
							 'zconf.h', 'zip.c', 'zip.h', 'zlib.h', 'zutil.c', 'zutil.h'}

		files(append_path(ghl_src .. '/zlib/',zlib_files))

		local jpeg_files = {
			'jaricom.c', 'jcapimin.c', 'jcapistd.c', 'jcarith.c', 'jccoefct.c','jccolor.c',
			'jcdctmgr.c', 'jchuff.c', 'jcinit.c', 'jcmainct.c', 'jcmarker.c', 'jcmaster.c', 'jcomapi.c', 'jcparam.c',
			'jcprepct.c', 'jcsample.c', 'jctrans.c', 'jdapimin.c', 'jdapistd.c', 'jdarith.c', 'jdatadst.c', 'jdcoefct.c',
			'jdcolor.c', 'jddctmgr.c', 'jdhuff.c', 'jdinput.c', 'jdmainct.c', 'jdmarker.c', 'jdmaster.c', 'jdmerge.c',
			'jdpostct.c', 'jdsample.c', 'jdtrans.c', 'jerror.c', 'jfdctflt.c', 'jfdctfst.c', 'jfdctint.c', 'jidctflt.c',
			'jidctfst.c', 'jidctint.c', 'jmemmgr.c', 'jmemnobs.c', 'jquant1.c', 'jquant2.c', 'jutils.c',	'transupp.c' 
		}
		files(append_path(ghl_src .. '/image/jpeg/',jpeg_files))

		local png_files = {
			'png.c', 'pngerror.c', 'pngget.c', 'pngmem.c', 'pngpread.c', 'pngread.c', 'pngrio.c', 'pngrtran.c',
			'pngrutil.c', 'pngset.c', 'pngtrans.c', 'pngwio.c', 'pngwrite.c', 'pngwtran.c', 'pngwutil.c' 
		}
		files(append_path(ghl_src .. '/image/libpng/',png_files))

		files {
			sandbox_dir .. '/GHL/include/**.h',
			ghl_src .. '*.cpp',
			ghl_src .. '*.h',
			ghl_src .. 'image/*',
			ghl_src .. 'vfs/memory_stream.*',
			ghl_src .. 'sound/ghl_sound_decoder.h',
			ghl_src .. 'sound/ghl_sound_impl.h',
			ghl_src .. 'sound/sound_decoders.cpp',
			ghl_src .. 'sound/wav_decoder.*',
			ghl_src .. 'render/buffer_impl.*',
			ghl_src .. 'render/lucida_console_regular_8.*',
			ghl_src .. 'render/render_impl.*',
			ghl_src .. 'render/rendertarget_impl.*',
			ghl_src .. 'render/shader_impl.*',
			ghl_src .. 'render/texture_impl.*',
			ghl_src .. 'render/pfpl/*'
		}

		local use_openal = false
		local use_opengl = false

		if os.is('ios') or os.is('macosx') then
			use_openal = true
			use_opengl = true
		end

		if os.is('windows') then
			use_opengl = true
		end

		if os.is('android') then
			use_opengl = true
		end

		local use_opengles = use_opengl and ( os.is('ios') or os.is('android') )

		if use_openal then
			files {
				ghl_src .. 'sound/openal/*'
			}
		end

		if use_opengl then
			files {
				ghl_src .. 'render/opengl/buffers_opengl.*',
				ghl_src .. 'render/opengl/glsl_generator.*',
				ghl_src .. 'render/opengl/render_opengl_api.h',
				ghl_src .. 'render/opengl/render_opengl_base.*',
				ghl_src .. 'render/opengl/rendertarget_opengl.*',
				ghl_src .. 'render/opengl/shader_glsl.*',
				ghl_src .. 'render/opengl/texture_opengl.*',
			}
			if use_opengles then
				files {
					ghl_src .. 'render/opengl/gles1_api.*',
					ghl_src .. 'render/opengl/gles2_api.*',
					ghl_src .. 'render/opengl/render_opengles.*',
				}
			else
				files {
					ghl_src .. 'render/opengl/dynamic/dynamic_gl.*',
					ghl_src .. 'render/opengl/render_opengl.*',
				}
			end
		end



		if os.is('macosx') then
			files { 
				ghl_src .. 'winlib/winlib_cocoa.*',
				ghl_src .. 'vfs/vfs_cocoa.*',
				ghl_src .. 'sound/cocoa/*'
			}
			if use_network then
				files {
					ghl_src .. 'net/cocoa/ghl_net_cocoa.mm'
				}
			end
		elseif os.is('ios') then
			defines 'GHL_PLATFORM_IOS'
			files {
				ghl_src .. 'winlib/winlib_cocoatouch.*',
				ghl_src .. 'winlib/WinLibCocoaTouchContext.*',
				ghl_src .. 'winlib/WinLibCocoaTouchContext2.*',
				ghl_src .. 'vfs/vfs_cocoa.*',
				ghl_src .. 'sound/cocoa/*'
			}
			if use_network then
				files {
					ghl_src .. 'net/cocoa/ghl_net_cocoa.mm'
				}
			end
		elseif os.is('flash') then
			--defines 'GHL_PLATFORM_FLASH'
			files {
				ghl_src .. 'winlib/winlib_flash.*',
				ghl_src .. 'vfs/vfs_posix.*',
				ghl_src .. 'sound/flash/*',
				ghl_src .. 'render/stage3d/*'
			}
			if use_network then
				files {
					ghl_src .. 'net/flash/ghl_net_flash.cpp'
				}
			end
		elseif os.is('windows') then
			--defines 'GHL_PLATFORM_FLASH'
			files {
				ghl_src .. 'winlib/winlib_win32.*',
				ghl_src .. 'vfs/vfs_win32.*',
				ghl_src .. 'sound/dsound/*',
			}
			if use_network then
				files {
					ghl_src .. 'net/win32/ghl_net_win32.cpp'
				}
			end
		elseif os.is('android') then
			--defines 'GHL_PLATFORM_FLASH'
			files {
				ghl_src .. 'winlib/winlib_android.*',
				ghl_src .. 'vfs/vfs_android.*',
				ghl_src .. 'vfs/vfs_posix.*',
				ghl_src .. 'sound/android/*'
			}
			if use_network then
				files {
					ghl_src .. 'net/android/ghl_net_android.cpp'
				}
			end
		end

		includedirs {
			sandbox_dir .. '/GHL/include'
		}

		configuration "Debug"
   			targetsuffix "_d"
   			defines "GHL_DEBUG"

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

   	project 'lua'
   		kind 'StaticLib'

		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)

		targetname ('lua_' .. platform_dir)

		local lua_files = {
			'lapi.c', 'lauxlib.c', 'lbaselib.c', 'lbitlib.c', 'lcode.c', 'lcorolib.c', 'lctype.c',
			'ldblib.c', 'ldebug.c', 'ldo.c', 'ldump.c', 'lfunc.c', 'lgc.c', 'llex.c', 'lmathlib.c',
			'lmem.c', 'loadlib.c', 'lobject.c', 'lopcodes.c', 'lparser.c', 'lstate.c', 'lstring.c',
			'lstrlib.c', 'ltable.c', 'ltablib.c', 'ltm.c', 'lundump.c', 'lvm.c', 'lzio.c'
		}
		files(append_path(sandbox_dir .. '/lua/src/',lua_files))


		configuration "Debug"
   			targetsuffix "_d"

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
			sandbox_dir .. '/yajl/src/api'
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
			files { sandbox_dir .. '/projects/ios/main.mm' }
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
			files { sandbox_dir .. '/projects/osx/main.mm' }
			links { 
				'OpenGL.framework', 
				'OpenAL.framework',
				'Cocoa.framework',
				'AudioToolbox.framework' }
		elseif os.is('flash') then
			files { sandbox_dir .. '/projects/flash/main.cpp' }
			links {
				'AS3++',
				'Flash++'
			}
		elseif os.is('windows') then
			files { sandbox_dir .. '/projects/windows/main.cpp' }
			links {
				'OpenGL32',
				'WinMM'
			}
		elseif os.is('android') then
			files { sandbox_dir .. '/projects/android/main.cpp' }
			links {
				'android',
				'log',
				'EGL',
				'OpenSLES',
				'GLESv1_CM',
				'GLESv2'
			}
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
