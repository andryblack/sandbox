//
//  sb_luabind_ref.h
//  YinYang
//
//  Created by Андрей Куницын on 7/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_luabind_ref_h
#define YinYang_sb_luabind_ref_h

#include <sbstd/sb_shared_ptr.h>
#include "sb_notcopyable.h"
#include "sb_luabind_stack.h"

struct lua_State;

namespace Sandbox {
    namespace luabind {
        
        struct LuaVMHelper {
            lua_State* lua;
        };
        typedef sb::shared_ptr<LuaVMHelper> LuaVMHelperPtr;
        typedef sb::weak_ptr<LuaVMHelper> LuaVMHelperWeakPtr;
        class LuaReference : public NotCopyable {
        public:
            LuaReference();
            explicit LuaReference( const LuaVMHelperWeakPtr& ptr );
            ~LuaReference();
            void SetObject( lua_State* state );
            void UnsetObject( lua_State* state );
            void GetObject( lua_State* state ) const;
            LuaVMHelperPtr GetHelper() const { return m_lua.lock();}
            const LuaVMHelperWeakPtr& GetHelperPtr() const { return m_lua;}
            bool Valid() const;
            void Reset();
        private:
            LuaVMHelperWeakPtr m_lua;
            int	m_ref;
        };
        
        template <>
        struct stack<LuaReference>{
            static void push( lua_State* L, const LuaReference& val ) {
                if (val.Valid()) {
                    val.GetObject(L);
                } else {
                    lua_pushnil(L);
                }
            }
        };
        template <>
        struct stack<sb::shared_ptr<LuaReference> >{
            static void push( lua_State* L, const sb::shared_ptr<LuaReference>& val ) {
                if (val) {
                    stack<LuaReference>::push(L, *val);
                } else {
                    lua_pushnil(L);
                }
            }
        };
                
    }
}

#endif
