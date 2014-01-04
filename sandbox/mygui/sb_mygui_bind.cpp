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
#include "MyGUI_Align.h"
#include "MyGUI_WidgetManager.h"

#include "MyGUI_TextBox.h"
#include "MyGUI_Window.h"
#include "MyGUI_ILayerNode.h"
#include "MyGUI_LayerNode.h"
#include "MyGUI_ScrollBar.h"
#include "MyGUI_PopupMenu.h"
#include "MyGUI_MenuItem.h"

#include "MyGUI_IResource.h"
#include "MyGUI_IPointer.h"

#include "MyGUI_TileRect.h"
#include "MyGUI_MainSkin.h"
#include "MyGUI_ImageBox.h"
#include "MyGUI_EditText.h"
#include "MyGUI_MultiListBox.h"
#include "MyGUI_MultiListItem.h"


#include "MyGUI_ComboBox.h"
#include "MyGUI_TabItem.h"
#include "MyGUI_MenuBar.h"
#include "MyGUI_ItemBox.h"
#include "MyGUI_Canvas.h"
#include "MyGUI_ScrollView.h"
#include "MyGUI_SimpleText.h"
#include "MyGUI_ProgressBar.h"
#include "MyGUI_ResourceImageSetPointer.h"
#include "MyGUI_ResourceManualPointer.h"
#include "MyGUI_ResourceManualFont.h"
#include "MyGUI_ResourceTrueTypeFont.h"
#include "MyGUI_SharedLayer.h"
#include "MyGUI_OverlappedLayer.h"
#include "MyGUI_ResourceLayout.h"
#include "MyGUI_ResourceImageSet.h"
#include "MyGUI_ResourceSkin.h"
#include "MyGUI_RotatingSkin.h"
#include "MyGUI_PolygonalSkin.h"
#include "MyGUI_ControllerFadeAlpha.h"
#include "MyGUI_ControllerPosition.h"
#include "MyGUI_ControllerEdgeHide.h"

#include "MyGUI_CommonStateInfo.h"




SB_META_DECLARE_KLASS(MyGUI::Align, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::Align)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(MyGUI::IntPoint, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::IntPoint)
SB_META_CONSTRUCTOR((int,int))
SB_META_PROPERTY(left)
SB_META_PROPERTY(top)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(MyGUI::IntSize, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::IntSize)
SB_META_CONSTRUCTOR((int,int))
SB_META_PROPERTY(width)
SB_META_PROPERTY(height)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(MyGUI::IntCoord, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::IntCoord)
SB_META_PROPERTY(left)
SB_META_PROPERTY(top)
SB_META_PROPERTY(width)
SB_META_PROPERTY(height)
SB_META_PROPERTY_RO(right, right)
SB_META_PROPERTY_RO(bottom, bottom)
SB_META_PROPERTY_RO(point, point)
SB_META_PROPERTY_RO(size, size)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(MyGUI::IntRect, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::IntRect)
SB_META_PROPERTY(left)
SB_META_PROPERTY(top)
SB_META_PROPERTY(right)
SB_META_PROPERTY(bottom)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(MyGUI::ICroppedRectangle, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::ICroppedRectangle)

SB_META_PROPERTY_RW(position, getPosition, setPosition)
SB_META_PROPERTY_RW(size, getSize, setSize)
SB_META_PROPERTY_RW(coord, getCoord, setCoord)
SB_META_PROPERTY_RO(left, getLeft)
SB_META_PROPERTY_RO(right, getRight)
SB_META_PROPERTY_RO(top, getTop)
SB_META_PROPERTY_RO(bottom, getBottom)
SB_META_PROPERTY_RO(width, getWidth)
SB_META_PROPERTY_RO(height, getHeight)
SB_META_PROPERTY_RO(absolutePosition, getAbsolutePosition)
SB_META_PROPERTY_RO(absoluteRect, getAbsoluteRect)
SB_META_PROPERTY_RO(absoluteLeft, getAbsoluteLeft)
SB_META_PROPERTY_RO(absoluteTop, getAbsoluteTop)

SB_META_END_KLASS_BIND()

