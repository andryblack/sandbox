//
//  sb_luabind_registrators.h
//  YinYang
//
//  Created by Андрей Куницын on 7/12/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_luabind_registrators_h
#define YinYang_sb_luabind_registrators_h

#include "meta/sb_meta_bind.h"
#include "sb_luabind_method.h"
#include <sbstd/sb_function.h>
#include "../../sb_log.h"

namespace Sandbox {
	namespace luabind {
    class wrapper;
    namespace impl {
        
        static const int __get = 1;
        static const int __set = 2;
        static const int __props = 2;
        static const int __methods = 3;
        
        class registrator_base {
        public:
        protected:
            registrator_base( lua_State* L ) : m_L(L) {}
            lua_State* m_L;
        };
        template <class T>
        class enum_registrator : public registrator_base {
        public:
            explicit enum_registrator( lua_State* L ) : registrator_base(L) {}
            void operator()( const meta::static_value_holder<T>& value ) {
                sb_assert(lua_istable(m_L, -1)); 
                lua_pushstring(m_L, value.name);
                stack<T>::push(m_L,value.value);
                lua_rawset(this->m_L, -3); 
            }
            static int compare_func( lua_State* L ) {
                lua_pushboolean(L, (stack<T>::get(L,1)==stack<T>::get(L,2)) ? 1 : 0 );
                return 1;
            }
        };
        
        template <class T,int base> struct method_helper<int(T::*)(lua_State*),base> {
            static int call( lua_State* L ) {
                typedef int (T::*Func)(lua_State*);
				Func func = *reinterpret_cast<Func*>(lua_touserdata(L, lua_upvalueindex(1)));
				return (&stack<T>::get(L,base+0)->*func)(L);
			}
        };
        
