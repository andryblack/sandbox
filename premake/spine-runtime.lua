local utils = require 'utils'

project 'spine-runtime'

		kind 'StaticLib'

	configure_lib_targetdir()

	targetname ('spine-runtime-' .. platform_dir)

	local srcfiles = {
		'src/spine/*.c',
		'include/spine/*.h'
	}
	
	files(utils.append_path(sandbox_dir .. '/external/spine-runtime-c/',srcfiles))

	sysincludedirs {
				sandbox_dir .. '/external/spine-runtime-c/include'
			}

	