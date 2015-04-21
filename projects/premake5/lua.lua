
local utils = require 'utils'

if not platform_dir then
	platform_dir = os.get()
end

project 'lua'
   		kind 'StaticLib'

		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir)

		targetname ('lua_' .. platform_dir)

		local lua_files = {
			'lapi.c', 'lauxlib.c', 'lbaselib.c', 'lbitlib.c', 'lcode.c', 'lcorolib.c', 'lctype.c',
			'ldblib.c', 'ldebug.c', 'ldo.c', 'ldump.c', 'lfunc.c', 'lgc.c', 'llex.c', 'lmathlib.c',
			'lmem.c', 'loadlib.c', 'lobject.c', 'lopcodes.c', 'lparser.c', 'lstate.c', 'lstring.c',
			'lstrlib.c', 'ltable.c', 'ltablib.c', 'ltm.c', 'lundump.c', 'lvm.c', 'lzio.c'
		}
		if lua_build_all then
			table.insert(lua_files,'loslib.c')
			table.insert(lua_files,'liolib.c')
		end
		files(utils.append_path(sandbox_dir .. '/lua/src/',lua_files))


		configuration "Debug"
   			targetsuffix "_d"