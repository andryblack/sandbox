//
//  sb_lua_context.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/13/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_lua_context__
#define __sr_osx__sb_lua_context__

#include "luabind/sb_luabind_ref.h"
#include "luabind/sb_luabind_stack.h"
#include "sb_notcopyable.h"
#include <sbstd/sb_traits.h>
#include <sbstd/sb_shared_ptr.h>

namespace Sandbox {
    
    class LuaContext : public luabind::LuaReference {
    private:
        lua_State* get_state_with_table_on_top(const char* path);
        void set_value_on_top_of_stack_to_table(lua_State* L);
    public:
        explicit LuaContext();
        
        sb::shared_ptr<LuaContext> CreateInherited();
        
        template <class T>
        inline void SetValue( const char* path, const sb::shared_ptr<T>& t ) {
            lua_State* L = get_state_with_table_on_top(path);
            if (!L) return;
            luabind::stack<sb::shared_ptr<T> >::push(L,t);
            set_value_on_top_of_stack_to_table(L);
        }
        template <class T>
        inline void SetValue( const char* path, typename sb::type_traits<T>::parameter_type t ) {
            lua_State* L = get_state_with_table_on_top(path);
            if (!L) return;
            luabind::stack<T>::push(L,t);
            set_value_on_top_of_stack_to_table(L);
        }
        template <class T>
        inline void SetValue( const char* path, T t ) {
            lua_State* L = get_state_with_table_on_top(path);
            if (!L) return;
            luabind::stack<T>::push(L,t);
            set_value_on_top_of_stack_to_table(L);
        }
        inline void SetValue( const char* path, const char* t ) {
            lua_State* L = get_state_with_table_on_top(path);
            if (!L) return;
            luabind::stack<const char*>::push(L,t);
            set_value_on_top_of_stack_to_table(L);
        }
        template <class T>
        inline void SetValue( const char* path, T* t ) {
            lua_State* L = get_state_with_table_on_top(path);
            if (!L) return;
            luabind::stack<T*>::push(L,t);
            set_value_on_top_of_stack_to_table(L);
        }

    };
    typedef sb::shared_ptr<LuaContext> LuaContextPtr;
}

#endif /* defined(__sr_osx__sb_lua_context__) */
