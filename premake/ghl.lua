local utils = require 'utils'

if not platform_dir then
	platform_dir = os.target()
end

local append_path = utils.append_path

local ghl_src = sandbox_dir .. '/GHL/src/'
ghl_links = {}
ghl_includes = {}

project 'GHL-zlib'
		kind 'StaticLib'
		configure_lib_targetdir()
		targetname ('GHL-zlib-' .. platform_dir)
		buildoptions{'-O3'}
		
		local zlib_files = { 'inffixed.h', 'inftrees.c', 'inftrees.h', 'adler32.c', 'crc32.c', 'crc32.h', 'crypt.h',
							 'deflate.c', 'deflate.h', 'inffast.c', 'inffast.h', 'inflate.c', 'inflate.h', 'ioapi.h',
							 'zconf.h',  'zlib.h', 'zutil.c', 'zutil.h', 'trees.h', 'trees.c', 'compress.c', 'uncompr.c'}

		files(append_path(ghl_src .. '/zlib/',zlib_files))
		table.insert(ghl_links,1,'GHL-zlib')

local ghl_defines = {}
local ghl_sysincludes = {}


		if (os.istarget('ios') or os.istarget('macosx')) and not ghl_disable_media  then
			ghl_disable_jpeg = true
		end
		if not ghl_disable_jpeg then
			project 'GHL-jpeg'
			kind 'StaticLib'
			configure_lib_targetdir()
			targetname ('GHL-jpeg-' .. platform_dir)
			buildoptions{'-O3'}

			local jpeg_files = {
				'jaricom.c', 'jcapimin.c', 'jcapistd.c', 'jcarith.c', 'jccoefct.c','jccolor.c',
				'jcdctmgr.c', 'jchuff.c', 'jcinit.c', 'jcmainct.c', 'jcmarker.c', 'jcmaster.c', 'jcomapi.c', 'jcparam.c',
				'jcprepct.c', 'jcsample.c', 'jctrans.c', 'jdapimin.c', 'jdapistd.c', 'jdarith.c', 'jdatadst.c', 'jdcoefct.c',
				'jdcolor.c', 'jddctmgr.c', 'jdhuff.c', 'jdinput.c', 'jdmainct.c', 'jdmarker.c', 'jdmaster.c', 'jdmerge.c',
				'jdpostct.c', 'jdsample.c', 'jdtrans.c', 'jerror.c', 'jfdctflt.c', 'jfdctfst.c', 'jfdctint.c', 'jidctflt.c',
				'jidctfst.c', 'jidctint.c', 'jmemmgr.c', 'jmemnobs.c', 'jquant1.c', 'jquant2.c', 'jutils.c',	'transupp.c' 
			}
			files(append_path(ghl_src .. '/image/jpeg/',jpeg_files))
			table.insert(ghl_links,1,'GHL-jpeg')
		else
			table.insert(ghl_defines, 'GHL_DISABLE_JPEG')
		end

project 'GHL-png'
		kind 'StaticLib'
		configure_lib_targetdir()
		targetname ('GHL-png-' .. platform_dir)
		buildoptions{'-O3'}
		
		local png_files = {
			'png.c', 'pngerror.c', 'pngget.c', 'pngmem.c', 'pngpread.c', 'pngread.c', 'pngrio.c', 'pngrtran.c',
			'pngrutil.c', 'pngset.c', 'pngtrans.c', 'pngwio.c', 'pngwrite.c', 'pngwtran.c', 'pngwutil.c' 
		}
		files(append_path(ghl_src .. '/image/libpng/',png_files))
		table.insert(ghl_links,1,'GHL-png')

project 'GHL-ogg'
		kind 'StaticLib'
		configure_lib_targetdir()
		targetname ('GHL-ogg-' .. platform_dir)
		buildoptions{'-O3'}
		
		sysincludedirs {
			ghl_src .. '/sound/libogg/include'
		}

		local ogg_files = {'bitwise.c','framing.c','*.h'}
		files(append_path(ghl_src .. '/sound/libogg/src/',ogg_files))
		files(ghl_src .. '/sound/libogg/include/**.h')

		table.insert(ghl_sysincludes,ghl_src .. '/sound/libogg/include')

		table.insert(ghl_links,1,'GHL-ogg')