SB_META_DECLARE_OBJECT(MyGUI::IObject, void)

SB_META_DECLARE_OBJECT(MyGUI::Widget, MyGUI::ICroppedRectangle)
SB_META_BEGIN_KLASS_BIND(MyGUI::Widget)
SB_META_PROPERTY_RO(name, getName)
SB_META_PROPERTY_RW(align, getAlign,setAlign)
SB_META_PROPERTY_RW(visible,getVisible,setVisible)
SB_META_PROPERTY_RW(alpha, getAlpha, setAlpha)
SB_META_PROPERTY_RW(InheritsAlpha, getInheritsAlpha, setInheritsAlpha)
SB_META_PROPERTY_RW(enabled, getEnabled, setEnabled)
SB_META_PROPERTY_RO(parent, getParent)
SB_META_PROPERTY_RO(clientWidget, getClientWidget)


SB_META_METHOD(findWidget)
SB_META_METHOD(setProperty)
SB_META_METHOD(changeWidgetSkin)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_OBJECT(MyGUI::ISubWidget, MyGUI::ICroppedRectangle)
SB_META_DECLARE_OBJECT(MyGUI::ISubWidgetRect, MyGUI::ISubWidget)
SB_META_DECLARE_OBJECT(MyGUI::ISubWidgetText, MyGUI::ISubWidget)

SB_META_DECLARE_OBJECT(MyGUI::EditText, MyGUI::ISubWidgetText)


SB_META_DECLARE_OBJECT(MyGUI::ImageBox, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::TextBox, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::Button, MyGUI::TextBox)

SB_META_DECLARE_OBJECT(MyGUI::MenuItem, MyGUI::Button)

SB_META_DECLARE_OBJECT(MyGUI::Window, MyGUI::TextBox)

SB_META_DECLARE_OBJECT(MyGUI::ScrollBar, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::MenuControl, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::PopupMenu, MyGUI::MenuControl)

SB_META_DECLARE_OBJECT(MyGUI::EditBox, MyGUI::TextBox)

SB_META_DECLARE_OBJECT(MyGUI::ComboBox, MyGUI::EditBox)

SB_META_DECLARE_OBJECT(MyGUI::TabItem, MyGUI::TextBox)

SB_META_DECLARE_OBJECT(MyGUI::MenuBar, MyGUI::MenuControl)

SB_META_DECLARE_OBJECT(MyGUI::ListBox, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::DDContainer, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::ItemBox, MyGUI::DDContainer)

SB_META_DECLARE_OBJECT(MyGUI::Canvas, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::ScrollView, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::SimpleText, MyGUI::EditText)

SB_META_DECLARE_OBJECT(MyGUI::TabControl, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::ProgressBar, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::MultiListBox, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::MultiListItem, MyGUI::TextBox)


SB_META_DECLARE_OBJECT(MyGUI::ISerializable, MyGUI::IObject)

SB_META_DECLARE_OBJECT(MyGUI::IResource, MyGUI::ISerializable)
SB_META_DECLARE_OBJECT(MyGUI::IPointer, MyGUI::IResource)
SB_META_DECLARE_OBJECT(MyGUI::IFont, MyGUI::IResource)

SB_META_DECLARE_OBJECT(MyGUI::IStateInfo, MyGUI::ISerializable)

SB_META_DECLARE_OBJECT(MyGUI::ILayer, MyGUI::ISerializable)

SB_META_DECLARE_OBJECT(MyGUI::ILayerNode, MyGUI::IObject)

SB_META_DECLARE_OBJECT(MyGUI::LayerNode, MyGUI::ILayerNode)

SB_META_DECLARE_OBJECT(MyGUI::SharedLayer, MyGUI::ILayer)
SB_META_DECLARE_OBJECT(MyGUI::OverlappedLayer, MyGUI::ILayer)

SB_META_DECLARE_OBJECT(MyGUI::SharedLayerNode, MyGUI::ILayerNode)

SB_META_DECLARE_OBJECT(MyGUI::TileRect, MyGUI::ISubWidgetRect)


SB_META_DECLARE_OBJECT(MyGUI::SubSkin, MyGUI::ISubWidgetRect)
SB_META_DECLARE_OBJECT(MyGUI::MainSkin, MyGUI::SubSkin)

