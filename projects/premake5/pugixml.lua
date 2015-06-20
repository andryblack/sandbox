local utils = require 'utils'
project 'pugixml'
		kind 'StaticLib'
		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)
		targetname ('pugixml-' .. platform_dir)
		files(utils.append_path(sandbox_dir .. '/pugixml/src/',{'*.cpp','*.hpp'}))
		configuration "Debug"
   			targetsuffix "-debug"