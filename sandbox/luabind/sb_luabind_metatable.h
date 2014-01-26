//
//  sb_luabind_metatable.h
//  YinYang
//
//  Created by Андрей Куницын on 7/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_luabind_metatable_h
#define YinYang_sb_luabind_metatable_h

#include "meta/sb_meta.h"

struct lua_State;

namespace Sandbox {
    namespace luabind {
        
        struct data_holder;
        void lua_get_create_table(lua_State* L,const char* name,int prealloc);
        void lua_set_metatable( lua_State* L, const data_holder& holder );
        void lua_create_metatable(lua_State* L);
        void lua_register_metatable(lua_State* L,const meta::type_info* info);
        void lua_register_enum_metatable(lua_State* L,const meta::type_info* info,int(*compare)(lua_State*));
        
        void lua_register_wrapper(lua_State* L,const meta::type_info* info);
        
        void lua_get_create_child_table( lua_State* L, const char*& path);
        void lua_set_value( lua_State* L, const char* path );
    }
}

#endif
