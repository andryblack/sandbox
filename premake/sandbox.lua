
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

	if os.is('emscripten') then
		toolset 'emcc'
	end

	configurations { 'Debug', 'Release' }

	if os.is('android') then
		android_abis(AndroidConfig.abi or {'armeabi'})
		android_stl(AndroidConfig.stl or 'gnustl_static')
		android_activity(AndroidConfig.activity or 'com.sandboxgames.Activity')
		android_libs(path.getabsolute(path.join(sandbox_dir,'GHL/src/android_ghl','src')))
		android_libs(path.getabsolute(path.join(sandbox_dir,'platform/android/libs','sandbox_lib')))
		android_api_level(AndroidConfig.api_level or 9)
		android_target_api_level(AndroidConfig.target_api_level or 14)
		android_build_api_level(AndroidConfig.build_api_level or AndroidConfig.target_api_level or 14)
		android_packagename( AndroidConfig.package or 'com.sandbox.example')
		android_toolchain( AndroidConfig.toolchain or '4.9' )
		android_screenorientation( AndroidConfig.screenorientation or 'landscape' )
		android_packageversion( AndroidConfig.versioncode or 1)
		android_packageversionname( AndroidConfig.versionname or "1.0" )

		local firebase_version = AndroidConfig.firebase_version 
		local play_version = AndroidConfig.play_version or '15.0.0'

		if AndroidConfig.manifest then
			android_manifest(path.getabsolute(path.join(_WORKING_DIR,AndroidConfig.manifest)))
		end
		for _,v in ipairs(AndroidConfig.receivers or {} ) do
			android_receiver( v )
		end
		
		if use.AndroidGooglePlayService or use.IAP then
			
			android_dependencies('com.google.android.gms:play-services-base:' .. play_version)
			if use.AndroidGooglePlayService then
				android_dependencies('com.google.android.gms:play-services-auth:' .. play_version)
				android_dependencies('com.google.android.gms:play-services-games:' .. play_version)
				android_dependencies('com.android.support:support-v4:27.0.2')
			end
			
			android_metadata {
				'com.google.android.gms.games.APP_ID=@string/google_play_services_app_id',
				'com.google.android.gms.version=@integer/google_play_services_version'
			}
		end

		if use.AndroidPN then
			android_module{fcm=true}
			android_dependencies('com.google.firebase:firebase-core:' .. ((firebase_version and firebase_version.core) or '16.0.1'))
			android_dependencies('com.google.firebase:firebase-messaging:' .. ((firebase_version and firebase_version.messaging) or '17.1.0'))

			android_service {
				{
					name = 'com.sandbox.SBFirebaseInstanceIDService',
					intent_filter = {
						{
							type = 'action',
							name = 'com.google.firebase.INSTANCE_ID_EVENT'
						}
					}
				},
			}

			android_libs(path.getabsolute(path.join(sandbox_dir,'platform/android/libs','pn_sandbox_lib','src')))
		end
		if use.IAP then
			android_libs(path.getabsolute(path.join(sandbox_dir,'platform/android/libs','iap_sandbox_lib','src')))
			android_aidl(path.getabsolute(path.join(sandbox_dir,'platform/android/libs','iap_sandbox_lib','aidl')))
		end
		if use.AndroidGooglePlayService then
			android_module{gps=true}
			android_libs(path.getabsolute(path.join(sandbox_dir,'platform/android/libs','gps_sandbox_lib','src')))
		end
		if AndroidConfig.permissions then
			android_permissions( AndroidConfig.permissions )
		end
		if use.IAP then
			android_permissions( 'com.android.vending.BILLING' )
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
	elseif platform_id == 'emscripten' then
		buildoptions { '-s NO_EXIT_RUNTIME=1' }
	elseif platform_id ~= 'windows' then
		table.insert(hide_options,'-fvisibility=hidden')
	end


	if platform_id == 'ios' then
		defines { 'GHL_PLATFORM_IOS' }
		xcodebuildsettings { 
			SDKROOT = 'iphoneos' , 
			CODE_SIGN_IDENTITY='iPhone Developer', 
			--@todo fixit
			ARCHS="$(ARCHS_STANDARD)",
			IPHONEOS_DEPLOYMENT_TARGET='7.0'
		}
		buildoptions{'-Wno-undefined-var-template'}
	elseif platform_id == 'macosx' then
		buildoptions{'-Wno-undefined-var-template'}
	elseif platform_id == 'windows' then
		flags{ 'StaticRuntime' }
	end

	if solution_config then
		solution_config()
	end

	buildoptions( hide_options )

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
    elseif platform_id == 'emscripten' then
    	buildoptions { '-O2' }
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

	function configure_lib_targetdir()
		configuration 'Debug'
		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir .. '/' .. 'debug')
		configuration 'Release'
		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir .. '/' .. 'release')
		configuration {}

		if os.is('ios') then
			xcodebuildsettings {
				SKIP_INSTALL='YES',
			}
		end
	end
	language "C++"

	

	dofile('ghl.lua')

   	if use.Chipmunk then
		dofile('chipmunk.lua')
	end
	
	dofile('pugixml.lua')
    dofile('lua.lua')
  	dofile('yajl.lua')
  	dofile('tlsf.lua')
   	
   	if use.Freetype then
	   	dofile('freetype.lua')
	end

	if use.MyGUI then
		dofile('mygui.lua')
	end

	if use.Spine then
		dofile('spine-runtime.lua')
	end


	if pre_project then
		pre_project()
	end

	project 'Sandbox'

		kind 'StaticLib'

		configure_lib_targetdir()
		

		targetname ('Sandbox-' .. platform_dir)

		
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
			sandbox_dir .. '/sandbox/skelet/**.h',
			sandbox_dir .. '/sandbox/skelet/**.cpp',
			sandbox_dir .. '/sandbox/utils/**.cpp',
			sandbox_dir .. '/sandbox/utils/**.c',
			sandbox_dir .. '/sandbox/utils/**.h',
		}

		if use.Freetype then
	   		files { 
	   			sandbox_dir .. '/sandbox/freetype/**.h',
	   			sandbox_dir .. '/sandbox/freetype/**.cpp',
	   		}
		end

		sysincludedirs {
			sandbox_dir .. '/GHL/include',
			sandbox_dir .. '/include',
			
			sandbox_dir .. '/external/freetype/include',
			sandbox_dir .. '/external/yajl/src/api',
			sandbox_dir .. '/external/pugixml/src',
			sandbox_dir .. '/external/tlsf'
		}

		includedirs {
			sandbox_dir .. '/sandbox',
		}

		if use.MyGUI then
			files {
				sandbox_dir .. '/sandbox/mygui/**.h',
				sandbox_dir .. '/sandbox/mygui/**.cpp',
			}
			includedirs {
				sandbox_dir .. '/external/MyGUI/MyGUIEngine/include',
			}
			defines 'SB_USE_MYGUI'
		end


		if use.Chipmunk then
			files {
				sandbox_dir .. '/sandbox/chipmunk/*.cpp',
				sandbox_dir .. '/sandbox/chipmunk/*.h',
			}
		end

		if use.Spine then
			files {
				sandbox_dir .. '/sandbox/spine/**.h',
				sandbox_dir .. '/sandbox/spine/**.cpp',
			}
			sysincludedirs {
				sandbox_dir .. '/external/spine-runtime-c/include',
			}
		end


		if use_network then
			files {
				sandbox_dir .. '/sandbox/net/**.cpp',
				sandbox_dir .. '/sandbox/net/**.h',
			}
			defines 'SB_USE_NETWORK'
		end

		if os.is('ios') then
			includedirs { sandbox_dir .. '/platform/ios' }
			files { sandbox_dir .. '/platform/ios/sb_ios_extension.*',
					sandbox_dir .. '/platform/ios/main.*',}
			if UseModules.IAP then
				files { sandbox_dir .. '/platform/ios/sb_iap_extension.*',}
			end
			if UseModules.iOSGC then
				files { sandbox_dir .. '/platform/ios/gc_extension.*',}
			end
			if UseModules.iOSPN then
				files { sandbox_dir .. '/platform/ios/pn_extension.*',}
			end
		elseif os.is('macosx') then
			files { sandbox_dir .. '/platform/osx/main.mm',
					sandbox_dir .. '/platform/osx/*.cpp',
					sandbox_dir .. '/platform/osx/*.h' }
		elseif os.is('flash') then
			files { sandbox_dir .. '/platform/flash/*.cpp' }
		elseif os.is('windows') then
			files { sandbox_dir .. '/platform/windows/*.cpp' }
		elseif os.is('android') then
			files { sandbox_dir .. '/platform/android/sb_android_extension.cpp' }
			files { sandbox_dir .. '/platform/android/jni_utils.cpp' }
			if use.AndroidGooglePlayService  then
				files { sandbox_dir .. '/platform/android/gps_extension.cpp' }
			end
			if use.IAP then
				files { sandbox_dir .. '/platform/android/iap_extension.cpp' }
			end
			if use.AndroidPN then
				files { sandbox_dir .. '/platform/android/pn_extension.*',}
			end
		elseif os.is( 'emscripten' ) then
			files { sandbox_dir .. '/platform/emscripten/*.cpp' }
		end

		configuration "Debug"
   			defines 'SB_DEBUG'

	project( ProjectName )

		kind 'WindowedApp'

		targetdir (_WORKING_DIR .. '/bin/' .. platform_dir)

		configuration 'Debug'
		libdirs { _WORKING_DIR .. '/lib/' .. platform_dir .. '/debug' }
		configuration 'Release'
		libdirs { _WORKING_DIR .. '/lib/' .. platform_dir .. '/release' }
		configuration {}


		links( {
			'Sandbox', 
			'pugixml',
			'lua', 
			'yajl',
			'tlsf',
			'GHL'
		} )

		links(ghl_links)

		if use.MyGUI then
			links { 'MyGUI' }
			includedirs { sandbox_dir .. '/external/MyGUI/MyGUIEngine/include' }
			defines 'SB_USE_MYGUI'
		end
		if use.Freetype then
			links { 'freetype' }
		end
		if use.Chipmunk then
			links { 'chipmunk' }
			includedirs { sandbox_dir .. '/external/chipmunk/include' }
		end
		if use.Spine then
			links { 'spine-runtime' }
			sysincludedirs {
				sandbox_dir .. '/external/spine-runtime-c/include',
			}
		end
		if os.is('ios') then
			xcodebuildsettings {
				INSTALL_PATH='/Applications'
			}
			links {
				'Foundation.framework', 
				'QuartzCore.framework', 
				'AVFoundation.framework', 
				'UIKit.framework',  
				'OpenGLES.framework', 
				'OpenAL.framework',
				'AudioToolbox.framework',
				'CoreMotion.framework',
				'StoreKit.framework' }
			if use.iOSGC then
				links {
					'GameKit.framework'
				}
			end
		elseif os.is('macosx') then
			links { 
				'OpenGL.framework', 
				'OpenAL.framework',
				'Cocoa.framework',
				'AudioToolbox.framework' }

		elseif os.is('flash') then
			links {
				'AS3++',
				'Flash++'
			}
		elseif os.is('windows') then
			
			links {
				'OpenGL32',
				'WinMM'
			}
			if use_network then
				links { 'Winhttp', }
			end
		elseif os.is('android') then
			files { sandbox_dir .. '/platform/android/main.cpp' }
			links {
				'atomic',
				'android',
				'jnigraphics',
				'log',
				'EGL',
				'OpenSLES',
				'GLESv1_CM',
				'GLESv2'
			}
			
		elseif os.is('emscripten') then
			
			links {
				'SDL'
			}
			linkoptions  {
				'-s USE_SDL=2',
				'-s FULL_ES2=1'
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

		sysincludedirs {
			sandbox_dir .. '/GHL/include',
			sandbox_dir .. '/include',
			sandbox_dir .. '/sandbox',
		}


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
			if os.is('android') then
				android_key_store(path.getabsolute(path.join(_WORKING_DIR,AndroidConfig.keystore)))
				android_key_alias(AndroidConfig.keyalias)
			end

		
		configuration "Debug"
   			defines 'SB_DEBUG'
   			if os.is('flash') then
				linkoptions  {
					'-g'
				}
			end
		configuration {}
