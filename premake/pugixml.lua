local utils = require 'utils'
project 'pugixml'
		kind 'StaticLib'
		targetdir(libs_dir)
		targetname ('pugixml-' .. platform_dir)
		files(utils.append_path(sandbox_dir .. '/external/pugixml/src/',{'*.cpp','*.hpp'}))
		configuration "Debug"
   			targetsuffix "-debug"