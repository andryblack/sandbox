local utils = require 'utils'
project 'yajl'
   		kind 'StaticLib'
   		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)
   		targetname ('yajl-' .. platform_dir)
   		local yajl_files = {
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
		files(utils.append_path(sandbox_dir .. '/yajl/src/',yajl_files))

		configuration "Debug"
   			targetsuffix "-debug"
