//
//

#ifndef __sb_mygui_bind_helpers_
#define __sb_mygui_bind_helpers_

#include "MyGUI_Delegate.h"
#include "luabind/sb_luabind_function.h"
#include "luabind/sb_luabind.h"
#include "MyGUI_Types.h"
#include "MyGUI_UString.h"
#include "MyGUI_Any.h"
#include "MyGUI_Colour.h"
#include "MyGUI_MouseButton.h"
#include "MyGUI_WidgetStyle.h"


        template <typename A1,typename A2,typename A3,typename A4>
        class LuaDelegate4 : public MyGUI::delegates::IDelegate4<A1,A2,A3,A4>, public MyGUI::delegates::IDelegateUnlink {
        private:
            Sandbox::luabind::function  m_function;
        public:
            virtual bool isType( const std::type_info& _type) { return false; }
            virtual void invoke( A1 a1, A2 a2, A3 a3, A4 a4 ) {
                if (m_function.Valid()) {
                    m_function.call( a1, a2, a3, a4 );
                }
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
                if (m_function.Valid()) {
                    m_function.call( a1, a2, a3 );
                }
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
                if (m_function.Valid()) {
                    a2 = m_function.call<A2>( a1, a3 );
                }
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
                if (m_function.Valid()) {
                    m_function.call( a1, a2 );
                }
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
                if (m_function.Valid()) {
                    m_function.call( a1 );
                }
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
                md.clear();
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
                md.clear();
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
                md.clear();
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
                md.clear();
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
                md.clear();
                md = delegate;
                return 0;
            }
        };


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
        
        
        template <class T>
        struct stack<MyGUI::types::TPoint<T> > {
            static void push( lua_State* L, const MyGUI::types::TPoint<T>& val ) {
                stack_help<MyGUI::types::TPoint<T>, false>::push(L, val);
            }
            static MyGUI::types::TPoint<T> get( lua_State* L, int idx ) {
                if (lua_istable(L, idx)) {
                    MyGUI::types::TPoint<T> res;
                    lua_rawgeti(L, idx, 1);
                    res.left = T(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 2);
                    res.top = T(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    return  res;
                }
                return stack_help<MyGUI::types::TPoint<T>, false>::get(L, idx);
            }
        };
        template <class T>
        struct stack<const MyGUI::types::TPoint<T>&> : stack<MyGUI::types::TPoint<T> > {};
        
        template <>
        struct stack<MyGUI::IntSize> {
            static void push( lua_State* L, const MyGUI::IntSize& val ) {
                stack_help<MyGUI::IntSize, false>::push(L, val);
            }
            static MyGUI::IntSize get( lua_State* L, int idx ) {
                if (lua_istable(L, idx)) {
                    MyGUI::IntSize res;
                    lua_rawgeti(L, idx, 1);
                    res.width = int(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 2);
                    res.height = int(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    return  res;
                }
                return stack_help<MyGUI::IntSize, false>::get(L, idx);
            }
        };
        template <>
        struct stack<const MyGUI::IntSize&> : stack<MyGUI::IntSize> {};
        
        template <>
        struct stack<MyGUI::IntRect> {
            static void push( lua_State* L, const MyGUI::IntRect& val ) {
                stack_help<MyGUI::IntRect, false>::push(L, val);
            }
            static MyGUI::IntRect get( lua_State* L, int idx ) {
                if (lua_istable(L, idx)) {
                    MyGUI::IntRect res;
                    lua_rawgeti(L, idx, 1);
                    res.left = int(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 2);
                    res.top = int(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 3);
                    res.right = int(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 4);
                    res.bottom = int(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    return  res;
                }
                return stack_help<MyGUI::IntRect, false>::get(L, idx);
            }
        };
        template <>
        struct stack<const MyGUI::IntRect&> : stack<MyGUI::IntRect> {};
        
        template <>
        struct stack<MyGUI::IntCoord> {
            static void push( lua_State* L, const MyGUI::IntCoord& val ) {
                stack_help<MyGUI::IntCoord, false>::push(L, val);
            }
            static MyGUI::IntCoord get( lua_State* L, int idx ) {
                if (lua_istable(L, idx)) {
                    MyGUI::IntCoord res;
                    lua_rawgeti(L, idx, 1);
                    res.left = int(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 2);
                    res.top = int(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 3);
                    res.width = int(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 4);
                    res.height = int(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    return  res;
                }
                return stack_help<MyGUI::IntCoord, false>::get(L, idx);
            }
        };
        template <>
        struct stack<const MyGUI::IntCoord&> : stack<MyGUI::IntCoord> {};
        
        template <>
        struct stack<MyGUI::Any> {
            static void push( lua_State* L, const MyGUI::Any& val ) {
                if (val.getType()==typeid(int))
                    stack<int>::push(L, *val.castType<int>());
                else if (val.getType()==typeid(float))
                    stack<float>::push(L, *val.castType<float>());
                else if (val.getType()==typeid(double))
                    stack<double>::push(L, *val.castType<double>());
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
        struct stack<MyGUI::Colour> {
            static void push( lua_State* L, const MyGUI::Colour& val ) {
                stack_help<MyGUI::Colour, false>::push(L, val);
            }
            static MyGUI::Colour get( lua_State* L, int idx ) {
                int type = lua_type(L, idx);
                if (type == LUA_TSTRING)
                    return MyGUI::Colour::parse(lua_tostring(L, idx));
                return stack_help<MyGUI::Colour, false>::get(L, idx);
            }
        };
        template <>
        struct stack<const MyGUI::Colour&> : stack<MyGUI::Colour> {};
        
        template <>
        struct stack<MyGUI::Align> {
            static void push( lua_State* L, const MyGUI::Align& val ) {
                stack_help<MyGUI::Align, false>::push(L, val);
            }
            static MyGUI::Align get( lua_State* L, int idx ) {
                int type = lua_type(L, idx);
                if (type == LUA_TSTRING)
                    return MyGUI::Align::parse(lua_tostring(L, idx));
                return stack_help<MyGUI::Align, false>::get(L, idx);
            }
        };
        template <>
        struct stack<const MyGUI::Align&> : stack<MyGUI::Align> {};
        
        
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
        
        template <>
        struct stack<MyGUI::WidgetStyle> {
            static void push( lua_State* L, const MyGUI::WidgetStyle& val ) {
                stack<int>::push(L, val.getValue());
            }
            static MyGUI::WidgetStyle get( lua_State* L, int idx ) {
                return MyGUI::WidgetStyle(static_cast<MyGUI::WidgetStyle::Enum>(stack<int>::get(L, idx)));
            }
        };
        
        template <>
        struct stack<const MyGUI::WidgetStyle&> : stack<MyGUI::WidgetStyle> {};
        
    }

}
        
  
#endif /* defined(__sb_mygui_bind_helpers_) */
