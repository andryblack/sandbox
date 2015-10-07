//
//  sb_mygui_widgets.cpp
//  Sandbox
//
//  Created by Andrey Kunitsyn on 30/01/14.
//
//

#include "sb_mygui_widgets.h"

#include "sb_mygui_scroll_list.h"
#include "sb_mygui_cached_widget.h"
#include "sb_mygui_scene_widget.h"
#include "sb_mygui_mask_image.h"
#include "sb_mygui_scroll_area.h"
#include "sb_mygui_text_widget.h"

#include "MyGUI_WidgetManager.h"
#include "MyGUI_FactoryManager.h"

#include "luabind/sb_luabind.h"

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::CachedWidget)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::SceneWidget)
SB_META_PROPERTY_RW(scene, getScene, setScene)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::MaskImageWidget)
SB_META_PROPERTY_RW(image,getImage,setImage)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::TextWidget)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    namespace mygui {
        
        void register_widgets() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            
            factory.registerFactory<ScrollList>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<ScrollArea>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<CachedWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<SceneWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<MaskImageWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<TextWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
        }
        void unregister_widgets() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            
            factory.unregisterFactory<SceneWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<ScrollList>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<ScrollArea>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<CachedWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<MaskImageWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<TextWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
        }

        
        void register_ScrollList(lua_State* L);
        void register_ScrollArea(lua_State* L);
        
        void register_widgets(lua_State* L) {
            luabind::ExternClass<CachedWidget>(L);
            luabind::ExternClass<SceneWidget>(L);
            
            register_ScrollList(L);
            register_ScrollArea(L);
            
            
            luabind::ExternClass<MaskImageWidget>(L);
            luabind::ExternClass<TextWidget>(L);
        }
    }
    
}
