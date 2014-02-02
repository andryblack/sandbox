//
//  sb_mygui_bind.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 24/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_mygui_bind.h"
#include <luabind/sb_luabind.h>
#include <luabind/sb_luabind_function.h>
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
#include "MyGUI_Window.h"

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

#include "widgets/sb_mygui_scene_widget.h"

namespace Sandbox {
    
    namespace luabind {
        
        template <>
        struct stack<MyGUI::UString> {
            static void push( lua_State* L, const MyGUI::UString& val ) {
                stack<const char*>::push(L, val.asUTF8_c_str());
            }
            static MyGUI::UString get( lua_State* L, int idx ) {
                return MyGUI::UString(stack<const char*>::get(L, idx));
            }
        };
        
        template <>
        struct stack<const MyGUI::UString&> : stack<MyGUI::UString> {};
        
        template <>
        struct stack<MyGUI::Any> {
            static void push( lua_State* L, const MyGUI::Any& val ) {
                if (val.getType()==typeid(int))
                    stack<int>::push(L, *val.castType<int>());
                else if (val.getType()==typeid(float))
                    stack<float>::push(L, *val.castType<float>());
                else if (val.getType()==typeid(double))
                    stack<float>::push(L, *val.castType<double>());
                else if (val.getType()==typeid(std::string))
                    stack<std::string>::push(L, *val.castType<std::string>());
                else if (val.getType()==typeid(bool))
                    stack<bool>::push(L, *val.castType<bool>());
                else
                    lua_pushnil(L);
            }
            static MyGUI::Any get( lua_State* L, int idx ) {
                int type = lua_type(L, idx);
                switch (type){
                    case LUA_TBOOLEAN:
                        return MyGUI::Any(stack<bool>::get(L, idx));
                    case LUA_TNUMBER:
                        return MyGUI::Any(stack<double>::get(L, idx));
                    case LUA_TSTRING:
                        return MyGUI::Any(stack<std::string>::get(L, idx));
                }
                return MyGUI::Any(MyGUI::Any::Null);
            }
        };
        
        template <>
        struct stack<const MyGUI::Any&> : stack<MyGUI::Any> {};
        
        
        template <>
        struct stack<MyGUI::MouseButton> {
            static void push( lua_State* L, const MyGUI::MouseButton& val ) {
                stack<int>::push(L, val.getValue());
            }
            static MyGUI::MouseButton get( lua_State* L, int idx ) {
                return MyGUI::MouseButton(static_cast<MyGUI::MouseButton::Enum>(stack<int>::get(L, idx)));
            }
        };
        
        template <>
        struct stack<const MyGUI::MouseButton&> : stack<MyGUI::MouseButton> {};

    }
}

