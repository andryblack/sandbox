local utils = require 'utils'
project 'yajl'
   		kind 'StaticLib'
   		configure_lib_targetdir()
   		targetname ('yajl-' .. platform_dir)
   		local yajl_files = {
			'yajl.c', 'yajl_alloc.c', 'yajl_buf.c', 'yajl_encode.c', 'yajl_gen.c', 'yajl_lex.c', 'yajl_parser.c',
			'yajl_tree.c', 
		}
		sysincludedirs {
				sandbox_dir .. '/include',
		}
		os.mkdir(path.getabsolute(sandbox_dir..'/include/yajl'))
		os.copyfile(path.getabsolute(sandbox_dir..'/external/yajl/src/api') .. '/*.h',
			path.getabsolute(sandbox_dir..'/include/yajl/'))
		
		files(utils.append_path(sandbox_dir .. '/external/yajl/src/',yajl_files))

		disablewarnings{ 'shorten-64-to-32' }
		xcodebuildsettings {
			GCC_WARN_64_TO_32_BIT_CONVERSION='NO'
		}
		
