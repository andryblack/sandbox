
local utils = require 'utils'

if not platform_dir then
	platform_dir = os.get()
end

if not configure_lib_targetdir then
	function configure_lib_targetdir(  )
		targetdir (_WORKING_DIR .. '/lib/' .. platform_dir )
	end
end

project 'lua'
   		kind 'StaticLib'
   		configure_lib_targetdir()
		targetname ('lua-' .. platform_dir)

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

		os.mkdir(path.getabsolute(sandbox_dir..'/include/lua'))
		os.copyfile(path.getabsolute(sandbox_dir..'/external/lua/src/lua.h'),path.getabsolute(sandbox_dir..'/include/lua/'))
		os.copyfile(path.getabsolute(sandbox_dir..'/external/lua/src/lualib.h'),path.getabsolute(sandbox_dir..'/include/lua/'))
		os.copyfile(path.getabsolute(sandbox_dir..'/external/lua/src/lauxlib.h'),path.getabsolute(sandbox_dir..'/include/lua/'))
		os.copyfile(path.getabsolute(sandbox_dir..'/external/lua/src/luaconf.h'),path.getabsolute(sandbox_dir..'/include/lua/'))
		
		files(utils.append_path(sandbox_dir .. '/external/lua/src/',lua_files))


		