//
//  sb_mygui_bind.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 24/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_mygui_bind.h"
#include <luabind/sb_luabind.h>
#include "../sb_lua.h"

#include "MyGUI_LayoutManager.h"
#include "MyGUI_Widget.h"
#include "MyGUI_LayerManager.h"
#include "MyGUI_ResourceManager.h"

SB_META_DECLARE_KLASS(MyGUI::Widget, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::Widget)
SB_META_END_KLASS_BIND()


SB_META_DECLARE_KLASS(MyGUI::LayoutManager, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::LayoutManager)
SB_META_METHOD(loadLayout)
SB_META_STATIC_METHOD(getInstancePtr)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(MyGUI::LayerManager, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::LayerManager)
SB_META_STATIC_METHOD(getInstancePtr)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(MyGUI::ResourceManager, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::ResourceManager)
SB_META_STATIC_METHOD(getInstancePtr)
SB_META_METHOD(load)
SB_META_END_KLASS_BIND()


namespace Sandbox {
    namespace mygui {
        
        void register_mygui( lua_State* lua ) {
            luabind::ExternClass<MyGUI::Widget>(lua);
            luabind::ExternClass<MyGUI::LayoutManager>(lua);
            luabind::ExternClass<MyGUI::LayerManager>(lua);
            luabind::ExternClass<MyGUI::ResourceManager>(lua);
        }
        
        void setup_singletons( LuaVM* lua ) {
            LuaContextPtr ctx = lua->GetGlobalContext();
            
            //ctx->SetValue("mygui.", this);
            //ctx->SetValue("settings", settings);

        }
    }
}

