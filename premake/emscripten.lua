premake.modules.emscripten = {}
local emscripten = premake.modules.emscripten
emscripten.emcc = {}
premake.tools.emcc = emscripten.emcc

local emcc = emscripten.emcc
local clang = premake.tools.clang
local config = premake.config

function emcc.getcppflags(cfg)

		-- Just pass through to Clang for now
	local flags = clang.getcppflags(cfg)
	return flags

end

function emcc.getcflags(cfg)

	-- Just pass through to Clang for now
	local flags = clang.getcflags(cfg)
	return flags

end

function emcc.getcxxflags(cfg)

	-- Just pass through to Clang for now
	local flags = clang.getcxxflags(cfg)
	return flags

end

function emcc.getLibraryDirectories(cfg)

	-- Just pass through to Clang for now
	local flags = clang.getLibraryDirectories(cfg)
	return flags

end

function emcc.getlinks(cfg, systemOnly)

	-- Just pass through to Clang for now
	local flags = clang.getlinks(cfg, systemOnly)
	return flags

end

function emcc.getmakesettings(cfg)

	-- Just pass through to Clang for now
	local flags = clang.getmakesettings(cfg)
	return flags

end

function emcc.getdefines(defines)

	-- Just pass through to Clang for now
	local flags = clang.getdefines(defines)
	return flags

end

function emcc.getundefines(undefines)

	-- Just pass through to Clang for now
	local flags = clang.getundefines(undefines)
	return flags

end

function emcc.getincludedirs(cfg, dirs, sysdirs)

	-- Just pass through to Clang for now
	local flags = clang.getincludedirs(cfg, dirs, sysdirs)
	return flags

end

function emcc.getforceincludes(cfg)

	-- Just pass through to Clang for now
	local flags = clang.getforceincludes(cfg)
	return flags

end

function emcc.getldflags(cfg)
	local flags = clang.getforceincludes(cfg)
	
	return flags
end

emcc.arargs = 'rcu'

emcc.tools = {
		cc = "emcc",
		cxx = "em++",
		ar = "emar"
}

function emcc.gettoolname(cfg, tool)
	return emcc.tools[tool]
end