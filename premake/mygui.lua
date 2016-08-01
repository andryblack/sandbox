project 'MyGUI'

	kind 'StaticLib'
	configure_lib_targetdir()
	targetname ('MyGUI-' .. platform_dir)

	files {
		sandbox_dir .. '/external/MyGUI/MyGUIEngine/**.h',
		sandbox_dir .. '/external/MyGUI/MyGUIEngine/**.cpp'
	}

	sysincludedirs {
		sandbox_dir .. '/external/MyGUI/MyGUIEngine/include',
		sandbox_dir .. '/include',
	}