template <typename A1,typename A2,typename A3,typename A4>
class LuaDelegate4 : public MyGUI::delegates::IDelegate4<A1,A2,A3,A4>, public MyGUI::delegates::IDelegateUnlink {
private:
    Sandbox::luabind::function  m_function;
public:
    virtual bool isType( const std::type_info& _type) { return false; }
    virtual void invoke( A1 a1, A2 a2, A3 a3, A4 a4 ) {
        m_function.call( a1, a2, a3, a4 );
    }
    virtual bool compare(  MyGUI::delegates::IDelegate4<A1,A2,A3,A4>* _delegate) const {
        return _delegate == this;
    }
    virtual bool compare( MyGUI::delegates::IDelegateUnlink* _unlink) const
    {
        return _unlink == this;
    }
    void SetObject(lua_State* L) {
        m_function.SetObject(L);
    }
};
template <typename A1,typename A2,typename A3>
class LuaDelegate3 : public MyGUI::delegates::IDelegate3<A1,A2,A3>, public MyGUI::delegates::IDelegateUnlink {
private:
    Sandbox::luabind::function  m_function;
public:
    virtual bool isType( const std::type_info& _type) { return false; }
    virtual void invoke( A1 a1, A2 a2, A3 a3) {
        m_function.call( a1, a2, a3 );
    }
    virtual bool compare(  MyGUI::delegates::IDelegate3<A1,A2,A3>* _delegate) const {
        return _delegate == this;
    }
    virtual bool compare( MyGUI::delegates::IDelegateUnlink* _unlink) const
    {
        return _unlink == this;
    }
    void SetObject(lua_State* L) {
        m_function.SetObject(L);
    }
};
template <typename A1,typename A2,typename A3>
class LuaDelegate3<A1,A2&,A3> : public MyGUI::delegates::IDelegate3<A1,A2&,A3>, public MyGUI::delegates::IDelegateUnlink {
private:
    Sandbox::luabind::function  m_function;
public:
    virtual bool isType( const std::type_info& _type) { return false; }
    virtual void invoke( A1 a1, A2& a2, A3 a3) {
        a2 = m_function.call<A2>( a1, a3 );
    }
    virtual bool compare(  MyGUI::delegates::IDelegate3<A1,A2&,A3>* _delegate) const {
        return _delegate == this;
    }
    virtual bool compare( MyGUI::delegates::IDelegateUnlink* _unlink) const
    {
        return _unlink == this;
    }
    void SetObject(lua_State* L) {
        m_function.SetObject(L);
    }
};
template <typename A1,typename A2>
class LuaDelegate2 : public MyGUI::delegates::IDelegate2<A1,A2>, public MyGUI::delegates::IDelegateUnlink {
private:
    Sandbox::luabind::function  m_function;
public:
    virtual bool isType( const std::type_info& _type) { return false; }
    virtual void invoke( A1 a1, A2 a2 ) {
        m_function.call( a1, a2 );
    }
    virtual bool compare(  MyGUI::delegates::IDelegate2<A1,A2>* _delegate) const {
        return _delegate == this;
    }
    virtual bool compare( MyGUI::delegates::IDelegateUnlink* _unlink) const
    {
        return _unlink == this;
    }
    void SetObject(lua_State* L) {
        m_function.SetObject(L);
    }
};
template <typename A1>
class LuaDelegate1 : public MyGUI::delegates::IDelegate1<A1>, public MyGUI::delegates::IDelegateUnlink {
private:
    Sandbox::luabind::function  m_function;
public:
    virtual bool isType( const std::type_info& _type) { return false; }
    virtual void invoke( A1 a1 ) {
        m_function.call( a1 );
    }
    virtual bool compare(  MyGUI::delegates::IDelegate1<A1>* _delegate) const {
        return _delegate == this;
    }
    virtual bool compare( MyGUI::delegates::IDelegateUnlink* _unlink) const
    {
        return _unlink == this;
    }
    void SetObject(lua_State* L) {
        m_function.SetObject(L);
    }
};


template <class O,class T,class U, U T::*>
struct delegate_bind;

template <class O,class T,class A1,class A2,class A3,class A4,MyGUI::delegates::CMultiDelegate4<A1, A2, A3, A4> T::*obj>
struct delegate_bind<O,T,MyGUI::delegates::CMultiDelegate4<A1, A2, A3, A4>, obj > {
    typedef LuaDelegate4<A1,A2,A3,A4> LuaDelegate;
    typedef MyGUI::delegates::CMultiDelegate4<A1, A2, A3, A4> MultiDelegate;
    static int lua_func( lua_State* L ) {
        O* t = Sandbox::luabind::stack<O*>::get(L,1);
        LuaDelegate* delegate = new LuaDelegate();
        lua_pushvalue(L, 2);
        delegate->SetObject(L);
        MultiDelegate& md(t->*obj);
        md += delegate;
        return 0;
    }
};
template <class O,class T,class A1,class A2,MyGUI::delegates::CMultiDelegate2<A1, A2> T::*obj>
struct delegate_bind<O,T,MyGUI::delegates::CMultiDelegate2<A1, A2>, obj > {
    typedef LuaDelegate2<A1,A2> LuaDelegate;
    typedef MyGUI::delegates::CMultiDelegate2<A1, A2> MultiDelegate;
    static int lua_func( lua_State* L ) {
        O* t = Sandbox::luabind::stack<O*>::get(L,1);
        LuaDelegate* delegate = new LuaDelegate();
        lua_pushvalue(L, 2);
        delegate->SetObject(L);
        MultiDelegate& md(t->*obj);
        md += delegate;
        return 0;
    }
};
template <class O,class T,class A1,MyGUI::delegates::CMultiDelegate1<A1> T::*obj>
struct delegate_bind<O,T,MyGUI::delegates::CMultiDelegate1<A1>, obj > {
    typedef LuaDelegate1<A1> LuaDelegate;
    typedef MyGUI::delegates::CMultiDelegate1<A1> MultiDelegate;
    static int lua_func( lua_State* L ) {
        O* t = Sandbox::luabind::stack<O*>::get(L,1);
        LuaDelegate* delegate = new LuaDelegate();
        lua_pushvalue(L, 2);
        delegate->SetObject(L);
        MultiDelegate& md(t->*obj);
        md += delegate;
        return 0;
    }
};
template <class O,class T,class A1,class A2,MyGUI::delegates::CDelegate2<A1, A2> T::*obj>
struct delegate_bind<O,T,MyGUI::delegates::CDelegate2<A1, A2>, obj > {
    typedef LuaDelegate2<A1,A2> LuaDelegate;
    typedef MyGUI::delegates::CDelegate2<A1, A2> MultiDelegate;
    static int lua_func( lua_State* L ) {
        O* t = Sandbox::luabind::stack<O*>::get(L,1);
        LuaDelegate* delegate = new LuaDelegate();
        lua_pushvalue(L, 2);
        delegate->SetObject(L);
        MultiDelegate& md(t->*obj);
        md = delegate;
        return 0;
    }
};
template <class O,class T,class A1,class A2,class A3,MyGUI::delegates::CDelegate3<A1, A2, A3> T::*obj>
struct delegate_bind<O,T,MyGUI::delegates::CDelegate3<A1, A2, A3>, obj > {
    typedef LuaDelegate3<A1,A2,A3> LuaDelegate;
    typedef MyGUI::delegates::CDelegate3<A1, A2, A3> MultiDelegate;
    static int lua_func( lua_State* L ) {
        O* t = Sandbox::luabind::stack<O*>::get(L,1);
        LuaDelegate* delegate = new LuaDelegate();
        lua_pushvalue(L, 2);
        delegate->SetObject(L);
        MultiDelegate& md(t->*obj);
        md = delegate;
        return 0;
    }
};




