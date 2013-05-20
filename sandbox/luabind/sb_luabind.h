//
//  sb_luabind.h
//  YinYang
//
//  Created by Андрей Куницын on 5/13/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_luabind_h
#define YinYang_sb_luabind_h

#include <sbtl/sb_shared_ptr.h>
#include "sb_notcopyable.h"
#include <sbtl/sb_string.h>
#include "sb_inplace_string.h"
#include "sb_notcopyable.h"
#include "sb_assert.h"

#include "sb_luabind_stack.h"
#include "impl/sb_luabind_registrators.h"
#include "sb_luabind_ref.h"
#include "sb_luabind_wrapper.h"

namespace Sandbox {
    
    namespace luabind {
        
        
        
        LuaVMHelperPtr GetHelper( lua_State* L );
        
        
        void Initialize( lua_State* L );
        void Deinitialize( lua_State* L );
        
        template <class T>
        static inline void RawClass( lua_State* L ) {
            LUA_CHECK_STACK(0)
            impl::raw_klass_registrator<T> kr(L);
            lua_create_metatable(L);
            meta::bind_type<T>::bind( kr );
            lua_register_metatable(L,meta::type<T>::info());
        }
        template <class T>
        static inline void ExternClass( lua_State* L ) {
            LUA_CHECK_STACK(0)
            impl::klass_registrator<T> kr(L);
            lua_create_metatable(L);
            meta::bind_type<T>::bind( kr );
            lua_register_metatable(L,meta::type<T>::info());
        }
        template <class T>
        static inline void Class( lua_State* L ) {
            LUA_CHECK_STACK(0)
            impl::shared_klass_registrator<T> kr(L);
            lua_create_metatable(L);
            meta::bind_type<T>::bind( kr );
            lua_register_metatable(L,meta::type<T>::info());
        }
        template <class T,class W>
        static inline void ClassWrapper( lua_State* L ) {
            LUA_CHECK_STACK(0)
            Class<T>(L);
            impl::shared_klass_registrator<W> kr(L);
            lua_create_metatable(L);
            {
                LUA_CHECK_STACK(0)
                meta::bind_type<W>::bind( kr );
            }
            lua_register_wrapper(L,meta::type<W>::info());
        }
        template <class T>
        static inline void Enum( lua_State* L ) {
            LUA_CHECK_STACK(0)
            impl::enum_registrator<T> kr(L);
            lua_create_metatable(L);
            lua_register_enum_metatable(L,meta::type<T>::info(),&impl::enum_registrator<T>::compare_func);
            lua_get_create_table(L, meta::type<T>::info()->name,3);
            meta::bind_type<T>::bind( kr );
            lua_pop(L, 1);
        }
        
        
        template <class T>
        static inline void SetValue( lua_State* L, const char* path, const sb::shared_ptr<T>& t ) {
            stack<sb::shared_ptr<T> >::push(L,t);
            lua_set_value( L, path );
        }
        template <class T>
        static inline void SetValue( lua_State* L, const char* path, typename sb::type_traits<T>::parameter_type t ) {
            stack<T>::push(L,t);
            lua_set_value( L, path );
        }
        template <class T>
        static inline void SetValue( lua_State* L, const char* path, T t ) {
            stack<T>::push(L,t);
            lua_set_value( L, path );
        }
        static inline void SetValue( lua_State* L, const char* path, const char* t ) {
            stack<const char*>::push(L,t);
            lua_set_value( L, path );
        }
        template <class T>
        static inline void SetValue( lua_State* L, const char* path, T* t ) {
            stack<T*>::push(L,t);
            lua_set_value( L, path );
        }
    }
    
}


#endif
