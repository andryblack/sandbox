//
//  sb_luabind_stack.cpp
//  YinYang
//
//  Created by Андрей Куницын on 7/8/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_luabind_stack.h"

#include "sb_inplace_string.h"
#include "../sb_log.h"
#include <sbstd/sb_platform.h>

namespace Sandbox {
    namespace luabind {
        static const char* const LuabindModule = "Sanbox:luabind";
        
        int lua_traceback (lua_State *L)
        {
            // look up Lua's 'debug.traceback' function
            lua_getglobal(L, "debug");
            if (!lua_istable(L, -1))
            {
                lua_pop(L, 1);
                return 1;
            }
            lua_getfield(L, -1, "traceback");
            if (!lua_isfunction(L, -1))
            {
                lua_pop(L, 2);
                return 1;
            }
            int nargs = 2;
            if (lua_isstring(L,1))
                lua_pushvalue(L, 1);  /* pass error message */
            else {
                lua_pushvalue(L, 1);
                lua_pushstring(L, "error");
                nargs = 3;
            }
            lua_pushinteger(L, 1);  /* skip this function and traceback */
            lua_call(L, nargs, 1);  /* call debug.traceback */
            return 1;
        }
        
        void lua_argerror( lua_State* L, int idx, 
                          const char* expected, 
                          const char* got ) {
            char buf[128];
            if ( !got ) {
                got = lua_typename( L, lua_type(L,idx) );
            }
            sb::snprintf(buf, 128, "invalid argument %d : got '%s' , expected '%s'\n", idx,got,expected);
            sb::string str = buf;
            int top = lua_gettop(L);
            lua_getglobal(L, "tostring");
            
            for (int i=1;i<=top;i++) {
                str+=lua_typename(L, lua_type(L,i) );
                
                const char *s;
                lua_pushvalue(L, -1);  /* function to be called */
                lua_pushvalue(L, i);   /* value to print */
                lua_call(L, 1, 1);
                s = lua_tostring(L, -1);  /* get result */
                if (s != NULL) {
                    str+="(";
                    str+=s;
                    str+=")";
                }
                str+=",";
                lua_pop(L, 1);
            }
            lua_pop(L, 1); // func
            str+="\n";
            sb_assert(top==lua_gettop(L));
            lua_pushstring(L, str.c_str());
            lua_error(L);
        }
        
        void lua_access_error( lua_State* L, int idx, 
                              const char* got ) {
            char buf[128];
            sb::snprintf(buf, 128, "invalid argument access %d : '%s' is const", idx,got);
            lua_pushstring(L, buf);
            lua_error(L);
        }
        
                
        void lua_call_method(lua_State* L, int args, int ress, const char* name) {
            
            lua_pushcclosure(L, &luabind::lua_traceback, 0);
            lua_insert(L, -2-args);
            
            int res = lua_pcall(L, args, ress, -2-args);
            
            if (res) {
                LogError(LuabindModule) << " Failed script call method '" << name << "'" ;
                LogError(LuabindModule) << lua_tostring(L, -1) ;
            }
            lua_remove(L, -1-ress);
        }
        
        
                
                
        void stack<char>::push( lua_State* L, char val ) {
            lua_pushlstring(L, &val, 1);
        }
        
        bool stack<bool>::get( lua_State* L, int idx ) {
            return lua_toboolean(L, idx)!=0;
        }
        
        void stack<bool>::push( lua_State* L, bool val ) {
            lua_pushboolean(L, val ? 1:0);
        }
        
        char stack<char>::get( lua_State* L, int idx ) {
            if ( lua_isnumber( L, idx ) ) {
                return char( lua_tointeger(L, idx) );
            } else if ( lua_isstring(L, idx ) ) {
                return *lua_tostring(L, idx);
            }
            lua_argerror(L, idx, "char", 0);
            return 0;
        }
        
        void stack<long>::push( lua_State* L, long val) {
            lua_pushnumber(L, val);
        }
        
        long stack<long>::get( lua_State* L, int idx ) {
            return long(lua_tonumber(L, idx));
        }
        
        void stack<short>::push( lua_State* L, short val) {
            lua_pushnumber(L, val);
        }
        
        short stack<short>::get( lua_State* L, int idx ) {
            return short(lua_tonumber(L, idx));
        }
        
        void stack<int>::push( lua_State* L, int val) {
            lua_pushinteger(L, val);
        }
        
        int stack<int>::get( lua_State* L, int idx ) {
            return int(lua_tointeger(L, idx));
        }
        
        void stack<float>::push( lua_State* L, float val) {
            lua_pushnumber(L, val);
        }
        
        float stack<float>::get( lua_State* L, int idx ) {
            return float(lua_tonumber(L, idx));
        }
        
        void stack<double>::push( lua_State* L, double val) {
            lua_pushnumber(L, val);
        }
        
        double stack<double>::get( lua_State* L, int idx ) {
            return lua_tonumber(L, idx);
        }
        
        void stack<unsigned char>::push( lua_State* L, unsigned char val) {
            lua_pushunsigned(L, val);
        }
        
        unsigned char stack<unsigned char>::get( lua_State* L, int idx ) {
            return (unsigned char)lua_tounsigned(L, idx);
        }
        
        void stack<unsigned long>::push( lua_State* L, unsigned long val) {
            lua_pushnumber(L, val);
        }
        
        unsigned long stack<unsigned long>::get( lua_State* L, int idx ) {
            return (unsigned long)lua_tonumber(L, idx);
        }
        
        void stack<unsigned short>::push( lua_State* L, unsigned short val) {
            lua_pushunsigned(L, val);
        }
        
        unsigned short stack<unsigned short>::get( lua_State* L, int idx ) {
            return (unsigned short)lua_tounsigned(L, idx);
        }
        
        void stack<unsigned int>::push( lua_State* L, unsigned int val) {
            lua_pushunsigned(L, val);
        }
        
        unsigned int stack<unsigned int>::get( lua_State* L, int idx ) {
            return (unsigned int)lua_tounsigned(L, idx);
        }
        
        void stack<const char*>::push( lua_State* L, const char* val) {
            lua_pushstring(L, val);
        }
        
        const char* stack<const char*>::get( lua_State* L, int idx ) {
            return lua_tostring(L, idx);
        }

        
        
        
                
        
    }
}
