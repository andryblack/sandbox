//
//  sb_luabind_ref.h
//  YinYang
//
//  Created by Андрей Куницын on 7/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_luabind_ref_h
#define YinYang_sb_luabind_ref_h

#include "sb_shared_ptr.h"

struct lua_State;

namespace Sandbox {
    namespace luabind {
        
        struct LuaVMHelper {
            lua_State* lua;
        };
        typedef sb::shared_ptr<LuaVMHelper> LuaVMHelperPtr;
        typedef sb::weak_ptr<LuaVMHelper> LuaVMHelperWeakPtr;
        class LuaReference {
        public:
            explicit LuaReference( const LuaVMHelperWeakPtr& ptr );
            ~LuaReference();
            void SetObject( lua_State* state );
            void UnsetObject( lua_State* state );
            void GetObject( lua_State* state ) const;
            LuaVMHelperPtr GetHelper() const { return m_lua.lock();}
            const LuaVMHelperWeakPtr& GetHelperPtr() const { return m_lua;}
            bool Valid() const;
        private:
            LuaVMHelperWeakPtr m_lua;
            int	m_ref;
        };
        
    }
}

#endif
