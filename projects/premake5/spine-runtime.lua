local utils = require 'utils'

project 'spine-runtime'

		kind 'StaticLib'

	targetdir (libs_dir)

	targetname ('spine-runtime-' .. platform_dir)

	local srcfiles = {
		'src/spine/*.c',
		'include/spine/*.h'
	}
	
	files(utils.append_path(sandbox_dir .. '/spine-runtime-c/',srcfiles))

	sysincludedirs {
				sandbox_dir .. '/spine-runtime-c/include'
			}

	configuration "Debug"
			targetsuffix "_d"