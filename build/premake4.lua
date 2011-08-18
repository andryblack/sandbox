
GHL="../../GHL"
LUA="../lua"

LUASOURCES={"lapi.c","lbaselib.c","lcode.c","ldo.c","ldump.c",
    "lfunc.c","lgc.c","llex.c","lmathlib.c","lmem.c","lobject.c","lopcodes.c",
    "lparser.c","lstate.c","lstring.c","lstrlib.c","ltable.c","ltablib.c",
    "ltm.c","lundump.c","lvm.c","lzio.c","lauxlib.c","ldebug.c","ldblib.c"}

for i,v in ipairs(LUASOURCES) do
    LUASOURCES[i]=LUA.."/src/"..v
end

solution "Sandbox"
	configurations { "Debug", "Release" }
	
	project "Sandbox"
	    language( "C++" )
	    kind "StaticLib"
	    
	    
	    includedirs { GHL.."/include" }
	    includedirs { LUA.."/src" }
	    
	    files { "../sandbox/*.h" , "../sandbox/*.cpp" }
	    files( LUASOURCES )

	    configuration "Debug"
		defines { "SB_DEBUG" , "DEBUG" }
		flags { "Symbols" }
		objdir( ".obj/debug" )
		
	    configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
		objdir( ".obj/release" )