        template <class T>
        class klass_registrator : public registrator_base {
        public:
            explicit klass_registrator( lua_State* L ) : registrator_base(L) {}
            template <class U>
            void operator()( const meta::property_holder<T, U>& prop ) {
                typedef typename meta::property_holder<T, U>::prop_ptr prop_ptr;
                sb_assert(lua_istable(m_L, -1)); 
                lua_rawgeti(m_L, -1 , __props);                 /// props
                sb_assert(lua_istable(m_L, -1)); 
                lua_createtable(m_L, 2, 0);                     /// props tbl
                prop_ptr* ptr = 
                reinterpret_cast<prop_ptr*>(lua_newuserdata(m_L, sizeof(prop_ptr)));    /// props tbl ud
                *ptr = prop.prop;
                lua_pushvalue(m_L, -1);                         /// props tbl ud ud
                lua_pushcclosure(m_L, &get_prop_func<U>, 1);    /// props tbl ud func
                lua_rawseti(m_L, -3, __get);                   /// props tbl ud
                lua_pushcclosure(m_L, &set_prop_func<U>, 1);    /// props tbl func
                lua_rawseti(m_L, -2, __set);                   /// props tbl
                lua_setfield(m_L, -2, prop.name);               /// props 
                lua_pop(m_L, 1);
            }
            template <class Getter>
            void operator()( const meta::property_holder_ro<T, Getter>& prop ) {
                sb_assert(lua_istable(m_L, -1)); 
                lua_rawgeti(m_L, -1 , __props);                 /// props
                sb_assert(lua_istable(m_L, -1)); 
                lua_createtable(m_L, 2, 0);                     /// props tbl
                Getter* ptr = 
                reinterpret_cast<Getter*>(lua_newuserdata(m_L, sizeof(Getter)));    /// props tbl ud
                *ptr = prop.getter;
                lua_pushcclosure(m_L, &method_helper<Getter>::call, 1);    /// props tbl func
                lua_rawseti(m_L, -2, __get);                   /// props tbl
                lua_setfield(m_L, -2, prop.name);               /// props 
                lua_pop(m_L, 1);
            }
            template <class Setter>
            void operator()( const meta::property_holder_wo<T, Setter>& prop ) {
                sb_assert(lua_istable(m_L, -1));
                lua_rawgeti(m_L, -1 , __props);                 /// props
                sb_assert(lua_istable(m_L, -1));
                lua_createtable(m_L, 2, 0);                     /// props tbl
                Setter* set_ptr =
                reinterpret_cast<Setter*>(lua_newuserdata(m_L, sizeof(Setter)));    /// props tbl ud
                *set_ptr = prop.setter;
                lua_pushcclosure(m_L, &method_helper<Setter>::call, 1);    /// props tbl func
                lua_rawseti(m_L, -2, __set);                  /// props tbl
                lua_setfield(m_L, -2, prop.name);               /// props
                lua_pop(m_L, 1);
            }
            void operator()( const meta::property_holder_wo<void,int(*)(lua_State*)>& prop ) {
                sb_assert(lua_istable(m_L, -1));
                lua_rawgeti(m_L, -1 , __props);                 /// props
                sb_assert(lua_istable(m_L, -1));
                lua_createtable(m_L, 2, 0);                     /// props tbl
                lua_pushcclosure(m_L, prop.setter, 0);    /// props tbl func
                lua_setfield(m_L, -2, __set);                  /// props tbl
                lua_setfield(m_L, -2, prop.name);               /// props
                lua_pop(m_L, 1);
            }
            template <class Getter,class Setter>
            void operator()( const meta::property_holder_rw<T, Getter, Setter>& prop ) {
                sb_assert(lua_istable(m_L, -1)); 
                lua_rawgeti(m_L, -1 , __props);                   /// props
                sb_assert(lua_istable(m_L, -1)); 
                lua_createtable(m_L, 2, 0);                     /// props tbl
                Getter* get_ptr = 
                reinterpret_cast<Getter*>(lua_newuserdata(m_L, sizeof(Getter)));    /// props tbl ud
                *get_ptr = prop.getter;
                lua_pushcclosure(m_L, &method_helper<Getter>::call, 1);    /// props tbl func
                lua_rawseti(m_L, -2, __get);                   /// props tbl
                Setter* set_ptr = 
                reinterpret_cast<Setter*>(lua_newuserdata(m_L, sizeof(Setter)));    /// props tbl ud
                *set_ptr = prop.setter;
                lua_pushcclosure(m_L, &method_helper<Setter>::call, 1);    /// props tbl func
                lua_rawseti(m_L, -2, __set);                   /// props tbl
                lua_setfield(m_L, -2, prop.name);               /// props 
                lua_pop(m_L, 1);
            }
            template <class Func>
            void operator()( const meta::method_holder<Func>& func ) {
                sb_assert(lua_istable(m_L, -1)); 
                lua_rawgeti(m_L, -1 , __methods);                 /// methods
                sb_assert(lua_istable(m_L, -1)); 
                Func* ptr = 
                reinterpret_cast<Func*>(lua_newuserdata(m_L, sizeof(Func)));    /// methods ud
                *ptr = func.func;
                lua_pushcclosure(m_L, &method_helper<Func>::call, 1);
                lua_setfield(m_L, -2, func.name);               /// methods 
                lua_pop(m_L, 1);
            }
            void operator()( const meta::method_holder<int(*)(lua_State*)>& func ) {
                sb_assert(lua_istable(m_L, -1));
                lua_rawgeti(m_L, -1 , __methods);                  /// methods
                sb_assert(lua_istable(m_L, -1));
                lua_pushcclosure(m_L, func.func, 0);
                lua_setfield(m_L, -2, func.name);               /// methods
                lua_pop(m_L, 1);
            }
            template <class Func>
            void operator()( const meta::operator_holder<Func>& func ) {
                sb_assert(lua_istable(m_L, -1)); 
                lua_pushliteral(m_L, "__metatable");
                lua_rawget(m_L, -2 );                 /// metatable
                sb_assert(lua_istable(m_L, -1)); 
                Func* ptr = 
                reinterpret_cast<Func*>(lua_newuserdata(m_L, sizeof(Func)));    /// methods ud
                *ptr = func.func;
                lua_pushcclosure(m_L, &method_helper<Func>::call, 1);
                static const char* meta_operator[] = {
                    "__add",
                    "__mul",
                    "__sub",
                    "__index"
                };
                lua_setfield(m_L, -2, meta_operator[func.name]);               /// methods 
                lua_pop(m_L, 1);
            }
            template <class Func>
            void operator()( const meta::static_method_holder<Func>& func ) {
                sb_assert(lua_istable(m_L, -1)); 
                lua_pushstring(m_L, func.name);
                Func* ptr = 
                reinterpret_cast<Func*>(lua_newuserdata(m_L, sizeof(Func)));    /// name ud
                *ptr = func.func;
                lua_pushcclosure(m_L, &method_helper<Func,1>::call, 1); /// name method
                lua_rawset(this->m_L, -3); 
            }
            template <class Type>
            void operator()( const meta::static_value_holder<Type>& value ) {
                sb_assert(lua_istable(m_L, -1)); 
                lua_pushstring(m_L, value.name);
                stack<Type>::push(m_L,value.value);
                lua_rawset(this->m_L, -3); 
            }
        private:
            template <class U>
            static int get_prop_func( lua_State* L ) {
                typedef typename meta::property_holder<T, U>::prop_ptr prop_ptr;
                prop_ptr ptr = *reinterpret_cast<prop_ptr*>(lua_touserdata(L, lua_upvalueindex(1)));
                stack<U>::push(L,stack<T>::get(L,1).*ptr);
                return 1;
            }
            template <class U>
            static int set_prop_func( lua_State* L ) {
                typedef typename meta::property_holder<T, U>::prop_ptr prop_ptr;
                prop_ptr ptr = *reinterpret_cast<prop_ptr*>(lua_touserdata(L, lua_upvalueindex(1)));
                (stack<T*>::get(L,1)->*ptr) = stack<U>::get(L,2);
                return 0;
            }
        protected:
            
        };
        template <class T>
        class raw_klass_registrator : public klass_registrator<T> {
        public:
            explicit raw_klass_registrator( lua_State* L ) : klass_registrator<T>(L) {}
            using klass_registrator<T>::operator ();
            template <typename Proto>
            void operator() ( const meta::constructor_holder<Proto>& ) {
                sb_assert(lua_istable(this->m_L, -1)); 
                lua_pushliteral(this->m_L, "__constructor");         /// mntbl name
                constructor_func* ptr = 
                reinterpret_cast<constructor_func*>(lua_newuserdata(this->m_L, sizeof(constructor_func)));    /// mntbl ud
                *ptr = &constructor_helper<Proto,2>::template inplace<T>;
                lua_pushcclosure(this->m_L, &raw_klass_registrator<T>::constructor, 1);   /// mntbl name ctr
                lua_rawset(this->m_L, -3);                    /// mntbl 
            }
            void operator() ( const meta::constructor_ptr_holder<int(*)(lua_State*)>& hdr) {
                sb_assert(lua_istable(this->m_L, -1));
                lua_pushliteral(this->m_L, "__constructor");         /// mntbl name
                lua_pushcclosure(this->m_L, hdr.func, 0);     /// mntbl name ctr
                lua_rawset(this->m_L, -3);                    /// mntbl
            }
        private:
            typedef void (*constructor_func)(lua_State*,void*);
            static int constructor( lua_State* L ) {
                data_holder* holder = reinterpret_cast<data_holder*>(lua_newuserdata(L, 
                                                                                     sizeof(data_holder)+
                                                                                     sizeof(T)));
                holder->type = data_holder::raw_data;
                holder->is_const = false;
                holder->info = meta::type<T>::info();
                holder->destructor = &destructor_helper<T>::raw;
                void* data = holder+1;
                constructor_func func = *reinterpret_cast<constructor_func*>(lua_touserdata(L, lua_upvalueindex(1)));
                (*func)(L,data);
                lua_set_metatable( L, *holder );
                return 1;
            }
        };
        
