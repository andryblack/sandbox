//
//  sb_luabind_wrapper.h
//  YinYang
//
//  Created by Андрей Куницын on 7/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_luabind_wrapper_h
#define YinYang_sb_luabind_wrapper_h

#include "sb_luabind_stack.h"
#include "sb_luabind_ref.h"
#include "sb_traits.h"
#include "impl/sb_luabind_wrapper_base.h"

namespace Sandbox {
    namespace luabind {
        
        
        class WrapperHolder {
        public:
            lua_State* GetVM() const {
                LuaVMHelperPtr helper = m_ref.GetHelper();
                if (!helper)
                    return 0;
                return helper->lua;
            }
            void push(lua_State* L) const {
                m_ref.GetObject(L);
            }
            WrapperHolder( ) : m_ref(LuaVMHelperWeakPtr()) {}
            void set(lua_State* L) {
                if (!m_ref.Valid()) {
                    m_ref.SetObject(L);
                }
            }
            void reset() {
                m_ref.Reset();
            }
        private:
            LuaReference    m_ref;
        };
        
        class wrapper : public impl::wrapper_base<WrapperHolder> {
        public:
            wrapper() : m_mark_destroy(false) {
            }
            virtual ~wrapper() {}
            void MarkDestroy(){
                sb_assert(!m_mark_destroy);
                m_mark_destroy = true;
                m_self.reset();
            }
            bool MarkedDestroy() const { return m_mark_destroy; }
            void SetObject(lua_State* L) {
                m_self.set(L);
            }
            void PushObject(lua_State* L) {
                m_self.push(L);
            }
            void ResetLuaRef() {
                m_self.reset();
            }
            static const meta::type_info* const* get_parents();
        private:
            bool    m_mark_destroy;
        };
        
        template <class T> struct wrapper_helper {
            static wrapper* get_wrapper_func( lua_State* L, int idx ) {
                return stack<T*>::get(L,idx);
            }
        };
        
#define SB_META_DECLARE_BINDING_OBJECT_WRAPPER_X(Klass,Parent,Line) \
        namespace Sandbox { namespace meta { \
            namespace Line {\
                static const type_info_parent parents[] = { \
                    { \
                        type<Parent>::private_info, \
                        &cast_helper<Klass,Parent>::raw, \
                        &cast_helper<Klass,Parent>::shared \
                    }, \
                    { \
                        type<Sandbox::luabind::wrapper>::info(), \
                        &cast_helper<Klass,Sandbox::luabind::wrapper>::raw, \
                        &cast_helper<Klass,Sandbox::luabind::wrapper>::shared \
                    }, \
                    { 0, 0, 0 } \
                };\
                static const type_info ti = { \
                    #Klass, \
                    sizeof(Klass), \
                    parents \
                }; \
            } \
            template <> const type_info* type<Klass>::private_info = &Line::ti; \
        }} 
#define SB_META_DECLARE_BINDING_OBJECT_WRAPPER(Klass,Parent) \
        SB_META_DECLARE_BINDING_OBJECT_WRAPPER_X(Klass,Parent,ANONYMOUS_VARIABLE(private_))\
        const Sandbox::meta::type_info* Klass::get_static_type_info() {\
            return Sandbox::meta::type<Klass>::info(); \
        }\
        const Sandbox::meta::type_info* Klass::get_type_info() const { \
            return get_static_type_info(); \
        }
    }
}

#endif
