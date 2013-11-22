//
//  sb_luabind_ref.cpp
//  YinYang
//
//  Created by Андрей Куницын on 7/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_luabind_ref.h"
#include "sb_luabind_stack.h"

namespace Sandbox {
    namespace luabind {
        
        LuaReference::LuaReference() : m_ref(LUA_NOREF) {
            
        }
        LuaReference::LuaReference( const LuaVMHelperWeakPtr& ptr ) : m_lua(ptr),m_ref(LUA_NOREF) {
        }
        LuaReference::~LuaReference() {
            Reset();
        }
        bool LuaReference::Valid() const {
            return m_ref!=LUA_NOREF;
        }
        LuaVMHelperPtr GetHelper( lua_State* L );
        
        void LuaReference::SetObject( lua_State* L ) {
            if (Valid()) {
                UnsetObject(L);
            }
            LUA_CHECK_STACK(-1)
            m_lua = luabind::GetHelper( L );
            sb_assert(m_ref==LUA_NOREF);
            m_ref = luaL_ref(L,LUA_REGISTRYINDEX);
            sb_assert(m_ref!=LUA_NOREF);
        }
        void LuaReference::UnsetObject( lua_State* L ) {
            LUA_CHECK_STACK(0)
            sb_assert(m_ref!=LUA_NOREF);
            luaL_unref(L,LUA_REGISTRYINDEX,m_ref);
            m_ref = LUA_NOREF;
        }
        void LuaReference::GetObject( lua_State* state ) const {
            sb_assert(m_ref!=LUA_NOREF);
            lua_rawgeti(state, LUA_REGISTRYINDEX, m_ref);
        }
        void LuaReference::Reset() {
            if (Valid()) {
                if (LuaVMHelperPtr lua = m_lua.lock()) {
                    UnsetObject(lua->lua);
                }
            }
        }
    }
}