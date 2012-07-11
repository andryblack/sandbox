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
    
    void register_math( lua_State* lua ) {
        luabind::RawClass<Vector2f>(lua);
        luabind::RawClass<Vector3f>(lua);
    }
    
}

