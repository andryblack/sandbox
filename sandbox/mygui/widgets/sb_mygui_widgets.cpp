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
#include "sb_mygui_image_widget.h"
#include "sb_mygui_mask_image.h"
#include "sb_mygui_mask_text.h"
#include "sb_mygui_scroll_area.h"
#include "sb_mygui_text_widget.h"
#include "sb_mygui_scene_object.h"
#include "sb_mygui_text_edit.h"

#include "MyGUI_WidgetManager.h"
#include "MyGUI_FactoryManager.h"

#include "luabind/sb_luabind.h"

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::CachedWidget)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::SceneWidget)
SB_META_PROPERTY_RW(scene, getScene, setScene)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ImageWidgetBase)
SB_META_PROPERTY_RW(image,getImage,setImage)
SB_META_PROPERTY_WO(texture, setTexture)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ImageWidget)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::MaskImageWidget)
SB_META_PROPERTY_RW(shader, getShader, setShader)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::MaskTextWidget)
SB_META_PROPERTY_RW(shader, getShader, setShader)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::SceneObjectWidget)
SB_META_PROPERTY_RW(object,getObject,setObject)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::TextWidget)
SB_META_PROPERTY_RW(wordWrap, getWordWrap, setWordWrap)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::TextEdit)
SB_META_PROPERTY_RW(placeholder, getPlaceholder, setPlaceholder)
SB_META_END_KLASS_BIND()


namespace Sandbox {
    
    namespace mygui {
        
        void register_widgets() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            
            factory.registerFactory<ScrollList>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<ScrollArea>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<CachedWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<SceneWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<ImageWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<MaskImageWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<MaskTextWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<TextWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<SceneObjectWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<TextEdit>(MyGUI::WidgetManager::getInstance().getCategoryName());
        }
        void unregister_widgets() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            
            factory.unregisterFactory<TextEdit>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<SceneWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<ScrollList>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<ScrollArea>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<CachedWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<MaskImageWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<ImageWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<MaskTextWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<TextWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<SceneObjectWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
        }

        
        void register_ScrollList(lua_State* L);
        
        void register_widgets(lua_State* L) {
            luabind::ExternClass<CachedWidget>(L);
            luabind::ExternClass<SceneWidget>(L);
            
            register_ScrollList(L);
            
            luabind::ExternClass<ImageWidgetBase>(L);
            luabind::ExternClass<ImageWidget>(L);
            luabind::ExternClass<MaskImageWidget>(L);
            luabind::ExternClass<TextWidget>(L);
            luabind::ExternClass<MaskTextWidget>(L);
            
            luabind::ExternClass<SceneObjectWidget>(L);
            luabind::ExternClass<TextEdit>(L);
        }
    }
    
}