SB_META_DECLARE_KLASS(MyGUI::Align, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::Align)
SB_META_CONSTRUCTOR(())
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
SB_META_CONSTRUCTOR((int,int,int,int))
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

SB_META_DECLARE_KLASS(MyGUI::Colour, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::Colour)
SB_META_CONSTRUCTOR((float,float,float,float))
SB_META_PROPERTY(red)
SB_META_PROPERTY(green)
SB_META_PROPERTY(blue)
SB_META_PROPERTY(alpha)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(MyGUI::IBDrawItemInfo, void)
SB_META_BEGIN_KLASS_BIND(MyGUI::IBDrawItemInfo)
SB_META_PROPERTY(index)
SB_META_PROPERTY(update)
SB_META_PROPERTY(select)
SB_META_PROPERTY(active)
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


SB_META_DECLARE_KLASS(MyGUI::MouseButton,void);
SB_META_ENUM_BIND(MyGUI::MouseButton,namespace MyGUI,
                  SB_META_ENUM_MEMBER_ITEM(Left,MouseButton::Left)
                  SB_META_ENUM_MEMBER_ITEM(Right,MouseButton::Right)
                  SB_META_ENUM_MEMBER_ITEM(Middle,MouseButton::Middle))


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
SB_META_PROPERTY_WO(colour, setColour)
SB_META_PROPERTY_RW(alpha, getAlpha,setAlpha)

bind(method("eventMouseButtonClick", delegate_bind<MyGUI::Widget,
            MyGUI::WidgetInput,
            MyGUI::EventHandle_WidgetVoid,
            &MyGUI::WidgetInput::eventMouseButtonClick>::lua_func));

bind(method("eventMouseButtonPressed", delegate_bind<MyGUI::Widget,
            MyGUI::WidgetInput,
            MyGUI::EventHandle_WidgetIntIntButton,
            &MyGUI::WidgetInput::eventMouseButtonPressed>::lua_func));

bind(method("eventMouseButtonReleased", delegate_bind<MyGUI::Widget,
            MyGUI::WidgetInput,
            MyGUI::EventHandle_WidgetIntIntButton,
            &MyGUI::WidgetInput::eventMouseButtonReleased>::lua_func));

SB_META_METHOD(findWidget)
SB_META_METHOD(setProperty)
SB_META_METHOD(changeWidgetSkin)


bind(method("createWidget", static_cast<MyGUI::Widget*(MyGUI::Widget::*)(const std::string&, const std::string&, const MyGUI::IntCoord&, MyGUI::Align, const std::string& _name)>(&MyGUI::Widget::createWidgetT)));
SB_META_END_KLASS_BIND()

SB_META_DECLARE_OBJECT(MyGUI::ISubWidget, MyGUI::ICroppedRectangle)
SB_META_DECLARE_OBJECT(MyGUI::ISubWidgetRect, MyGUI::ISubWidget)
SB_META_DECLARE_OBJECT(MyGUI::ISubWidgetText, MyGUI::ISubWidget)

SB_META_DECLARE_OBJECT(MyGUI::EditText, MyGUI::ISubWidgetText)


SB_META_DECLARE_OBJECT(MyGUI::ImageBox, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::TextBox, MyGUI::Widget)
SB_META_BEGIN_KLASS_BIND(MyGUI::TextBox)
SB_META_PROPERTY_RW(caption, getCaption, setCaption)
SB_META_END_KLASS_BIND()