        template <class Type>
        class is_wrapper_helper
        {
            class yes { char m;};
            class no { yes m[2];};
            
            static yes deduce(wrapper*);
            static no deduce(...);
            
        public:
            enum { result = sizeof(yes) == sizeof(deduce((Type*)(0))) };
        };
        template <class T>
        struct shared_klass_constructor_helper {
            typedef T* (*constructor_func)(lua_State*);
            static int constructor( lua_State* L ) {
                constructor_func func = *reinterpret_cast<constructor_func*>(lua_touserdata(L, lua_upvalueindex(1)));
                stack<sb::intrusive_ptr<T> >::push(L,sb::intrusive_ptr<T>((*func)(L)));
                return 1;
            }
        };
        
        template <class T>
        class shared_klass_registrator : public klass_registrator<T> {
        public:
            explicit shared_klass_registrator( lua_State* L ) : klass_registrator<T>(L) {}
            using klass_registrator<T>::operator ();
            template <typename Proto>
            void operator() ( const meta::constructor_holder<Proto>& ) {
                sb_assert(lua_istable(this->m_L, -1)); 
                lua_pushliteral(this->m_L, "__constructor");         /// mntbl in ctr name
                typedef typename shared_klass_constructor_helper<T>::constructor_func constructor_func;
                constructor_func* ptr = 
                reinterpret_cast<constructor_func*>(lua_newuserdata(this->m_L, sizeof(constructor_func)));    /// mntbl in ud
                *ptr = &constructor_helper<Proto,2>::template raw<T>;
                lua_pushcclosure(this->m_L, &shared_klass_constructor_helper<T>::constructor, 1);   /// mntbl in ctr
                lua_rawset(this->m_L, -3);
            }
            void operator() ( const meta::constructor_ptr_holder<int(*)(lua_State*)>& hdr) {
                sb_assert(lua_istable(this->m_L, -1)); 
                lua_pushliteral(this->m_L, "__constructor");         /// mntbl name
                lua_pushcclosure(this->m_L, hdr.func, 0);     /// mntbl name ctr
                lua_rawset(this->m_L, -3);                    /// mntbl 
            }
        protected:
            typedef T* (*constructor_func)(lua_State*);
        };



    }}
}

#endif
