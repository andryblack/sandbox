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


//SB_BIND_RAW_CONSTRUCTOR(Sandbox::Vector2f,(float,float))
//SB_BIND_BEGIN_METHODS
//SB_BIND_METHOD(Sandbox::Vector2f,length,float())
//SB_BIND_METHOD(Sandbox::Vector2f,unit,Sandbox::Vector2f())
//SB_BIND_METHOD(Sandbox::Vector2f,normal,Sandbox::Vector2f())
//SB_BIND_OPERATOR_ADD_(Sandbox::Vector2f,Sandbox::Vector2f(Sandbox::Vector2f),Sandbox::Vector2f(Sandbox::Vector2f::*)(const Sandbox::Vector2f&)const)
//SB_BIND_OPERATOR_SUB_(Sandbox::Vector2f,Sandbox::Vector2f(Sandbox::Vector2f),Sandbox::Vector2f(Sandbox::Vector2f::*)(const Sandbox::Vector2f&)const)
//SB_BIND_OPERATOR_MUL_(Sandbox::Vector2f,Sandbox::Vector2f(float),Sandbox::Vector2f(Sandbox::Vector2f::*)(float)const)
//SB_BIND_END_METHODS
//SB_BIND_BEGIN_PROPERTYS
//SB_BIND_PROPERTY_RAW(Sandbox::Vector2f,x,float)
//SB_BIND_PROPERTY_RAW(Sandbox::Vector2f,y,float)
//SB_BIND_END_PROPERTYS
//SB_BIND_END_CLASS