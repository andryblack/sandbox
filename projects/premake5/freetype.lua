local utils = require 'utils'
project 'freetype'
		kind 'StaticLib'
		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)

	targetname ('freetype-' .. platform_dir)

	local freetype_files = {
		'autofit/afangles.c',
        'autofit/afcjk.c',
        'autofit/afdummy.c',
        'autofit/afglobal.c',
        'autofit/afhints.c',
        'autofit/afindic.c',
        'autofit/aflatin.c',
        'autofit/afloader.c',
        'autofit/afmodule.c',
        'autofit/afpic.c',
        'autofit/afwarp.c',

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
	files(utils.append_path(sandbox_dir .. '/freetype/src/',freetype_files))

	defines 'FT2_BUILD_LIBRARY'
	defines 'DARWIN_NO_CARBON'

	sysincludedirs {
		sandbox_dir .. '/include',
		sandbox_dir .. '/freetype/include'
	}

	configuration "Debug"
			targetsuffix "-debug"