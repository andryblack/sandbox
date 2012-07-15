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
#include "sb_traits.h"
#include "impl/sb_luabind_wrapper_base.h"

namespace Sandbox {
    namespace luabind {
        
        
        class WrapperHolder {
        public:
            lua_State* GetVM() const {
                LuaVMHelperPtr helper = m_ref.GetHelper();
                if (!helper)
                    return 0;
                return helper->lua;
            }
            void push(lua_State* L) const {
                m_ref.GetObject(L);
            }
            WrapperHolder( ) : m_ref(LuaVMHelperWeakPtr()) {}
            void SetObject(lua_State* L) {
                m_ref.SetObject(L);
            }
        private:
            LuaReference    m_ref;
        };
        
        class wrapper : public impl::wrapper_base<WrapperHolder> {
        public:
            void SetObject(lua_State* L) {
                m_self.SetObject(L);
            }
        };
        
        template <class T> struct wrapper_helper {
            static wrapper* get_wrapper_func( lua_State* L, int idx ) {
                return stack<T*>::get(L,idx);
            }
        };
    }
}

#endif