if not ghl_use_tremor then
project 'GHL-vorbis'
		kind 'StaticLib'
		configure_lib_targetdir()
		targetname ('GHL-vorbis-' .. platform_dir)
		buildoptions{'-O3'}
		
		includedirs {
			ghl_src .. '/sound/libvorbis/lib',
		}

		local vorbis_files = {'bitrate.c','block.c','codebook.c','envelope.c','floor0.c','floor1.c','info.c','lookup.c',
				'lpc.c','lsp.c','mapping0.c','mdct.c','psy.c','registry.c','res0.c','sharedbook.c','smallft.c','synthesis.c',
				'vorbisfile.c','window.c','*.h'}
		if build_cli_tools then
			table.insert(vorbis_files,'analysis.c')
			table.insert(vorbis_files,'vorbisenc.c')
			table.insert(vorbis_files,'modes/*.h')
			table.insert(vorbis_files,'books/**.h')
		end

		sysincludedirs {
			ghl_src .. '/sound/libogg/include',
			ghl_src .. '/sound/libvorbis/include'
		}

		files(append_path(ghl_src .. '/sound/libvorbis/lib/',vorbis_files))
		files(ghl_src .. '/sound/libvorbis/include/**.h')
		table.insert(ghl_sysincludes,ghl_src .. '/sound/libvorbis/include')
		table.insert(ghl_links,1,'GHL-vorbis')
		table.insert(ghl_defines, 'GHL_USE_VORBIS_DECODER')
else
project 'GHL-tremor'
		kind 'StaticLib'
		configure_lib_targetdir()
		targetname ('GHL-tremor-' .. platform_dir)
		buildoptions{'-O3'}
		if os.istarget('android') then
			android_ndk_arm_mode(true)
		end
		
		includedirs {
			ghl_src .. '/sound/tremor',
		}

		local tremor_files = {'mdct.c', 'block.c', 'window.c',
                        'synthesis.c', 'info.c',
                        'floor1.c', 'floor0.c', 'vorbisfile.c', 
                        'res012.c', 'mapping0.c', 'registry.c', 'codebook.c',
						'sharedbook.c', '*.h'}
		
		sysincludedirs {
			ghl_src .. '/sound/libogg/include',
		}


		files(utils.append_path(ghl_src .. '/sound/tremor/',tremor_files))
		table.insert(ghl_includes,ghl_src .. '/sound/tremor')
		table.insert(ghl_links,1,'GHL-tremor')
		table.insert(ghl_defines, 'GHL_USE_IVORBIS_DECODER')
end

