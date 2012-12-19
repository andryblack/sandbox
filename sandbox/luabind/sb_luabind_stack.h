//
//  sb_luabind_stack.h
//  YinYang
//
//  Created by Андрей Куницын on 7/8/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_luabind_stack_h
#define YinYang_sb_luabind_stack_h

#include "sb_notcopyable.h"
#include "meta/sb_meta.h"

#include "sb_shared_ptr.h"
#include "sb_traits.h"
#include "sb_map.h"
#include "sb_string.h"
#include "sb_vector.h"

extern "C" {
#include "../../lua/src/lua.h"
#include "../../lua/src/lauxlib.h"
}

#include "sb_luabind_metatable.h"

namespace Sandbox {
    
    namespace luabind {
 
        int lua_traceback (lua_State *L);
        
        void lua_argerror( lua_State* L, int idx, 
                          const char* expected, 
                          const char* got );
        void lua_access_error( lua_State* L, int idx, 
                              const char* got );
        
        struct data_holder {
            const meta::type_info* info;
            enum {
                raw_data,
                raw_ptr,
                shared_ptr,
                wrapper_ptr
            } type;
            bool    is_const;
            void    (*destructor)(data_holder* data);
        };
        struct wrapper_holder : data_holder {
            bool (*lock)(wrapper_holder*,lua_State*,int idx);
            void (*unlock)(wrapper_holder*);
        };
        
        template <class T>
        struct destructor_helper {
            static void raw( data_holder* d ) {
                meta::destructor_helper<T>::raw( d + 1 );
            }
            static void shared( data_holder* d ) {
                meta::destructor_helper<T>::shared( d + 1 );
            }
        };
        
        void lua_call_method(lua_State* L, int args, int res, const char* name);
        
        class wrapper;
        void lua_push_wrapper_value(lua_State* L,wrapper* wpr);
        
        template <class T>
        inline T* get_ptr( const meta::type_info* from, void* data ) {
            const meta::type_info* to = meta::type<T>::info();
            if ( from == to ) return reinterpret_cast<T*>(data);
            size_t pi = 0;
            while (from->parents && from->parents[pi].info) {
                T* r = get_ptr<T>(from->parents[pi].info, from->parents[pi].downcast(data) );
                if ( r ) return r;
                pi++;
            }
            return 0;
        }
        
        template <class T>
        inline sb::shared_ptr<T> get_shared_ptr( const meta::type_info* from, void* data ) {
            const meta::type_info* to = meta::type<T>::info();
            if ( from == to ) {
                return *reinterpret_cast<sb::shared_ptr<T>*>(data);
            }
            char buf[sizeof(sb::shared_ptr<T>)];
            size_t pi = 0;
            while (from->parents && 
                   from->parents[pi].info) {
                void (*destruct)(void*) = from->parents[pi].downcast_shared( data, buf );
                if ( destruct ) {
                    sb::shared_ptr<T> res = get_shared_ptr<T>(from->parents[pi].info, buf);
                    destruct(buf);
                    if ( res ) {
                        return res;
                    }
                }
                pi++;
            }
            return sb::shared_ptr<T>();
        }

        
        template <class T> 
        inline T* get_object_ptr( data_holder* holder , lua_State* L,int idx) {
            T* res = 0;
            const meta::type_info* info = holder->info;
            if ( holder->type == data_holder::raw_data ) {
                res = get_ptr<T>(info, holder+1);
            } else if ( holder->type == data_holder::raw_ptr ) {
                res = get_ptr<T>(info, *reinterpret_cast<void**>(holder+1));
            } else if ( holder->type == data_holder::shared_ptr ) {
                sb::shared_ptr<T> ptr = get_shared_ptr<T>(info,holder+1);
                if (ptr)
                    res = ptr.get();
            } else if ( holder->type == data_holder::wrapper_ptr ) {
                wrapper_holder* wrapper = reinterpret_cast<wrapper_holder*>(holder);
                bool lock = wrapper->lock(wrapper,L,idx);
                sb::shared_ptr<T> ptr = get_shared_ptr<T>(info,wrapper+1);
                if (lock) wrapper->unlock(wrapper);
                if (ptr)
                    res = ptr.get();
            } else {
                sb_assert(false);
            }
            return res;
        }
        
        template <class Type>
        class has_meta_object_base
        {
            class yes { char m;};
            class no { yes m[2];};
            
            static yes deduce(meta::object*);
            static no deduce(...);
            
        public:
            static const bool result = sizeof(yes) == sizeof(deduce((Type*)(0)));
        };
        
