---
-- Premake 5.x build configuration script
-- Use this script to configure the project with Premake5.
---

--
-- Remember my location; I will need it to locate sub-scripts later.
--

	local corePath = _SCRIPT_DIR

	sandbox_dir = path.join(_SCRIPT_DIR,'../..')



--
-- Register supporting actions and options.
--
	local utils = require 'utils'



	newoption {
		trigger = "to",
		value   = "path",
		description = "Set the output location for the generated files"
	}



--
-- Define the project. Put the release configuration first so it will be the
-- default when folks build using the makefile. That way they don't have to
-- worry about the /scripts argument and all that.
--
-- TODO: defaultConfiguration "Release"
--

	solution "AssetsBuilder"
		configurations { "Release", "Debug" }
		location ( _OPTIONS["to"] )

		local ab = require('assetsbuilder')

		ab.configure_solution()

		ab.configure_project()

