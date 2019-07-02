
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


		local src_dir = path.getabsolute(path.join(lua_root,'src'))
		local dst_dir = path.getabsolute(path.join(sandbox_dir,'include/lua'))
		os.mkdir(dst_dir)
		for _,f in ipairs{'lua.h','lualib.h','lauxlib.h','lauxlib.h','luaconf.h'} do
			assert(os.copyfile(path.join(src_dir,f),path.join(dst_dir,f)))
		end
		
		files(utils.append_path(lua_root .. 'src/',lua_files))
		files{ lua_root .. 'src/*.h' }

		