SB_META_DECLARE_OBJECT(MyGUI::ResourceImageSetPointer, MyGUI::IPointer)
SB_META_DECLARE_OBJECT(MyGUI::ResourceManualPointer, MyGUI::IPointer)

SB_META_DECLARE_OBJECT(MyGUI::ResourceManualFont, MyGUI::IFont)

SB_META_DECLARE_OBJECT(MyGUI::ResourceLayout, MyGUI::IResource)
SB_META_DECLARE_OBJECT(MyGUI::ResourceSkin, MyGUI::IResource)
SB_META_DECLARE_OBJECT(MyGUI::ResourceImageSet, MyGUI::IResource)

SB_META_DECLARE_OBJECT(MyGUI::RotatingSkin, MyGUI::ISubWidgetRect)
SB_META_DECLARE_OBJECT(MyGUI::PolygonalSkin, MyGUI::ISubWidgetRect)

SB_META_DECLARE_OBJECT(MyGUI::ControllerItem, MyGUI::IObject)
SB_META_DECLARE_OBJECT(MyGUI::ControllerPosition, MyGUI::ControllerItem)
SB_META_DECLARE_OBJECT(MyGUI::ControllerFadeAlpha, MyGUI::ControllerItem)
SB_META_DECLARE_OBJECT(MyGUI::ControllerEdgeHide, MyGUI::ControllerItem)

SB_META_DECLARE_OBJECT(MyGUI::SubSkinStateInfo, MyGUI::IStateInfo)
SB_META_DECLARE_OBJECT(MyGUI::EditTextStateInfo, MyGUI::IStateInfo)
SB_META_DECLARE_OBJECT(MyGUI::TileRectStateInfo, MyGUI::IStateInfo)


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

SB_META_DECLARE_KLASS(MyGUI::WidgetManager, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::WidgetManager)
SB_META_STATIC_METHOD(getInstancePtr)
SB_META_END_KLASS_BIND()


static int gui_find_widget_proxy(lua_State* L) {
    MyGUI::Gui* self = Sandbox::luabind::stack<MyGUI::Gui*>::get(L, 1);
    MyGUI::Widget* w = 0;
    if (lua_gettop(L)>2) {
        w = self->findWidgetT(Sandbox::luabind::stack<sb::string>::get(L,2),
                              Sandbox::luabind::stack<sb::string>::get(L,3), false);
    } else {
        w = self->findWidgetT(Sandbox::luabind::stack<sb::string>::get(L,2),false);
    }
    Sandbox::luabind::stack<MyGUI::Widget*>::push(L, w);
    return 1;
}

SB_META_DECLARE_KLASS(MyGUI::Gui, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::Gui)
SB_META_STATIC_METHOD(getInstancePtr)
bind( method( "findWidget" , &gui_find_widget_proxy ) );
SB_META_END_KLASS_BIND()

namespace Sandbox {
    namespace mygui {
        
        void register_mygui( lua_State* lua ) {
            luabind::RawClass<MyGUI::Align>(lua);
            luabind::RawClass<MyGUI::IntPoint>(lua);
            luabind::RawClass<MyGUI::IntSize>(lua);
            luabind::RawClass<MyGUI::IntCoord>(lua);
            luabind::RawClass<MyGUI::IntRect>(lua);
            
            luabind::ExternClass<MyGUI::ICroppedRectangle>(lua);
            luabind::ExternClass<MyGUI::Widget>(lua);
            luabind::ExternClass<MyGUI::LayoutManager>(lua);
            luabind::ExternClass<MyGUI::LayerManager>(lua);
            luabind::ExternClass<MyGUI::ResourceManager>(lua);
            luabind::ExternClass<MyGUI::WidgetManager>(lua);
            luabind::ExternClass<MyGUI::Gui>(lua);

            
        }
        
        void setup_singletons( LuaVM* lua ) {
            LuaContextPtr ctx = lua->GetGlobalContext();
            
            //ctx->SetValue("mygui.", this);
            //ctx->SetValue("settings", settings);

        }
    }
}

