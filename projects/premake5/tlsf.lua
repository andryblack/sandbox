local utils = require 'utils'
project 'tlsf'
		kind 'StaticLib'
		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)
		targetname ('tlsf-' .. platform_dir)
		files(utils.append_path(sandbox_dir .. '/tlsf/',{'*.c','*.h'}))
		configuration "Debug"
   			targetsuffix "-debug"