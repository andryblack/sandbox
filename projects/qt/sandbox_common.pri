SRCDIR=$$PWD/../../sandbox

HEADERS+=\
    $$SRCDIR/sb_lua.h\
    $$SRCDIR/sb_lua_context.h\
    $$SRCDIR/sb_memory_mgr.h\
    $$SRCDIR/sb_log.h\
    $$SRCDIR/sb_file_provider.h

SOURCES+=\
    $$SRCDIR/sb_lua.cpp\
    $$SRCDIR/sb_lua_context.cpp\
    $$SRCDIR/sb_memory_mgr.cpp\
    $$SRCDIR/sb_log.cpp\

SOURCES+=\
    $$SRCDIR/meta/sb_meta.cpp\

SOURCES+=\
    $$SRCDIR/luabind/sb_luabind_metatable.cpp\
    $$SRCDIR/luabind/sb_luabind_ref.cpp\
    $$SRCDIR/luabind/sb_luabind_stack.cpp\
    $$SRCDIR/luabind/sb_luabind_wrapper.cpp\
    $$SRCDIR/luabind/sb_luabind.cpp

