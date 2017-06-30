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
#include "sb_mygui_selectable_widget.h"
#include "sb_mygui_state_visible_widget.h"
#include "sb_mygui_client_widget.h"
#include "sb_mygui_animated_widget.h"
#include "sb_mygui_background_widget.h"
#include "sb_mygui_text_input.h"
#include "sb_mygui_shader_widget.h"

#include "MyGUI_WidgetManager.h"
#include "MyGUI_FactoryManager.h"

#include "luabind/sb_luabind.h"
#include "../sb_mygui_bind_helpers.h"

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::CachedWidget)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ClientWidget)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::SceneWidget)
SB_META_PROPERTY_RW(scene, getScene, setScene)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ImageWidgetBase)
SB_META_PROPERTY_RW(image,getImage,setImage)
SB_META_PROPERTY_WO(texture, setTexture)
SB_META_METHOD(setImage)
SB_META_METHOD(setTexture)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ImageWidget)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::MaskImageWidget)
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

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::SelectableWidget)
SB_META_PROPERTY_RW(selected, getSelected, setSelected)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::StateVisibleWidget)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::AnimatedWidget)
SB_META_PROPERTY_RO(Thread, GetThread)
SB_META_PROPERTY_RO(Transform, GetTransform)
SB_META_PROPERTY_RO(Color, GetColor)
SB_META_PROPERTY_RW(Origin,GetOrigin,SetOrigin)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::BackgroundWidget)
SB_META_PROPERTY_WO(background, setBackground)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ShaderWidget)
SB_META_PROPERTY_RW(shader,getShader,setShader)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::TextInput)
SB_META_PROPERTY_RW(Placeholder, getPlaceholder, setPlaceholder)
bind(method("eventEditAccept", delegate_bind<Sandbox::mygui::TextInput,
            Sandbox::mygui::TextInput,
            Sandbox::mygui::EventHandle_TextInputPtr,
            &Sandbox::mygui::TextInput::eventEditAccept>::lua_func));
bind(method("eventEditTextChange", delegate_bind<Sandbox::mygui::TextInput,
            Sandbox::mygui::TextInput,
            Sandbox::mygui::EventHandle_TextInputPtr,
            &Sandbox::mygui::TextInput::eventEditTextChange>::lua_func));

SB_META_END_KLASS_BIND()


SB_META_DECLARE_OBJECT(Sandbox::mygui::ScrollArea, MyGUI::ScrollView)
SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ScrollArea)
SB_META_PROPERTY_RW(manualScroll,manualScroll,setManualScroll)
SB_META_PROPERTY_WO(scrollPos, setScrollPos)
SB_META_PROPERTY_WO(scrollPosF, setScrollPosF)
SB_META_PROPERTY_RO(scrollActive, scrollActive)
SB_META_METHOD(scrollToWidget)
SB_META_METHOD(isWidgetFullVisible)
bind(method("scrollBegin", delegate_bind<Sandbox::mygui::ScrollArea,
            Sandbox::mygui::ScrollArea,
            Sandbox::mygui::EventHandle_ScrollAreaPtrIntPoint,
            &Sandbox::mygui::ScrollArea::scrollBegin>::lua_func));
bind(method("scrollComplete", delegate_bind<Sandbox::mygui::ScrollArea,
            Sandbox::mygui::ScrollArea,
            Sandbox::mygui::EventHandle_ScrollAreaPtrIntPoint,
            &Sandbox::mygui::ScrollArea::scrollComplete>::lua_func));
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    namespace mygui {
        
        void register_widgets() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            const std::string& category = MyGUI::WidgetManager::getInstance().getCategoryName();
            factory.registerFactory<ScrollList>(category);
            factory.registerFactory<ScrollArea>(category);
            factory.registerFactory<CachedWidget>(category);
            factory.registerFactory<SceneWidget>(category);
            factory.registerFactory<ImageWidget>(category);
            factory.registerFactory<MaskImageWidget>(category);
            factory.registerFactory<MaskTextWidget>(category);
            factory.registerFactory<TextWidget>(category);
            factory.registerFactory<SceneObjectWidget>(category);
            factory.registerFactory<TextEdit>(category);
            factory.registerFactory<SelectableWidget>(category);
            factory.registerFactory<StateVisibleWidget>(category);
            factory.registerFactory<ClientWidget>(category);
            factory.registerFactory<AnimatedWidget>(category);
            factory.registerFactory<BackgroundWidget>(category);
            factory.registerFactory<TextInput>(category);
            factory.registerFactory<ShaderWidget>(category);
        }
        void unregister_widgets() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            const std::string& category = MyGUI::WidgetManager::getInstance().getCategoryName();
            
            factory.unregisterFactory<TextEdit>(category);
            factory.unregisterFactory<SceneWidget>(category);
            factory.unregisterFactory<ScrollList>(category);
            factory.unregisterFactory<ScrollArea>(category);
            factory.unregisterFactory<CachedWidget>(category);
            factory.unregisterFactory<MaskImageWidget>(category);
            factory.unregisterFactory<ImageWidget>(category);
            factory.unregisterFactory<MaskTextWidget>(category);
            factory.unregisterFactory<TextWidget>(category);
            factory.unregisterFactory<SceneObjectWidget>(category);
            factory.unregisterFactory<SelectableWidget>(category);
            factory.unregisterFactory<StateVisibleWidget>(category);
            factory.unregisterFactory<ClientWidget>(category);
            factory.unregisterFactory<AnimatedWidget>(category);
            factory.unregisterFactory<BackgroundWidget>(category);
            factory.unregisterFactory<TextInput>(category);
            factory.unregisterFactory<ShaderWidget>(category);
        }

        
        void register_ScrollList(lua_State* L);
        
        void register_widgets(lua_State* L) {
            luabind::ExternClass<CachedWidget>(L);
            luabind::ExternClass<SceneWidget>(L);
            
            
            
            luabind::ExternClass<AnimatedWidget>(L);
            luabind::ExternClass<ShaderWidget>(L);
            luabind::ExternClass<ImageWidgetBase>(L);
            luabind::ExternClass<ImageWidget>(L);
            luabind::ExternClass<MaskImageWidget>(L);
            luabind::ExternClass<TextWidget>(L);
            luabind::ExternClass<MaskTextWidget>(L);
            
            luabind::ExternClass<SceneObjectWidget>(L);
            luabind::ExternClass<TextEdit>(L);
            luabind::ExternClass<SelectableWidget>(L);
            luabind::ExternClass<StateVisibleWidget>(L);
            luabind::ExternClass<ClientWidget>(L);
            luabind::ExternClass<BackgroundWidget>(L);
            luabind::ExternClass<TextInput>(L);
            
            luabind::ExternClass<ScrollArea>(L);
            register_ScrollList(L);
            
            
            
        }
    }
    
}
