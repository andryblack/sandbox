project 'chipmunk'
			kind 'StaticLib'
			targetdir(libs_dir)
			targetname ('chipmunk-' .. platform_dir)

			buildoptions {'-std=c99' }

			files {
				sandbox_dir .. '/chipmunk/include/**.h',
				sandbox_dir .. '/chipmunk/src/**.c'
			}

			includedirs {
				sandbox_dir .. '/chipmunk/include/chipmunk'
			}

			configuration "Debug"
	   			targetsuffix "-debug"