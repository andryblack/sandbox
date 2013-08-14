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
                lua_pushliteral(m_L, "__props");
                lua_rawget(m_L, -2 );                 /// props
                sb_assert(lua_istable(m_L, -1)); 
                lua_createtable(m_L, 0, 2);                     /// props tbl
                prop_ptr* ptr = 
                reinterpret_cast<prop_ptr*>(lua_newuserdata(m_L, sizeof(prop_ptr)));    /// props tbl ud
                *ptr = prop.prop;
                lua_pushvalue(m_L, -1);                         /// props tbl ud ud
                lua_pushcclosure(m_L, &get_prop_func<U>, 1);    /// props tbl ud func
                lua_setfield(m_L, -3, "get");                   /// props tbl ud
                lua_pushcclosure(m_L, &set_prop_func<U>, 1);    /// props tbl func
                lua_setfield(m_L, -2, "set");                   /// props tbl 
                lua_setfield(m_L, -2, prop.name);               /// props 
                lua_pop(m_L, 1);
            }
            template <class Getter>
            void operator()( const meta::property_holder_ro<T, Getter>& prop ) {
                sb_assert(lua_istable(m_L, -1)); 
                lua_pushliteral(m_L, "__props");
                lua_rawget(m_L, -2 );                 /// props
                sb_assert(lua_istable(m_L, -1)); 
                lua_createtable(m_L, 0, 2);                     /// props tbl
                Getter* ptr = 
                reinterpret_cast<Getter*>(lua_newuserdata(m_L, sizeof(Getter)));    /// props tbl ud
                *ptr = prop.getter;
                lua_pushcclosure(m_L, &method_helper<Getter>::call, 1);    /// props tbl func
                lua_setfield(m_L, -2, "get");                   /// props tbl 
                lua_setfield(m_L, -2, prop.name);               /// props 
                lua_pop(m_L, 1);
            }
            template <class Setter>
            void operator()( const meta::property_holder_wo<T, Setter>& prop ) {
                sb_assert(lua_istable(m_L, -1));
                lua_pushliteral(m_L, "__props");
                lua_rawget(m_L, -2 );                 /// props
                sb_assert(lua_istable(m_L, -1));
                lua_createtable(m_L, 0, 2);                     /// props tbl
                Setter* set_ptr =
                reinterpret_cast<Setter*>(lua_newuserdata(m_L, sizeof(Setter)));    /// props tbl ud
                *set_ptr = prop.setter;
                lua_pushcclosure(m_L, &method_helper<Setter>::call, 1);    /// props tbl func
                lua_setfield(m_L, -2, "set");                  /// props tbl
                lua_setfield(m_L, -2, prop.name);               /// props
                lua_pop(m_L, 1);
            }
            template <class Getter,class Setter>
            void operator()( const meta::property_holder_rw<T, Getter, Setter>& prop ) {
                sb_assert(lua_istable(m_L, -1)); 
                lua_pushliteral(m_L, "__props");
                lua_rawget(m_L, -2 );                 /// props
                sb_assert(lua_istable(m_L, -1)); 
                lua_createtable(m_L, 0, 2);                     /// props tbl
                Getter* get_ptr = 
                reinterpret_cast<Getter*>(lua_newuserdata(m_L, sizeof(Getter)));    /// props tbl ud
                *get_ptr = prop.getter;
                lua_pushcclosure(m_L, &method_helper<Getter>::call, 1);    /// props tbl func
                lua_setfield(m_L, -2, "get");                   /// props tbl 
                Setter* set_ptr = 
                reinterpret_cast<Setter*>(lua_newuserdata(m_L, sizeof(Setter)));    /// props tbl ud
                *set_ptr = prop.setter;
                lua_pushcclosure(m_L, &method_helper<Setter>::call, 1);    /// props tbl func
                lua_setfield(m_L, -2, "set");                   /// props tbl 
                lua_setfield(m_L, -2, prop.name);               /// props 
                lua_pop(m_L, 1);
            }
            template <class Func>
            void operator()( const meta::method_holder<Func>& func ) {
                sb_assert(lua_istable(m_L, -1)); 
                lua_pushliteral(m_L, "__methods");
                lua_rawget(m_L, -2 );                 /// methods
                sb_assert(lua_istable(m_L, -1)); 
                Func* ptr = 
                reinterpret_cast<Func*>(lua_newuserdata(m_L, sizeof(Func)));    /// methods ud
                *ptr = func.func;
                lua_pushcclosure(m_L, &method_helper<Func>::call, 1);
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
                    "__sub"
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
                stack<U>::push(L,&stack<T>::get(L,1)->*ptr);
                return 1;
            }
            template <class U>
            static int set_prop_func( lua_State* L ) {
                typedef typename meta::property_holder<T, U>::prop_ptr prop_ptr;
                prop_ptr ptr = *reinterpret_cast<prop_ptr*>(lua_touserdata(L, lua_upvalueindex(1)));
                (&stack<T>::get(L,1)->*ptr) = stack<U>::get(L,2);
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
        template <class T,bool is_wrapper>
        struct shared_klass_constructor_helper;
        
        template <class T> struct shared_klass_constructor_helper<T,false>{
            typedef T* (*constructor_func)(lua_State*);
            static int constructor( lua_State* L ) {
                data_holder* holder = reinterpret_cast<data_holder*>(lua_newuserdata(L, 
                                                                                     sizeof(data_holder)+
                                                                                     sizeof(sb::shared_ptr<T>)));
                holder->type = data_holder::shared_ptr;
                holder->is_const = false;
                holder->info = meta::type<T>::info();
                holder->destructor = &destructor_helper<T>::shared;
                void* data = holder+1;
                constructor_func func = *reinterpret_cast<constructor_func*>(lua_touserdata(L, lua_upvalueindex(1)));
                new (data) sb::shared_ptr<T>((*func)(L));
                lua_set_metatable( L, *holder );
                return 1;
            }
        };
        template <class T> struct shared_klass_constructor_helper<T,true>{
            typedef typename shared_klass_constructor_helper<T,false>::constructor_func constructor_func;
            static bool wrapper_lock( wrapper_holder* w ,lua_State* L,int idx) {
                //LogDebug() << "wrapper_lock " << w;
                sb::shared_ptr<T>* obj_ptr = reinterpret_cast<sb::shared_ptr<T>*>(w+1);
                if (!*obj_ptr) {
                    sb::weak_ptr<T>* wek_ptr = reinterpret_cast<sb::weak_ptr<T>*>(obj_ptr+1);
                    *obj_ptr = wek_ptr->lock();
                }
                if (*obj_ptr) {
                    lua_pushvalue(L, idx);
                    (*obj_ptr)->SetObject(L);
                    return true;
                }
                return false;
            }
            static void wrapper_unlock( wrapper_holder* w ) {
                //LogDebug() << "wrapper_unlock " << w;
                sb::shared_ptr<T>* obj_ptr = reinterpret_cast<sb::shared_ptr<T>*>(w+1);
                sb::shared_ptr<T> ref = *obj_ptr;
                obj_ptr->reset();
                (void)ref;
            }
            static void wrapper_deleter( wrapper_holder* w, const T* obj ) {
                //LogDebug() << "wrapper_deleter " << w;
                if (obj->MarkedDestroy()) {
                    delete obj;
                } else {
                    sb::shared_ptr<T>* obj_ptr = reinterpret_cast<sb::shared_ptr<T>*>(w+1);
                    sb::weak_ptr<T>* wek_ptr = reinterpret_cast<sb::weak_ptr<T>*>(obj_ptr+1);
                    *obj_ptr = wek_ptr->lock();
                    const_cast<T*>(obj)->ResetLuaRef();
                }
            }
            static void wrapper_destructor( data_holder* h ) {
                //LogDebug() << "wrapper_destructor " << h;
                typedef sb::shared_ptr<T> shared_ptr_type;
                typedef sb::weak_ptr<T> weak_ptr_type;
                
                wrapper_holder* w = reinterpret_cast<wrapper_holder*>(h);
                shared_ptr_type* obj_ptr = reinterpret_cast<shared_ptr_type*>(w+1);
                weak_ptr_type* wek_ptr = reinterpret_cast<weak_ptr_type*>(obj_ptr+1);
                T* obj = obj_ptr->get();
                obj->MarkDestroy();
                obj_ptr->~shared_ptr_type();
                wek_ptr->~weak_ptr_type();
            }
            static int constructor( lua_State* L ) {
                wrapper_holder* holder = reinterpret_cast<wrapper_holder*>(lua_newuserdata(L, 
                                                                                           sizeof(wrapper_holder)+
                                                                                           sizeof(sb::shared_ptr<T>)+
                                                                                           sizeof(sb::weak_ptr<T>)));
                holder->type = data_holder::wrapper_ptr;
                holder->is_const = false;
                holder->info = meta::type<T>::info();
                holder->destructor = &shared_klass_constructor_helper<T,true>::wrapper_destructor;
                holder->lock = &shared_klass_constructor_helper<T,true>::wrapper_lock;
                holder->unlock = &shared_klass_constructor_helper<T,true>::wrapper_unlock;
                void* data = holder+1;
                constructor_func func = *reinterpret_cast<constructor_func*>(lua_touserdata(L, lua_upvalueindex(1)));
                sb::shared_ptr<T>* obj_ptr = new (data) sb::shared_ptr<T>((*func)(L),
                                                                          sb::bind(
                                                                          &shared_klass_constructor_helper<T,true>::wrapper_deleter,
                                                                                   holder,
                                                                                   sb::placeholders::_1
                                                                                   ));
                new (obj_ptr+1) sb::weak_ptr<T>(*obj_ptr);
                lua_set_metatable( L, *holder );
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
                typedef typename shared_klass_constructor_helper<T,false>::constructor_func constructor_func;
                constructor_func* ptr = 
                reinterpret_cast<constructor_func*>(lua_newuserdata(this->m_L, sizeof(constructor_func)));    /// mntbl in ud
                *ptr = &constructor_helper<Proto,2>::template raw<T>;
                lua_pushcclosure(this->m_L, &shared_klass_constructor_helper<T,is_wrapper_helper<T>::result>::constructor, 1);   /// mntbl in ctr
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
