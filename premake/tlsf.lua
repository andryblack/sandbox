local utils = require 'utils'
project 'tlsf'
		kind 'StaticLib'
		targetdir(libs_dir)
		targetname ('tlsf-' .. platform_dir)
		files(utils.append_path(sandbox_dir .. '/external/tlsf/',{'*.c','*.h'}))
		configuration "Debug"
   			targetsuffix "-debug"