        template<bool has>
        class has_meta_object_base_hpr {
        };
        template <class T>
        inline bool try_to_push_lua_object_impl( lua_State*,const T*,const has_meta_object_base_hpr<false>* ) {
            return false;
        }
        template <class T>
        inline bool try_to_push_lua_object_impl( lua_State* L,const T* v,const has_meta_object_base_hpr<true>* ) {
            wrapper* w = get_ptr<wrapper>(v->get_type_info(), const_cast<T*>(v));
            if ( w ) {
                lua_push_wrapper_value(L,w);
                return true;
            }
            return false;
        }
        template <class T>
        inline bool try_to_push_lua_object( lua_State* L, const T* val, 
                                    const has_meta_object_base_hpr<has_meta_object_base<T>::result>* v=0 ) {
            return try_to_push_lua_object_impl(L,val,v);
        }
        
        template <class T>
        struct stack {
            static void push( lua_State* L, const T& val ) {
                data_holder* holder = reinterpret_cast<data_holder*>(lua_newuserdata(L, 
                                                                                     sizeof(data_holder)+
                                                                                     sizeof(T)));
                holder->type = data_holder::raw_data;
                holder->is_const = false;
                holder->info = meta::type<T>::info();
                holder->destructor = &destructor_helper<T>::raw;
                void* data = holder+1;
                new (data) T(val);
                lua_set_metatable( L, *holder );
            }
            static T& get_impl( lua_State* L, data_holder* holder,int idx ) {
                if ( holder->is_const ) {
                    lua_access_error( L, idx , holder->info->name );
                    return *reinterpret_cast<T*>(0);
                } 
                T* res = get_object_ptr<T>(holder,L,idx);
                if (!res) {
                    lua_argerror( L, idx, meta::type<T>::info()->name, holder->info->name );
                }
                return *res;
            }
            static T& get( lua_State* L, int idx ) {
                if ( lua_isuserdata(L, idx) ) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, idx));
                    return get_impl(L, holder,idx);
                } else if ( lua_istable(L, idx) ) {
                    lua_pushliteral(L, "__native");
                    lua_rawget(L, idx);
                    if (lua_isuserdata(L, -1)) {
                        data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, -1));
                        lua_pop(L, 1);
                        return get_impl(L, holder,idx);
                    } 
                    lua_pop(L, 1);
                } 
                lua_argerror( L, idx, meta::type<T>::info()->name, 0);
                return *reinterpret_cast<T*>(0);
            }
        };
        template <class T>
        struct stack<const T> {
            static const T& get_impl( lua_State* L, data_holder* holder,int idx ) {
                T* res = get_object_ptr<T>(holder,L,idx);
                if (!res) {
                    lua_argerror( L, idx, meta::type<T>::info()->name, holder->info->name );
                }
                return *res;
            }
            static const T& get( lua_State* L, int idx ) {
                if ( lua_isuserdata(L, idx) ) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, idx));
                    return get_impl(L, holder, idx);
                } else if ( lua_istable(L, idx) ) {
                    lua_pushliteral(L, "__native");
                    lua_rawget(L, idx);
                    if (lua_isuserdata(L, -1)) {
                        data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, -1));
                        lua_pop(L, 1);
                        return get_impl(L, holder, idx);
                    }
                    lua_pop(L, 1);
                }
                lua_argerror( L, idx, meta::type<T>::info()->name, 0 );
                return *reinterpret_cast<T*>(0);
            }
        };
        template <class T>
        struct stack<const T&> {
            static void push( lua_State* L, const T& val ) {
                stack<T>::push(L,val);
            }
            static const T& get( lua_State* L, int idx ) {
                return stack<const T>::get(L,idx);
            }
        };
        
        template <class T>
        struct stack<T*> {
            static void push( lua_State* L, T* val ) {
                if ( val ) {
                    if (try_to_push_lua_object(L,val))
                        return;
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_newuserdata(L, 
                                                                                         sizeof(data_holder)+
                                                                                         sizeof(T*)));
                    holder->type = data_holder::raw_ptr;
                    holder->is_const = false;
                    holder->info = meta::type<T>::info();
                    holder->destructor = 0;
                    T** data = reinterpret_cast<T**>(holder+1);
                    *data = val;
                    lua_set_metatable( L, *holder );
                } else {
                    lua_pushnil(L);
                }
            }
            static T* get_impl( lua_State* L, data_holder* holder,int idx ) {
                if ( holder->is_const ) {
                    lua_access_error( L, idx , holder->info->name );
                    return reinterpret_cast<T*>(0);
                } 
                T* res = get_object_ptr<T>(holder,L,idx);
                if (!res) {
                    lua_argerror( L, idx, meta::type<T>::info()->name, holder->info->name );
                } 
                return res;
            }
            static T* get( lua_State* L, int idx ) {
                if ( lua_isuserdata(L, idx) ) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, idx));
                    return get_impl(L, holder, idx);
                } else if ( lua_istable(L, idx) ) {
                    lua_pushliteral(L, "__native");
                    lua_rawget(L, idx);
                    if (lua_isuserdata(L, -1)) {
                        data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, -1));
                        lua_pop(L, 1);
                        return get_impl(L, holder, idx);
                    }
                    lua_pop(L, 1);
                } else if ( lua_isnil(L, idx) ) {
                    return reinterpret_cast<T*>(0);
                }
                lua_argerror( L, idx, meta::type<T>::info()->name, 0);
                return reinterpret_cast<T*>(0);
            }
        };
        
        template <class T>
        struct stack<const T*> {
            static void push( lua_State* L, const T* val ) {
                if ( val ) {
                    if (try_to_push_lua_object(L,val))
                        return;
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_newuserdata(L, 
                                                                                         sizeof(data_holder)+
                                                                                         sizeof(T*)));
                    holder->type = data_holder::raw_ptr;
                    holder->is_const = true;
                    holder->info = meta::type<T>::info();
                    holder->destructor = 0;
                    const T** data = reinterpret_cast<const T**>(holder+1);
                    *data = val;
                    lua_set_metatable( L, *holder );
                } else {
                    lua_pushnil(L);
                }
            }
            static const T* get_impl( lua_State* L, data_holder* holder,int idx ) {
                T* res = get_object_ptr<T>(holder,L,idx);
                if (!res) {
                    lua_argerror( L, idx, meta::type<T>::info()->name, holder->info->name );
                } 
                return res;
            }
            static const T* get( lua_State* L, int idx ) {
                if ( lua_isuserdata(L, idx) ) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, idx));
                    return get_impl(L, holder, idx);                    
                } else if ( lua_istable(L, idx) ) {
                    lua_pushliteral(L, "__native");
                    lua_rawget(L, idx);
                    if (lua_isuserdata(L, -1)) {
                        data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, -1));
                        lua_pop(L, 1);
                        return get_impl(L, holder, idx);
                    }
                    lua_pop(L, 1);
                } else if ( lua_isnil(L, idx) ) {
                    return reinterpret_cast<T*>(0);
                }
                lua_argerror( L, idx, meta::type<T>::info()->name, 0);
                return reinterpret_cast<T*>(0);
            }
        };
        
        template <class T>
        struct stack<sb::shared_ptr<T> > {
            static void push( lua_State* L, const sb::shared_ptr<T>& val ) {
                if (!val) {
                    lua_pushnil(L);
                    return;
                }
                if (try_to_push_lua_object(L,val.get()))
                    return;
                data_holder* holder = reinterpret_cast<data_holder*>(lua_newuserdata(L, 
                                                                                     sizeof(data_holder)+
                                                                                     sizeof(sb::shared_ptr<T>)));
                holder->type = data_holder::shared_ptr;
                holder->is_const = false;
                holder->info = meta::type<T>::info();
                holder->destructor = &destructor_helper<T>::shared;
                new (holder+1) sb::shared_ptr<T>(val);
                lua_set_metatable( L, *holder );
            }
            
            static sb::shared_ptr<T> get_impl( lua_State* L,  data_holder* holder, int idx ) {
                if ( holder->type==data_holder::shared_ptr ) {
                    sb::shared_ptr<T> ptr = get_shared_ptr<T>(holder->info,holder+1);
                    return ptr;
                } else if ( holder->type==data_holder::wrapper_ptr ) {
                    wrapper_holder* wrapper = reinterpret_cast<wrapper_holder*>(holder);
                    bool lock = wrapper->lock(wrapper,L,idx);
                    sb::shared_ptr<T> ptr = get_shared_ptr<T>(holder->info,wrapper+1);
                    if (lock) wrapper->unlock(wrapper);
                    return ptr;
                }
                lua_argerror( L, idx, meta::type<T>::info()->name, 0 );
                return sb::shared_ptr<T>();
            }
            static sb::shared_ptr<T> get( lua_State* L, int idx ) {
                if ( lua_isuserdata(L, idx) ) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, idx));
                    return get_impl(L, holder, idx);
                } else if ( lua_istable(L, idx) ) {
                    lua_pushliteral(L, "__native");
                    lua_rawget(L, idx);
                    if (lua_isuserdata(L, -1)) {
                        data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, -1));
                        lua_pop(L, 1);
                        return get_impl(L, holder, idx);
                    }
                    lua_pop(L, 1);
                } else if ( lua_isnil(L, idx) ) {
                    return sb::shared_ptr<T>();
                }
                lua_argerror( L, idx, meta::type<T>::info()->name, 0 );
                return sb::shared_ptr<T>();
            }
        };
        
        template <class T>
        struct stack<const sb::shared_ptr<T>& > {
            static void push( lua_State* L, const sb::shared_ptr<T>& val ) {
                stack<sb::shared_ptr<T> >::push( L,val );
            }
            static sb::shared_ptr<T> get( lua_State* L, int idx ) {
                return stack<sb::shared_ptr<T> >::get( L, idx );
            }
        };
        
        template <class T>
        struct stack<sb::map<sb::string,T> > {
            static void push( lua_State* L, const sb::map<sb::string,T>& val ) {
                lua_createtable(L, 0, val.size() );
                for (typename sb::map<sb::string,T>::const_iterator it=val.begin();it!=val.end();++it) {
                    stack<T>::push( L,it->second );
                    lua_setfield(L, -2, it->first.c_str());
                }
            }
        };
        
        template <class T>
        struct stack<const sb::map<sb::string,T>& > {
            static void push( lua_State* L, const sb::map<sb::string,T>& val ) {
                stack<sb::map<sb::string,T> >::push(L,val);
            }
            static sb::map<sb::string,T> get( lua_State* L, int idx ) {
                sb::map<sb::string,T> res;
                if (!lua_istable(L, idx)) {
                    lua_argerror(L, idx, "table", 0);
                    return res;
                }
                lua_pushnil(L);
                while (lua_next(L, idx) != 0) {
                    /* uses 'key' (at index -2) and 'value' (at index -1) */
                    if (lua_isstring(L, -2))
                        res[lua_tostring(L, -2)]=stack<T>::get(L,-1);
                    /* removes 'value'; keeps 'key' for next iteration */
                    lua_pop(L, 1);
                }
                return res;
            }
        };
        
        template <class T>
        struct stack<sb::vector<T> > {
            static void push( lua_State* L, const sb::map<sb::string,T>& val ) {
                lua_createtable(L, val.size(), 0 );
                lua_Integer idx = 1;
                for (typename sb::vector<T>::const_iterator it=val.begin();it!=val.end();++it) {
                    lua_pushinteger(L, idx);
                    stack<T>::push( L,it->second );
                    lua_settable(L, -3);
                    idx++;
                }
            }
        };
        
        template <class T>
        struct stack<const sb::vector<T>& > {
            static void push( lua_State* L, const sb::vector<T>& val ) {
                stack<sb::vector<T> >::push(L,val);
            }
            static sb::vector<T> get( lua_State* L, int idx ) {
                sb::vector<T> res;
                if (!lua_istable(L, idx)) {
                    lua_argerror(L, idx, "table", 0);
                    return res;
                }
                lua_pushnil(L);
                while (lua_next(L, idx) != 0) {
                    /* uses 'key' (at index -2) and 'value' (at index -1) */
                    res.push_back(stack<T>::get(L,-1));
                    /* removes 'value'; keeps 'key' for next iteration */
                    lua_pop(L, 1);
                }
                return res;
            }
        };
        
