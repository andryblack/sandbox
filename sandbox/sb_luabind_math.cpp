//
//  sb_luabind_math.cpp
//  YinYang
//
//  Created by Андрей Куницын on 5/13/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "luabind/sb_luabind.h"
#include "meta/sb_meta.h"
#include "sb_vector2.h"
#include "sb_vector3.h"
#include "sb_matrix4.h"


SB_META_DECLARE_KLASS(Sandbox::Vector2f, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Vector2f)
SB_META_CONSTRUCTOR((float,float))
SB_META_PROPERTY(x)
SB_META_PROPERTY(y)
SB_META_METHOD(length)
SB_META_METHOD(unit)
SB_META_METHOD(normal)
SB_META_OPERATOR_ADD_(Sandbox::Vector2f(Sandbox::Vector2f::*)(const Sandbox::Vector2f&)const)
SB_META_OPERATOR_SUB_(Sandbox::Vector2f(Sandbox::Vector2f::*)(const Sandbox::Vector2f&)const)
SB_META_OPERATOR_MUL_(Sandbox::Vector2f(Sandbox::Vector2f::*)(float)const)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Vector3f, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Vector3f)
SB_META_CONSTRUCTOR((float,float,float))
SB_META_PROPERTY(x)
SB_META_PROPERTY(y)
SB_META_PROPERTY(z)
SB_META_METHOD(length)
SB_META_OPERATOR_ADD_(Sandbox::Vector3f(Sandbox::Vector3f::*)(const Sandbox::Vector3f&)const)
SB_META_OPERATOR_SUB_(Sandbox::Vector3f(Sandbox::Vector3f::*)(const Sandbox::Vector3f&)const)
SB_META_OPERATOR_MUL_(Sandbox::Vector3f(Sandbox::Vector3f::*)(float)const)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    static int constructor_Matrix4f(lua_State* L) {
        if (!lua_istable(L, 2)) {
            luabind::lua_argerror(L, 2, "table", 0);
            return 0;
        }
        Matrix4f m = Matrix4f::identity();
        {
            LUA_CHECK_STACK(0)
            size_t indx = 0;
            lua_pushnil(L);
            while (lua_next(L, 2) != 0) {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
                if (indx<16) {
                    m.matrix[indx] = float(lua_tonumber(L, -1));
                    //m.matrix[(indx%4)*4+(indx/4)] = lua_tonumber(L, -1);
                }
                ++indx;
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(L, 1);
            }
        }
        luabind::stack<Matrix4f>::push(L, m);
        return 1;
    }
}


SB_META_DECLARE_KLASS(Sandbox::Matrix4f, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Matrix4f)
bind( constructor(&Sandbox::constructor_Matrix4f) );
SB_META_OPERATOR_MUL_(Sandbox::Matrix4f(Sandbox::Matrix4f::*)(const Sandbox::Matrix4f&)const)
SB_META_METHOD(inverted)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    void register_math( lua_State* lua ) {
        luabind::RawClass<Vector2f>(lua);
        luabind::RawClass<Vector3f>(lua);
        luabind::RawClass<Matrix4f>(lua);
    }
    
}

