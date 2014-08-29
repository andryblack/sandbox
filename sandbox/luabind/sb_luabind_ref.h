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
#include <sbstd/sb_pointer.h>

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
        class LuaReference : public sb::ref_countered_base_not_copyable {
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
            lua_State* GetVM() const {
                LuaVMHelperPtr helper = GetHelper();
                if (!helper)
                    return 0;
                return helper->lua;
            }
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
        struct stack<sb::intrusive_ptr<LuaReference> >{
            static void push( lua_State* L, const sb::intrusive_ptr<LuaReference>& val ) {
                if (val) {
                    stack<LuaReference>::push(L, *val);
                } else {
                    lua_pushnil(L);
                }
            }
            static sb::intrusive_ptr<LuaReference> get(lua_State* L, int idx) {
                if (lua_isnil(L, idx)) {
                    return sb::intrusive_ptr<LuaReference>();
                }
                sb::intrusive_ptr<LuaReference> res(new LuaReference());
                lua_pushvalue(L, idx);
                res->SetObject(L);
                return res;
            }
        };
                
    }
}

#endif
