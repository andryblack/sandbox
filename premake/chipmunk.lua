project 'chipmunk'
			kind 'StaticLib'
			configure_lib_targetdir()
			targetname ('chipmunk-' .. platform_dir)

			buildoptions {'-std=c99','-O3' }
			
			files {
				sandbox_dir .. '/external/chipmunk/include/**.h',
				sandbox_dir .. '/external/chipmunk/src/**.c'
			}

			includedirs {
				sandbox_dir .. '/external/chipmunk/include/chipmunk'
			}

			