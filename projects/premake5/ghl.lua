local utils = require 'utils'

if not platform_dir then
	platform_dir = os.get()
end

local append_path = utils.append_path

project 'GHL'
		kind 'StaticLib'

		local ghl_src = sandbox_dir .. '/GHL/src/'

		targetdir(libs_dir)
		targetname ('GHL-' .. platform_dir)

		local zlib_files = { 'inffixed.h', 'inftrees.c', 'inftrees.h', 'adler32.c', 'crc32.c', 'crc32.h', 'crypt.h',
							 'deflate.c', 'deflate.h', 'inffast.c', 'inffast.h', 'inflate.c', 'inflate.h', 'ioapi.h',
							 'zconf.h',  'zlib.h', 'zutil.c', 'zutil.h', 'trees.h', 'trees.c', 'compress.c', 'uncompr.c'}

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
			ghl_src .. 'render/pfpl/*'
		}

		local use_openal = false
		local use_opengl = false
		local use_vorbis = true

		if os.is('ios') or os.is('macosx') then
			use_openal = not ghl_disable_media
			use_opengl = not ghl_disable_media
		end

		if os.is('windows') then
			use_opengl = not ghl_disable_media
			use_vorbis = true
		end

		if os.is('android') then
			use_opengl = not ghl_disable_media
		end

		local use_opengles = use_opengl and ( os.is('ios') or os.is('android') )

		if use_openal then
			files {
				ghl_src .. 'sound/openal/*'
			}
		end

		if use_vorbis then

			files {
				ghl_src .. 'sound/vorbis_decoder.*'
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


		if use_vorbis then
			includedirs {
				sandbox_dir .. '/sound/libogg/include',
				sandbox_dir .. '/sound/libvorbis/include'
			}
			local ogg_files = {'bitwise.c','framing.c'}
			files(append_path(ghl_src .. '/sound/libogg/src/',ogg_files))
			local vorbis_files = {'bitrate.c','block.c','codebook.c','envelope.c','floor0.c','floor1.c','info.c','lookup.c',
				'lpc.c','lsp.c','mapping0.c','mdct.c','psy.c','registry.c','res0.c','sharedbook.c','smallft.c','synthesis.c',
				'vorbisfile.c','window.c'}
			files(append_path(ghl_src .. '/sound/libvorbis/lib/',vorbis_files))
		end

		if os.is('macosx') or os.is('ios') then	
			files { ghl_src .. 'vfs/vfs_cocoa.*', }
		elseif os.is('windows') then
			files { ghl_src .. 'vfs/vfs_win32.*', }
		elseif os.is('android') then
			files { ghl_src .. 'vfs/vfs_posix.*',
				ghl_src .. 'vfs/vfs_android.*',}
		else
			files { ghl_src .. 'vfs/vfs_posix.*', }
		end

		if not ghl_disable_media then
			if os.is('macosx') then	
				files { 
					ghl_src .. 'winlib/winlib_cocoa.*',
					ghl_src .. 'winlib/winlib_posix_time.cpp',
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
					ghl_src .. 'winlib/winlib_posix_time.cpp',
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
					ghl_src .. 'sound/flash/*',
					ghl_src .. 'winlib/winlib_posix_time.cpp',
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
					ghl_src .. 'winlib/winlib_posix_time.cpp',
					ghl_src .. 'sound/android/*'
				}
				if use_network then
					files {
						ghl_src .. 'net/android/ghl_net_android.cpp'
					}
				end
			end
		end

		sysincludedirs {
			sandbox_dir .. '/GHL/include'
		}

		configuration "Debug"
   			targetsuffix "-debug"
   			defines "GHL_DEBUG"