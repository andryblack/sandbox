/*
 *  sb_lua_io.cpp
 *  Copyright 2016 andryblack. All rights reserved.
 *
 */


#include "sb_lua.h"

extern "C" {
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include <ghl_types.h>
#include <ghl_data_stream.h>

#include "sb_file_provider.h"

namespace Sandbox {
    
    
    struct FileHandle {
        GHL::DataStream* ds;
    };
    
    
    
    static int lua_io_readline (lua_State *L);
    
    
    static void lua_aux_lines (lua_State *L, int toclose) {
        int i;
        int n = lua_gettop(L) - 1;  /* number of arguments to read */
        /* ensure that arguments will fit here and into 'io_readline' stack */
        luaL_argcheck(L, n <= LUA_MINSTACK - 3, LUA_MINSTACK - 3, "too many options");
        lua_pushvalue(L, 1);  /* file handle */
        lua_pushinteger(L, n);  /* number of arguments to read */
        lua_pushboolean(L, toclose);  /* close/not close file when finished */
        for (i = 1; i <= n; i++) lua_pushvalue(L, i + 1);  /* copy arguments */
        lua_pushcclosure(L, lua_io_readline, 3 + n);
    }
    
    static FileHandle* lua_io_openfile(lua_State*L) {
        const char *filename = luaL_checkstring(L, 1);
        //const char *mode = luaL_optstring(L, 2, "r");
        
        FileProvider* provider = reinterpret_cast<FileProvider*>(lua_touserdata(L, lua_upvalueindex(1)));
        GHL::DataStream* ds = provider->OpenFile(filename);
        if (!ds) {
            return 0;
        }
        FileHandle* handle = (FileHandle*)lua_newuserdata(L, sizeof(FileHandle));
        handle->ds = ds;
        luaL_setmetatable(L, LUA_FILEHANDLE);
        return handle;
        
    }
    
    static int lua_io_open(lua_State* L) {
        FileHandle* handle = lua_io_openfile(L);
        if (!handle) {
            lua_pushboolean(L, 0);
            lua_pushstring(L, luaL_checkstring(L, 1));
            return 2;
        }
        return 1;
    }
    
    static int lua_io_f_lines (lua_State *L) {
        (luaL_checkudata(L, 1, LUA_FILEHANDLE));  /* check that it's a valid file handle */
        lua_aux_lines(L, 0);
        return 1;
    }
    
    
    static int lua_io_lines (lua_State *L) {
        int toclose;
        if (lua_isnone(L, 1)) lua_pushnil(L);  /* at least one argument */
        if (lua_isnil(L, 1)) {  /* no file name? */
            luaL_argerror(L, 1, "need file");
        }
        else {  /* open a new file */
            if (!lua_io_openfile(L)) {
                luaL_argerror(L, 1, luaL_checkstring(L, 1));
            }
            lua_replace(L, 1);  /* put file at index 1 */
            toclose = 1;  /* close it after iteration */
        }
        lua_aux_lines(L, toclose);
        return 1;
    }
    
    
    static int lua_io_close(lua_State* L) {
        FileHandle* handle = ((FileHandle *)luaL_checkudata(L, 1, LUA_FILEHANDLE));
        if (handle->ds) {
            handle->ds->Release();
            handle->ds = 0;
        }
        return 0;
    }
    
#define MAX_SIZE_T	(~(size_t)0)
    
    static void read_all (lua_State *L, GHL::DataStream *f) {
        size_t rlen = LUAL_BUFFERSIZE;  /* how much to read in each cycle */
        luaL_Buffer b;
        luaL_buffinit(L, &b);
        for (;;) {
            char *p = luaL_prepbuffsize(&b, rlen);
            size_t nr = f->Read(reinterpret_cast<GHL::Byte*>(p), static_cast<GHL::UInt32>(rlen));
            luaL_addsize(&b, nr);
            if (nr < rlen) break;  /* eof? */
            else if (rlen <= (MAX_SIZE_T / 4))  /* avoid buffers too large */
                rlen *= 2;  /* double buffer size at each iteration */
        }
        luaL_pushresult(&b);  /* close buffer */
    }
    
    
    static int read_chars (lua_State *L, GHL::DataStream *f, size_t n) {
        size_t nr;  /* number of chars actually read */
        char *p;
        luaL_Buffer b;
        luaL_buffinit(L, &b);
        p = luaL_prepbuffsize(&b, n);  /* prepare buffer to read whole block */
        nr = f->Read(reinterpret_cast<GHL::Byte*>(p), static_cast<GHL::UInt32>(n));  /* try to read 'n' chars */
        luaL_addsize(&b, nr);
        luaL_pushresult(&b);  /* close buffer */
        return (nr > 0);  /* true iff read something */
    }
    
    static bool get_string(GHL::DataStream *f,void* p,size_t max_size) {
        bool res = false;
        GHL::Byte* buf = static_cast<GHL::Byte*>(p);
        --max_size;
        while (!f->Eof() && max_size) {
            if (f->Read(buf, 1)!=1) {
                *buf = 0;
                return res;
            }
            res = true;
            if (*buf == '\n') {
                ++buf;
                break;
            }
            ++buf;
            --max_size;
            
        }
        *buf = 0;
        return res;
    }
    
    static int lua_io_read_line (lua_State *L, GHL::DataStream *f, int chop) {
        luaL_Buffer b;
        luaL_buffinit(L, &b);
        for (;;) {
            size_t l;
            char *p = luaL_prepbuffer(&b);
            if (!get_string(f, p, LUAL_BUFFERSIZE)) {  /* eof? */
                luaL_pushresult(&b);  /* close buffer */
                return (lua_rawlen(L, -1) > 0);  /* check whether read something */
            }
            l = strlen(p);
            if (l == 0 || p[l-1] != '\n')
                luaL_addsize(&b, l);
            else {
                luaL_addsize(&b, l - chop);  /* chop 'eol' if needed */
                luaL_pushresult(&b);  /* close buffer */
                return 1;  /* read at least an `eol' */
            }
        }
    }
    
    static int lua_io_g_read (lua_State *L, GHL::DataStream *f, int first) {
        int nargs = lua_gettop(L) - 1;
        int success;
        int n;
        
        if (nargs == 0) {  /* no arguments? */
            success = lua_io_read_line(L, f, 1);
            n = first+1;  /* to return 1 result */
        }
        else {  /* ensure stack space for all results and for auxlib's buffer */
            luaL_checkstack(L, nargs+LUA_MINSTACK, "too many arguments");
            success = 1;
            for (n = first; nargs-- && success; n++) {
                if (lua_type(L, n) == LUA_TNUMBER) {
                    size_t l = (size_t)lua_tointeger(L, n);
                    success = (l == 0) ? (f->Eof()) : read_chars(L, f, l);
                }
                else {
                    const char *p = lua_tostring(L, n);
                    luaL_argcheck(L, p && p[0] == '*', n, "invalid option");
                    switch (p[1]) {
                        case 'n':  /* number */
                            luaL_argerror(L, n, "unsupported read numbe");
                            break;
                        case 'l':  /* line */
                            success = lua_io_read_line(L, f, 1);
                            break;
                        case 'L':  /* line with end-of-line */
                            success = lua_io_read_line(L, f, 0);
                            break;
                        case 'a':  /* file */
                            read_all(L, f);  /* read entire file */
                            success = 1; /* always success */
                            break;
                        default:
                            return luaL_argerror(L, n, "invalid format");
                    }
                }
            }
        }
        if (!success) {
            lua_pop(L, 1);  /* remove last result */
            lua_pushnil(L);  /* push nil instead */
        }
        return n - first;
    }
    
    static GHL::DataStream *tofile (lua_State *L) {
        FileHandle* handle = ((FileHandle *)luaL_checkudata(L, 1, LUA_FILEHANDLE));
        if (!handle->ds)
            luaL_error(L, "attempt to use a closed file");
        return handle->ds;
    }
    
    
    static int lua_io_f_read (lua_State *L) {
        return lua_io_g_read(L, tofile(L), 2);
    }
    
    
    static int lua_io_readline (lua_State *L) {
        FileHandle* handle = ((FileHandle *)lua_touserdata(L, lua_upvalueindex(1)));
        int i;
        int n = (int)lua_tointeger(L, lua_upvalueindex(2));
        if (!handle->ds)  /* file is already closed? */
            return luaL_error(L, "file is already closed");
        lua_settop(L , 1);
        for (i = 1; i <= n; i++)  /* push arguments to 'g_read' */
            lua_pushvalue(L, lua_upvalueindex(3 + i));
        n = lua_io_g_read(L, handle->ds, 2);  /* 'n' is number of results */
        lua_assert(n > 0);  /* should return at least a nil */
        if (!lua_isnil(L, -n))  /* read at least one value? */
            return n;  /* return them */
        else {  /* first result is nil: EOF or error */
            if (n > 1) {  /* is there error information? */
                /* 2nd result is error message */
                return luaL_error(L, "%s", lua_tostring(L, -n + 1));
            }
            if (lua_toboolean(L, lua_upvalueindex(3))) {  /* generator created file? */
                lua_settop(L, 0);
                lua_pushvalue(L, lua_upvalueindex(1));
                lua_io_close(L);  /* close it */
            }
            return 0;
        }
    }
    
    
    /*
     ** methods for file handles
     */
    static const luaL_Reg flib[] = {
        {"close", lua_io_close},
        //{"flush", f_flush},
        {"lines", lua_io_f_lines},
        {"read", lua_io_f_read},
        //{"seek", f_seek},
        //{"setvbuf", f_setvbuf},
        //{"write", f_write},
        {"__gc", lua_io_close},
        //{"__tostring", f_tostring},
        {NULL, NULL}
    };
    
    
    static void createmeta (lua_State *L) {
        luaL_newmetatable(L, LUA_FILEHANDLE);  /* create metatable for file handles */
        lua_pushvalue(L, -1);  /* push metatable */
        lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
        luaL_setfuncs(L, flib, 0);  /* add file methods to new metatable */
        lua_pop(L, 1);  /* pop new metatable */
    }

    void lua_io_register( lua_State* L, FileProvider* provider ) {
        createmeta(L);
        lua_createtable(L, 0, 2);
        static const luaL_Reg io_funcs_impl[] = {
            {"open", lua_io_open},
            {"close",   lua_io_close},
            {"lines",   lua_io_lines},
            {NULL, NULL}
        };
        lua_pushlightuserdata(L, provider);
        luaL_setfuncs(L,  io_funcs_impl,1);
        lua_setglobal(L, "io");
    }
}
