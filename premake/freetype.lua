local utils = require 'utils'
project 'freetype'
		kind 'StaticLib'
	configure_lib_targetdir()
	targetname ('freetype-' .. platform_dir)
	buildoptions{'-O3'}

	local freetype_files = {
		'autofit/autofit.c',

		'base/ftbase.c',
		'base/ftbitmap.c',
		'base/ftdebug.c',
		'base/ftinit.c',
		'base/ftsystem.c',
		'base/ftwinfnt.c',
		'base/ftstroke.c',
		'base/ftglyph.c',

		'cff/cff.c',
		'raster/raster.c',
		'sfnt/sfnt.c',
		'smooth/smooth.c',
		'truetype/truetype.c',
		'type42/type42.c',
		--'winfonts/winfnt.c',
		'psaux/psaux.c',
		'psnames/psnames.c',
		'pshinter/pshinter.c',
		--'type1/type1.c',
	}
	files(utils.append_path(sandbox_dir .. '/external/freetype/src/',freetype_files))

	defines 'FT2_BUILD_LIBRARY'
	defines 'DARWIN_NO_CARBON'

	sysincludedirs {
		sandbox_dir .. '/include',
		sandbox_dir .. '/external/freetype/include'
	}

	disablewarnings{ 'shorten-64-to-32' }
	xcodebuildsettings {
		GCC_WARN_64_TO_32_BIT_CONVERSION='NO'
	}

