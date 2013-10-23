SRCDIR=$$PWD/../../lua/src/
LUA_SOURCES+=\
        $$SRCDIR/lapi.c\
        $$SRCDIR/lauxlib.c\
        $$SRCDIR/lbaselib.c\
        $$SRCDIR/lbitlib.c\
        $$SRCDIR/lcode.c\
        $$SRCDIR/lcorolib.c\
        $$SRCDIR/lctype.c\
        $$SRCDIR/ldblib.c\
        $$SRCDIR/ldebug.c\
        $$SRCDIR/ldo.c\
        $$SRCDIR/ldump.c\
        $$SRCDIR/lfunc.c\
        $$SRCDIR/lgc.c\
        $$SRCDIR/llex.c\
        $$SRCDIR/lmathlib.c\
        $$SRCDIR/lmem.c\
        $$SRCDIR/loadlib.c\
        $$SRCDIR/lobject.c\
        $$SRCDIR/lopcodes.c\
        $$SRCDIR/lparser.c\
        $$SRCDIR/lstate.c\
        $$SRCDIR/lstring.c\
        $$SRCDIR/lstrlib.c\
        $$SRCDIR/ltable.c\
        $$SRCDIR/ltablib.c\
        $$SRCDIR/ltm.c\
        $$SRCDIR/lundump.c\
        $$SRCDIR/lvm.c\
        $$SRCDIR/lzio.c


SOURCES += $$LUA_SOURCES
