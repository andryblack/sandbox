//
//  sb_rocket_bind.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/12/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_rocket_bind.h"
#include "sb_rocket_context.h"
#include "sb_rocket_document.h"
#include "sb_rocket_event_listener.h"

namespace Sandbox {
    namespace luabind {
        
        template <>
        struct stack<Rocket::Core::String>{
            static void push( lua_State* L, const Rocket::Core::String& val ) {
                lua_pushstring(L, val.CString());
            }
            static Rocket::Core::String get( lua_State* L, int idx ) {
                return Rocket::Core::String( lua_tostring(L, idx) );
            }
        };
        template <>
        struct stack<const Rocket::Core::String&>{
            static void push( lua_State* L, const Rocket::Core::String& val ) {
                stack<Rocket::Core::String>::push(L, val);
            }
            static Rocket::Core::String get( lua_State* L, int idx ) {
                return stack<Rocket::Core::String>::get(L, idx);
            }
        };
        template <>
        struct stack<Rocket::Core::Variant*>{
            static void push( lua_State* L, Rocket::Core::Variant* val ) {
                if (!val) {
                    lua_pushnil(L);
                } else {
                    switch (val->GetType()) {
                        case Rocket::Core::Variant::VOIDPTR:
                        case Rocket::Core::Variant::SCRIPTINTERFACE:
                        case Rocket::Core::Variant::NONE:
                            lua_pushnil(L);
                            break;
                        case Rocket::Core::Variant::STRING:
                            lua_pushstring(L, val->Get<Rocket::Core::String>().CString());
                            break;
                        case Rocket::Core::Variant::FLOAT:
                            lua_pushnumber(L, val->Get<float>());
                            break;
                        case Rocket::Core::Variant::INT:
                        case Rocket::Core::Variant::BYTE:
                            lua_pushinteger(L, val->Get<int>());
                            break;
                        case Rocket::Core::Variant::WORD:
                            lua_pushinteger(L, val->Get<unsigned int>());
                            break;
                            
                        case Rocket::Core::Variant::CHAR: {
                            char c = val->Get<char>();
                            lua_pushlstring(L, &c, 1);
                            }break;
                            
                        default:
                            lua_pushstring(L, val->Get<Rocket::Core::String>().CString());
                            break;
                    }
                }
            }
        };
    }
}



SB_META_DECLARE_KLASS(Rocket::Core::Dictionary, void)
SB_META_BEGIN_KLASS_BIND(Rocket::Core::Dictionary)
//bind( operator_( op_index , static_cast<Rocket::Core::Variant*(Rocket::Core::Dictionary::*)(const Rocket::Core::String& ) const>(&Rocket::Core::Dictionary::Get)) );
bind( method("Get", static_cast<Rocket::Core::Variant*(Rocket::Core::Dictionary::*)(const Rocket::Core::String& ) const>(&Rocket::Core::Dictionary::Get)) );
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Rocket::Core::Event, void)
SB_META_BEGIN_KLASS_BIND(Rocket::Core::Event)
SB_META_PROPERTY_RO(Type, GetType)
SB_META_PROPERTY_RO(Parameters, GetParameters)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Rocket::Core::Element, void)
SB_META_BEGIN_KLASS_BIND(Rocket::Core::Element)
SB_META_METHOD(Focus)
SB_META_METHOD(Blur)
SB_META_METHOD(Click)
SB_META_METHOD(GetElementById)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Rocket::Core::ElementDocument, Rocket::Core::Element)
SB_META_BEGIN_KLASS_BIND(Rocket::Core::ElementDocument)
SB_META_METHOD(Show)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::RocketDocument)
SB_META_PROPERTY_RO(LuaContext, GetLuaContext)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::RocketContext, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::RocketContext)
SB_META_METHOD(LoadRocketDocument)
SB_META_METHOD(RegisterFont)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    void RocketBind(lua_State* L) {
        luabind::ExternClass<Rocket::Core::Dictionary>(L);
        luabind::ExternClass<Rocket::Core::Event>(L);
        luabind::ExternClass<Rocket::Core::Element>(L);
        luabind::ExternClass<Rocket::Core::ElementDocument>(L);
        luabind::ExternClass<RocketDocument>(L);
        luabind::ExternClass<RocketContext>(L);
    }
}
