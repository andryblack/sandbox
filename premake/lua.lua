
local utils = require 'utils'

if not platform_dir then
	platform_dir = os.target()
end

local lua_root = external_module_lua or (sandbox_dir .. '/external/lua/')

project 'lua'
   		kind 'StaticLib'
   		configure_lib_targetdir()
		targetname ('lua-' .. platform_dir)

		if build_cli_tools then
			if os.istarget('macosx') then
				defines{ 'LUA_USE_MACOSX' }
			elseif os.istarget('linux') then
				defines{ 'LUA_USE_LINUX' }
			end
		end

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
		os.copyfile(path.getabsolute(lua_root .. 'src/lua.h'),path.getabsolute(sandbox_dir..'/include/lua/'))
		os.copyfile(path.getabsolute(lua_root .. 'src/lualib.h'),path.getabsolute(sandbox_dir..'/include/lua/'))
		os.copyfile(path.getabsolute(lua_root .. 'src/lauxlib.h'),path.getabsolute(sandbox_dir..'/include/lua/'))
		os.copyfile(path.getabsolute(lua_root .. 'src/luaconf.h'),path.getabsolute(sandbox_dir..'/include/lua/'))
		
		files(utils.append_path(lua_root .. 'src/',lua_files))
		files{ lua_root .. 'src/*.h' }

		