SB_META_DECLARE_OBJECT(MyGUI::Button, MyGUI::TextBox)
SB_META_BEGIN_KLASS_BIND(MyGUI::Button)
SB_META_PROPERTY_RW(stateSelected, getStateSelected, setStateSelected)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_OBJECT(MyGUI::MenuItem, MyGUI::Button)

SB_META_DECLARE_OBJECT(MyGUI::Window, MyGUI::TextBox)
SB_META_BEGIN_KLASS_BIND(MyGUI::Window)
SB_META_PROPERTY_RW("movable", getMovable, setMovable)
SB_META_METHOD(setVisibleSmooth)
SB_META_METHOD(destroySmooth)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_OBJECT(MyGUI::ScrollBar, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::MenuControl, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::PopupMenu, MyGUI::MenuControl)

SB_META_DECLARE_OBJECT(MyGUI::EditBox, MyGUI::TextBox)

SB_META_DECLARE_OBJECT(MyGUI::ComboBox, MyGUI::EditBox)

SB_META_DECLARE_OBJECT(MyGUI::TabItem, MyGUI::TextBox)

SB_META_DECLARE_OBJECT(MyGUI::MenuBar, MyGUI::MenuControl)

SB_META_DECLARE_OBJECT(MyGUI::ListBox, MyGUI::Widget)

SB_META_DECLARE_OBJECT(MyGUI::DDContainer, MyGUI::Widget)
SB_META_BEGIN_KLASS_BIND(MyGUI::DDContainer)
SB_META_PROPERTY_RW("needDragDrop", getNeedDragDrop, setNeedDragDrop)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_OBJECT(MyGUI::ItemBox, MyGUI::DDContainer)
SB_META_BEGIN_KLASS_BIND(MyGUI::ItemBox)
bind(method("requestCreateWidgetItem", delegate_bind<MyGUI::ItemBox,
            MyGUI::ItemBox,
            MyGUI::EventHandle_ItemBoxPtrWidgetPtr,
            &MyGUI::ItemBox::requestCreateWidgetItem>::lua_func));
bind(method("requestCoordItem", delegate_bind<MyGUI::ItemBox,
            MyGUI::ItemBox,
            MyGUI::EventHandle_ItemBoxPtrIntCoordRefBool,
            &MyGUI::ItemBox::requestCoordItem>::lua_func));

SB_META_METHOD(addItem)
SB_META_METHOD(getIndexByWidget)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::CachedWidget)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::SceneWidget)
SB_META_PROPERTY_RW("Scene", getScene, setScene)
SB_META_END_KLASS_BIND()

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

#ifdef MYGUI_USE_FREETYPE
SB_META_DECLARE_OBJECT(MyGUI::ResourceTrueTypeFont, MyGUI::IFont)
#endif

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
SB_META_METHOD(destroyWidget)
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
        
        void register_ScrollList(lua_State* L);
        
        void register_mygui( lua_State* lua ) {
            luabind::RawClass<MyGUI::MouseButton>(lua);
            
            luabind::RawClass<MyGUI::Align>(lua);
            luabind::RawClass<MyGUI::IntPoint>(lua);
            luabind::RawClass<MyGUI::IntSize>(lua);
            luabind::RawClass<MyGUI::IntCoord>(lua);
            luabind::RawClass<MyGUI::IntRect>(lua);
            luabind::RawClass<MyGUI::Colour>(lua);
            luabind::RawClass<MyGUI::IBDrawItemInfo>(lua);
            
            luabind::ExternClass<MyGUI::ICroppedRectangle>(lua);
            luabind::ExternClass<MyGUI::Widget>(lua);
            luabind::ExternClass<MyGUI::LayoutManager>(lua);
            luabind::ExternClass<MyGUI::LayerManager>(lua);
            luabind::ExternClass<MyGUI::ResourceManager>(lua);
            luabind::ExternClass<MyGUI::WidgetManager>(lua);
            luabind::ExternClass<MyGUI::Gui>(lua);

            luabind::ExternClass<MyGUI::TextBox>(lua);
            luabind::ExternClass<MyGUI::Button>(lua);
            luabind::ExternClass<MyGUI::Window>(lua);
            luabind::ExternClass<MyGUI::DDContainer>(lua);
            luabind::ExternClass<MyGUI::ItemBox>(lua);
            
            luabind::ExternClass<CachedWidget>(lua);
            luabind::ExternClass<SceneWidget>(lua);
            
            register_ScrollList(lua);
        }
        
        void setup_singletons( LuaVM* lua ) {
            LuaContextPtr ctx = lua->GetGlobalContext();
            
            //ctx->SetValue("mygui.", this);
            //ctx->SetValue("settings", settings);

        }
    }
}

