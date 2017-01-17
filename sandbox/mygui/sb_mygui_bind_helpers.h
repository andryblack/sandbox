//
//

#ifndef __sb_mygui_bind_helpers_
#define __sb_mygui_bind_helpers_

#include "MyGUI_Delegate.h"
#include "luabind/sb_luabind_function.h"

         
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
        
        template <class O,class T,class A0,class A1,
        MyGUI::EventPair<MyGUI::delegates::CMultiDelegate1<A0>, MyGUI::delegates::CMultiDelegate1<A1> > T::*obj>
        struct delegate_bind<O,T,MyGUI::EventPair<MyGUI::delegates::CMultiDelegate1<A0>, MyGUI::delegates::CMultiDelegate1<A1> >, obj > {
            typedef LuaDelegate1<A1> LuaDelegate;
            typedef MyGUI::delegates::IDelegate1<A1>* IDelegatePtr;
            typedef MyGUI::EventPair<MyGUI::delegates::CMultiDelegate1<A0>, MyGUI::delegates::CMultiDelegate1<A1> > MultiDelegate;
            static int lua_func( lua_State* L ) {
                O* t = Sandbox::luabind::stack<O*>::get(L,1);
                LuaDelegate* delegate = new LuaDelegate();
                lua_pushvalue(L, 2);
                delegate->SetObject(L);
                MultiDelegate& md(t->*obj);
                //md.clear();
                md += IDelegatePtr(delegate);
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
        
  
#endif /* defined(__sb_mygui_bind_helpers_) */
