local utils = require 'utils'
project 'tlsf'
		kind 'StaticLib'
		configure_lib_targetdir()
		targetname ('tlsf-' .. platform_dir)
		files(utils.append_path(sandbox_dir .. '/external/tlsf/',{'*.c','*.h'}))
		