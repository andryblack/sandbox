project 'MyGUI'

	kind 'StaticLib'

	targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)

	targetname ('MyGUI-' .. platform_dir)

	files {
		sandbox_dir .. '/MyGUI/MyGUIEngine/**.h',
		sandbox_dir .. '/MyGUI/MyGUIEngine/**.cpp'
	}

	includedirs {
		sandbox_dir .. '/MyGUI/MyGUIEngine/include',
		sandbox_dir .. '/include',
		sandbox_dir .. '/freetype/include'
	}

	if os.is('windows') then
		defines 'MYGUI_CONFIG_INCLUDE="../../../sandbox/mygui/sb_mygui_config.h"'
	else
		defines 'MYGUI_CONFIG_INCLUDE="<../../../sandbox/mygui/sb_mygui_config.h>"'
	end

	configuration "Debug"
			targetsuffix "-debug"