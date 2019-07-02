local utils = require 'utils'
project 'yajl'
   		kind 'StaticLib'
   		configure_lib_targetdir()
   		targetname ('yajl-' .. platform_dir)
   		buildoptions{'-O3'}
   		local yajl_files = {
			'yajl.c', 'yajl_alloc.c', 'yajl_buf.c', 'yajl_encode.c', 'yajl_gen.c', 'yajl_lex.c', 'yajl_parser.c',
			'yajl_tree.c', 
		}
		sysincludedirs {
				sandbox_dir .. '/include',
		}
		local dst_dir = path.getabsolute(path.join(sandbox_dir,'include/yajl'))
		os.mkdir(path.getabsolute(path.join(sandbox_dir,'include/yajl')))
		local src_dir = path.getabsolute(path.join(sandbox_dir,'external/yajl/src/api'))
		local incfiles = {
			'yajl_common.h','yajl_gen.h','yajl_parse.h','yajl_tree.h'
		}
		for _,f in ipairs(incfiles) do
			assert(os.copyfile(path.join(src_dir,f),path.join(dst_dir,f)))
		end
		
		files(utils.append_path(sandbox_dir .. '/external/yajl/src/',yajl_files))

		disablewarnings{ 'shorten-64-to-32' }
		xcodebuildsettings {
			GCC_WARN_64_TO_32_BIT_CONVERSION='NO'
		}
		