project 'GHL'
		kind 'StaticLib'

		configure_lib_targetdir()
		
		targetname ('GHL-' .. platform_dir)

		
		if next(ghl_defines) then
			defines(ghl_defines)
		end

		if next(ghl_sysincludes) then
			sysincludedirs(ghl_sysincludes)
		end
		
		includedirs(ghl_src)

		files {
			sandbox_dir .. '/GHL/include/**.h',
			ghl_src .. '*.cpp',
			ghl_src .. '*.h',
			ghl_src .. 'image/*',
			ghl_src .. 'vfs/memory_stream.*',
			ghl_src .. 'vfs/ghl_vfs_impl.*',
			ghl_src .. 'vfs/ghl_vfs_factory.*',
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
			ghl_src .. 'render/pfpl/*',
			ghl_src .. 'font/font_impl.*',
			ghl_src .. 'sound/vorbis_decoder.*',
		}


		local use_openal = false
		local use_opengl = false
		
		if os.istarget('ios') or os.istarget('macosx') then
			if not ghl_disable_media then
				if ghl_use_avaudioengine then
					defines 'GHL_USE_AVAUDIOENGINE'
				else
					use_openal = true
				end
			end
			
			use_opengl = not ghl_disable_media
		end

		if os.istarget('windows') then
			use_opengl = not ghl_disable_media
		end

		if os.istarget('android') then
			use_opengl = not ghl_disable_media
		end

		if os.istarget('emscripten') then
			use_opengl = true
		end

		local use_opengles = use_opengl and ( os.istarget('ios') or os.istarget('android') or os.istarget('emscripten') )

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
				if os.istarget('emscripten') then
					files {
						ghl_src .. 'render/opengl/render_webgl.*',
					}
				end
			else
				files {
					ghl_src .. 'render/opengl/dynamic/dynamic_gl.*',
					ghl_src .. 'render/opengl/render_opengl.*',
				}
			end
		end


		
		if build_cli_tools then
			defines{ 'GHL_BUILD_TOOLS' }
			if os.istarget('windows') then
				files { ghl_src .. 'vfs/vfs_win32.*', }
			else
				files { ghl_src .. 'vfs/vfs_posix.*', 
						ghl_src .. 'vfs/posix_stream.*', 
				}
			end
		else
			if os.istarget('macosx') or os.istarget('ios') then	
				files { ghl_src .. 'vfs/vfs_cocoa.*',
						ghl_src .. 'vfs/posix_stream.*', }
			elseif os.istarget('windows') then
				files { ghl_src .. 'vfs/vfs_win32.*', }
			elseif os.istarget('android') then
				files { ghl_src .. 'vfs/vfs_posix.*',
					ghl_src .. 'vfs/posix_stream.*',
					ghl_src .. 'vfs/vfs_android.*',}
			elseif os.istarget('emscripten') then
				files { ghl_src .. 'vfs/vfs_emscripten.*',
					ghl_src .. 'vfs/vfs_posix.*',
					ghl_src .. 'vfs/posix_stream.*',
					ghl_src .. 'vfs/ghl_vfs_factory.*'}
			else
				files { ghl_src .. 'vfs/vfs_posix.*', 
						ghl_src .. 'vfs/posix_stream.*',}
			end
		end

		if not ghl_disable_media then
			if os.istarget('macosx') then	
				files { 
					ghl_src .. 'winlib/winlib_cocoa.*',
					ghl_src .. 'winlib/winlib_cocoa_time.*',
					ghl_src .. 'sound/cocoa/*',
					ghl_src .. 'font/font_ct.*'
				}
				if use_network then
					files {
						ghl_src .. 'net/cocoa/ghl_net_cocoa.mm'
					}
				end
			elseif os.istarget('ios') then
				defines 'GHL_PLATFORM_IOS'
				files {
					ghl_src .. 'winlib/winlib_cocoatouch.*',
					ghl_src .. 'winlib/cocoatouch_input.*',
					ghl_src .. 'winlib/WinLibCocoaTouchContext.*',
					ghl_src .. 'winlib/WinLibCocoaTouchContext2.*',
					ghl_src .. 'winlib/winlib_cocoa_time.*',
					ghl_src .. 'sound/cocoa/*',
					ghl_src .. 'font/font_ct.*'
				}
				if use_network then
					files {
						ghl_src .. 'net/cocoa/ghl_net_cocoa.mm'
					}
				end
			elseif os.istarget('flash') then
				--defines 'GHL_PLATFORM_FLASH'
				files {
					ghl_src .. 'winlib/winlib_flash.*',
					ghl_src .. 'sound/flash/*',
					ghl_src .. 'winlib/winlib_posix_time.cpp',
					ghl_src .. 'render/stage3d/*'
				}
				if use_network then
					files {
						ghl_src .. 'net/flash/ghl_net_flash.cpp'
					}
				end
			elseif os.istarget('windows') then
				--defines 'GHL_PLATFORM_FLASH'
				files {
					ghl_src .. 'winlib/winlib_win32.*',
					ghl_src .. 'sound/dsound/*',
					ghl_src .. 'winlib/winlib_posix_time.cpp',
				}
				if use_network then
					files {
						ghl_src .. 'net/win32/ghl_net_win32.cpp'
					}
				end
			elseif os.istarget('android') then
				--defines 'GHL_PLATFORM_FLASH'
				files {
					ghl_src .. 'winlib/winlib_android.*',
					ghl_src .. 'winlib/winlib_posix_time.cpp',
					ghl_src .. 'sound/android/*',
					ghl_src .. 'font/font_android.*'
				}
				if use_network then
					files {
						ghl_src .. 'net/android/ghl_net_android.cpp'
					}
				end
			elseif os.istarget('emscripten') then
				defines 'GHL_NO_ES1'
				files {
					ghl_src .. 'winlib/winlib_emscripten.*',
					ghl_src .. 'winlib/winlib_posix_time.cpp',
					ghl_src .. 'sound/emscripten/*',
				}
				if use_network then
					files {
						ghl_src .. 'net/emscripten/ghl_net_emscripten.cpp'
					}
				end
			end
		end

		sysincludedirs {
			sandbox_dir .. '/GHL/include'
		}

		disablewarnings{ 'shorten-64-to-32' }
		xcodebuildsettings {
			GCC_WARN_64_TO_32_BIT_CONVERSION='NO'
		}

		configuration "Debug"
   			defines "GHL_DEBUG"