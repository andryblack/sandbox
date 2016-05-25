local utils = require 'utils'
project 'yajl'
   		kind 'StaticLib'
   		targetdir(libs_dir)
   		targetname ('yajl-' .. platform_dir)
   		local yajl_files = {
			'yajl.c', 'yajl_alloc.c', 'yajl_buf.c', 'yajl_encode.c', 'yajl_gen.c', 'yajl_lex.c', 'yajl_parser.c',
			'yajl_tree.c', 
		}
		sysincludedirs {
				sandbox_dir .. '/include',
		}
		prebuildcommands {
				'{MKDIR} ' .. path.getabsolute(sandbox_dir..'/include/yajl'),
				'{COPY} ' .. path.getabsolute(sandbox_dir..'/yajl/src/api') .. '/*.h ' .. path.getabsolute(sandbox_dir..'/include/yajl/')
			}
		files(utils.append_path(sandbox_dir .. '/yajl/src/',yajl_files))

		configuration "Debug"
   			targetsuffix "-debug"
