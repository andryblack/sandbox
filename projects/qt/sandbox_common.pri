SRCDIR=$$PWD/../../sandbox

HEADERS+=\
    $$SRCDIR/sb_lua.h\
    $$SRCDIR/sb_lua_context.h\
    $$SRCDIR/sb_memory_mgr.h\
    $$SRCDIR/sb_log.h\
    $$SRCDIR/sb_file_provider.h \
    sandbox/sandbox/meta/sb_meta_bind.h \
    sandbox/sandbox/luabind/sb_luabind.h \
    sandbox/sandbox/luabind/sb_luabind_stack.h \
    sandbox/sandbox/luabind/impl/sb_luabind_wrapper_base.h \
    sandbox/sandbox/luabind/impl/sb_luabind_registrators.h \
    sandbox/sandbox/luabind/impl/sb_luabind_method.h

SOURCES+=\
    $$SRCDIR/sb_lua.cpp\
    $$SRCDIR/sb_lua_context.cpp\
    $$SRCDIR/sb_memory_mgr.cpp\
    $$SRCDIR/sb_log.cpp\

HEADERS+=\
    $$SRCDIR/meta/sb_meta.h\

SOURCES+=\
    $$SRCDIR/meta/sb_meta.cpp\

SOURCES+=\
    $$SRCDIR/luabind/sb_luabind_metatable.cpp\
    $$SRCDIR/luabind/sb_luabind_ref.cpp\
    $$SRCDIR/luabind/sb_luabind_stack.cpp\
    $$SRCDIR/luabind/sb_luabind_wrapper.cpp\
    $$SRCDIR/luabind/sb_luabind.cpp

