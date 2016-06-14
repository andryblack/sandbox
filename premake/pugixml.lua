local utils = require 'utils'
project 'pugixml'
		kind 'StaticLib'
		configure_lib_targetdir()
		targetname ('pugixml-' .. platform_dir)
		files(utils.append_path(sandbox_dir .. '/external/pugixml/src/',{'*.cpp','*.hpp'}))
		