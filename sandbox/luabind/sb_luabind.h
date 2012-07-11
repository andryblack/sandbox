//
//  sb_luabind.h
//  YinYang
//
//  Created by Андрей Куницын on 5/13/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_luabind_h
#define YinYang_sb_luabind_h

#include "sb_shared_ptr.h"
#include "sb_notcopyable.h"
#include "sb_string.h"
#include "sb_inplace_string.h"
#include "sb_notcopyable.h"
#include "sb_assert.h"

#include "sb_luabind_stack.h"
#include "impl/sb_luabind_registrators.h"


namespace Sandbox {
    
    namespace luabind {
        
        
        template <class T>
        inline void RawClass( lua_State* L ) {
            stack_cleaner clean_stack(L);
            impl::raw_klass_registrator<T> kr(L);
            lua_create_metatable(L,meta::type<T>::info());
            meta::bind_type<T>::bind( kr );
        }
        template <class T>
        inline void ExternClass( lua_State* L ) {
            stack_cleaner clean_stack(L);
            impl::klass_registrator<T> kr(L);
            lua_create_metatable(L,meta::type<T>::info());
            meta::bind_type<T>::bind( kr );
        }
        template <class T>
        inline void Class( lua_State* L ) {
            stack_cleaner clean_stack(L);
            impl::shared_klass_registrator<T> kr(L);
            lua_create_metatable(L,meta::type<T>::info());
            meta::bind_type<T>::bind( kr );
        }
        template <class T>
        inline void Enum( lua_State* L ) {
            stack_cleaner clean_stack(L);
            impl::enum_registrator<T> kr(L);
            lua_create_metatable(L,meta::type<T>::info());
            meta::bind_type<T>::bind( kr );
        }
        
        
        template <class T>
        inline void SetValue( lua_State* L, const char* path, const sb::shared_ptr<T>& t ) {
            stack<sb::shared_ptr<T> >::push(L,t);
            lua_set_value( L, path );
        }
        template <class T>
        inline void SetValue( lua_State* L, const char* path, typename sb::type_traits<T>::parameter_type t ) {
            stack<T>::push(L,t);
            lua_set_value( L, path );
        }
        inline void SetValue( lua_State* L, const char* path, const char* t ) {
            stack<const char*>::push(L,t);
            lua_set_value( L, path );
        }
        template <class T>
        inline void SetValue( lua_State* L, const char* path, T* t ) {
            stack<T*>::push(L,t);
            lua_set_value( L, path );
        }
    }
    
}


#endif
