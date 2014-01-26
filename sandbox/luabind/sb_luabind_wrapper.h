//
//  sb_luabind_wrapper.h
//  YinYang
//
//  Created by Андрей Куницын on 7/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_luabind_wrapper_h
#define YinYang_sb_luabind_wrapper_h

#include "sb_luabind_stack.h"
#include "sb_luabind_ref.h"
#include <sbstd/sb_traits.h>
#include "impl/sb_luabind_wrapper_base.h"

namespace Sandbox {
    namespace luabind {
        
        class wrapper : public impl::wrapper_base {
        public:
            using impl::wrapper_base::call;
            using impl::wrapper_base::call_self;
            template <class T>
            T get(const char* name) {
                T res;
                lua_State* L = GetVM();
				GetObject(L);
				lua_pushstring(L,name);
				lua_gettable(L,-2);
                res = stack<T>::get(L,-1);
                lua_pop(L, 2);
                return res;
            }
            template <class T>
            T get(const char* name, T def) {
                T res = def;
                lua_State* L = GetVM();
				GetObject(L);
				lua_pushstring(L,name);
				lua_gettable(L,-2);
                res = stack<T>::get(L,-1);
                lua_pop(L, 2);
                return res;
            }
        private:
        };
        typedef sb::shared_ptr<wrapper> wrapper_ptr;
        
        
        template <>
        struct stack<wrapper_ptr> {
            static void push( lua_State* L, const wrapper_ptr& val ) {
                if (val && val->Valid()) {
                    val->GetObject(L);
                } else {
                    lua_pushnil(L);
                }
            }
            static wrapper_ptr get( lua_State* L, int idx ) {
                if (lua_istable(L,idx)) {
                    wrapper* w = new wrapper();
                    lua_pushvalue(L, idx);
                    w->SetObject(L);
                    return wrapper_ptr(w);
                }
                return wrapper_ptr();
            }
        };
        template <>
        struct stack<const wrapper_ptr&> : stack<wrapper_ptr> {};
    }
}

#endif
