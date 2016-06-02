project 'chipmunk'
			kind 'StaticLib'
			targetdir(libs_dir)
			targetname ('chipmunk-' .. platform_dir)

			buildoptions {'-std=c99' }

			files {
				sandbox_dir .. '/external/chipmunk/include/**.h',
				sandbox_dir .. '/external/chipmunk/src/**.c'
			}

			includedirs {
				sandbox_dir .. '/external/chipmunk/include/chipmunk'
			}

			configuration "Debug"
	   			targetsuffix "-debug"