#ifdef SB_DEBUG
        class lua_stack_check {
        public:
            explicit lua_stack_check(lua_State* L) {
                m_L = L;
                m_top = lua_gettop(L);
            }
            ~lua_stack_check() {
                int top = lua_gettop(m_L);
                sb_assert( top == m_top );
            }
        private:
            lua_State* m_L;
            int m_top;
        };
#define LUA_CHECK_STACK lua_stack_check sc(L);
#else
#define LUA_CHECK_STACK 
#endif
        
#define LUABIND_DECLARE_RAW_STACK_TYPE(Type) \
template <> \
struct stack<Type> { \
static void push( lua_State* L, Type val ); \
static Type get( lua_State* L, int idx ); \
}; 
        
        LUABIND_DECLARE_RAW_STACK_TYPE(bool)
        LUABIND_DECLARE_RAW_STACK_TYPE(char)
        LUABIND_DECLARE_RAW_STACK_TYPE(long)
        LUABIND_DECLARE_RAW_STACK_TYPE(short)
        LUABIND_DECLARE_RAW_STACK_TYPE(int)
        LUABIND_DECLARE_RAW_STACK_TYPE(float)
        LUABIND_DECLARE_RAW_STACK_TYPE(unsigned char)
        LUABIND_DECLARE_RAW_STACK_TYPE(unsigned long)
        LUABIND_DECLARE_RAW_STACK_TYPE(unsigned short)
        LUABIND_DECLARE_RAW_STACK_TYPE(unsigned int)
        LUABIND_DECLARE_RAW_STACK_TYPE(const char*)
        
        
    
    }
}

#endif
