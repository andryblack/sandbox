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

#include <sbstd/sb_shared_ptr.h>
#include <sbstd/sb_traits.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_string.h>
#include <sbstd/sb_vector.h>

extern "C" {
#include <lua/lua.h>
#include <lua/lauxlib.h>
}

#include "sb_luabind_metatable.h"
#include <exception>

namespace Sandbox {
    
    namespace luabind {
    
 
        int lua_traceback (lua_State *L);
        
        void lua_argerror( lua_State* L, int idx, 
                          const char* expected, 
                          const char* got )
#ifdef __clang__
        __attribute__((noreturn))
#endif
        ;
        void lua_access_error( lua_State* L, int idx, 
                              const char* got )
#ifdef __clang__
        __attribute__((noreturn))
#endif
        ;
        
        struct data_holder {
            const meta::type_info* info;
            enum {
                raw_data,
                raw_ptr,
                shared_ptr,
                intrusive_ptr
            } type:8;
            bool    is_const:8;
            void    (*destructor)(data_holder* data);
        };
        GHL_STATIC_ASSERT(sizeof(data_holder)==(sizeof(void*)*3));
        
        template <class T>
        struct destructor_helper {
            static void raw( data_holder* d ) {
                meta::destructor_helper<T>::raw( d + 1 );
            }
            static void shared( data_holder* d ) {
                meta::destructor_helper<T>::shared( d + 1 );
            }
            static void intrusive( data_holder* d) {
                meta::destructor_helper<T>::intrusive( *reinterpret_cast<void**>( d + 1 ) );
            }
        };
        
        void lua_call_method(lua_State* L, int args, int res, const char* name);
        
        
        template <class T>
        inline T* get_ptr( const meta::type_info* from, void* data ) {
            const meta::type_info* to = meta::type<T>::info();
            while (true) {
                if ( from == to ) return static_cast<T*>(data);
                if (!from->parent.info) break;
                data = from->parent.downcast(data);
                from = from->parent.info;
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
            if (from->parent.info) {
                void (*destruct)(void*) = from->parent.downcast_shared( data, buf );
                if ( destruct ) {
                    sb::shared_ptr<T> res = get_shared_ptr<T>(from->parent.info, buf);
                    destruct(buf);
                    if ( res ) {
                        return res;
                    }
                }
            }
            return sb::shared_ptr<T>();
        }

        
        template <class T> 
        inline T* get_object_ptr( data_holder* holder) {
            T* res = 0;
            const meta::type_info* info = holder->info;
            if ( holder->type == data_holder::raw_data ) {
                res = get_ptr<T>(info, holder+1);
            } else if ( holder->type == data_holder::raw_ptr ) {
                res = get_ptr<T>(info, *reinterpret_cast<void**>(holder+1));
            } else if ( holder->type == data_holder::intrusive_ptr ) {
                res = get_ptr<T>(info, *reinterpret_cast<void**>(holder+1));
            } else if ( holder->type == data_holder::shared_ptr ) {
                sb::shared_ptr<T> ptr = get_shared_ptr<T>(info,holder+1);
                if (ptr)
                    res = ptr.get();
            } else {
                sb_assert(false);
            }
            return res;
        }
        

                
        template <class T,bool IsEnum> struct stack_help{
            static void push( lua_State* L, const T& val ) {
                {
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
            }
            static T& get_impl( lua_State* L, data_holder* holder,int idx ) {
                if ( holder->is_const ) {
                    lua_access_error( L, idx , holder->info->name );
                    return *reinterpret_cast<T*>(0);
                }
                T* res = get_object_ptr<T>(holder);
                if (!res) {
                    lua_argerror( L, idx, meta::type<T>::info()->name, holder->info->name );
                }
                return *res;
            }
            typedef T& get_type;
            static T& get( lua_State* L, int idx ) {
                if ( lua_isuserdata(L, idx) ) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, idx));
                    return get_impl(L, holder,idx);
                }
                lua_argerror( L, idx, meta::type<T>::info()->name, 0);
                return *reinterpret_cast<T*>(0);
            }
        };
        
        template <class T> struct stack_help<T,true> {
            static void push( lua_State* L, const T& val ) {
                lua_pushinteger(L, val);
            }
            typedef T get_type;
            static T get( lua_State* L, int idx ) {
                return static_cast<T>(lua_tointeger(L, idx));
            }
        };
        
        template <class T>
        struct stack {
            typedef stack_help<T, sb::is_enum<T>::result > help;
            static void push( lua_State* L, const T& val ) {
                help::push(L,val);
            }
            typedef typename help::get_type get_type;
            static get_type get( lua_State* L, int idx ) {
                return help::get(L,idx);
            }
        };
        template <class T>
        struct stack<const T> {
            static const T& get_impl( lua_State* L, data_holder* holder,int idx ) {
                T* res = get_object_ptr<T>(holder);
                if (!res) {
                    lua_argerror( L, idx, meta::type<T>::info()->name, holder->info->name );
                }
                return *res;
            }
            static const T& get( lua_State* L, int idx ) {
                if ( lua_isuserdata(L, idx) ) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, idx));
                    return get_impl(L, holder, idx);
                }
                lua_argerror( L, idx, meta::type<T>::info()->name, 0 );
                return *reinterpret_cast<T*>(0);
            }
        };
                
        template <class T>
        struct stack<T*> {
            static void push( lua_State* L, T* val ) {
                if ( val ) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_newuserdata(L, 
                                                                                         sizeof(data_holder)+
                                                                                         sizeof(T*)));
                    holder->type = data_holder::raw_ptr;
                    holder->is_const = false;
                    holder->info = meta::get_type_info(val);
                    holder->destructor = 0;
                    T** data = reinterpret_cast<T**>(holder+1);
                    *data = val;
                    lua_set_metatable( L, *holder );
                } else {
                    lua_pushnil(L);
                }
            }
            static T* get_impl( lua_State* L, data_holder* holder,int idx) {
                if ( holder->is_const ) {
                    lua_access_error( L, idx , holder->info->name );
                    return reinterpret_cast<T*>(0);
                } 
                T* res = get_object_ptr<T>(holder);
                if (!res) {
                    lua_argerror( L, idx, meta::type<T>::info()->name, holder->info->name );
                } 
                return res;
            }
            static T* get( lua_State* L, int idx , bool allow_nill = true ) {
                if ( lua_gettop(L)<idx ) {
                    if (allow_nill)
                        return reinterpret_cast<T*>(0);
                    lua_argerror( L, idx, meta::type<T>::info()->name, 0);
                }
                if ( lua_isuserdata(L, idx) ) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, idx));
                    return get_impl(L, holder, idx);
                } else if ( allow_nill && lua_isnil(L, idx) ) {
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
                T* res = get_object_ptr<T>(holder);
                if (!res) {
                    lua_argerror( L, idx, meta::type<T>::info()->name, holder->info->name );
                } 
                return res;
            }
            static const T* get( lua_State* L, int idx, bool allow_nil = true ) {
                if ( lua_gettop(L)<idx ) {
                    if (allow_nil)
                        return reinterpret_cast<T*>(0);
                    lua_argerror( L, idx, meta::type<T>::info()->name, 0);
                }
                if ( lua_isuserdata(L, idx) ) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, idx));
                    return get_impl(L, holder, idx);
                } else if ( allow_nil && lua_isnil(L, idx) ) {
                    return reinterpret_cast<T*>(0);
                }
                lua_argerror( L, idx, meta::type<T>::info()->name, 0);
                return reinterpret_cast<T*>(0);
            }
        };
        
        template <class T>
        static inline void stack_push_impl( lua_State* L, const sb::shared_ptr<T>& val  ) {
            if (!val) {
                lua_pushnil(L);
                return;
            }
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
        
        template <class T>
        static inline void stack_push_impl( lua_State* L, const sb::intrusive_ptr<T>& val  ) {
            if (!val) {
                lua_pushnil(L);
                return;
            }
            data_holder* holder = reinterpret_cast<data_holder*>(lua_newuserdata(L,
                                                                                 sizeof(data_holder)+
                                                                                 sizeof(T*)));
            holder->type = data_holder::intrusive_ptr;
            holder->is_const = false;
            holder->info = meta::get_top_type_info(val.get());
            holder->destructor = &destructor_helper<T>::intrusive;
            *reinterpret_cast<T**>(holder+1) = val.get();
            val->add_ref();
            lua_set_metatable( L, *holder );
        }
        
        template <class T>
        static sb::shared_ptr<T> stack_get_impl_help( lua_State* L,  data_holder* holder, int idx ) {
            if ( holder->type==data_holder::shared_ptr ) {
                sb::shared_ptr<T> ptr = get_shared_ptr<T>(holder->info,holder+1);
                return ptr;
            }
            lua_argerror( L, idx, meta::type<T>::info()->name, 0 );
            return sb::shared_ptr<T>();
        }
        
        template <class T>
        static inline sb::shared_ptr<T> stack_get_impl( lua_State* L, int idx  ) {
            if ( lua_isuserdata(L, idx) ) {
                data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, idx));
                return stack_get_impl_help<T>(L, holder, idx);
            } else if ( lua_isnil(L, idx) ) {
                return sb::shared_ptr<T>();
            }
            lua_argerror( L, idx, meta::type<T>::info()->name, 0 );
            return sb::shared_ptr<T>();

        }
        
        template <class T>
        struct stack<sb::shared_ptr<T> > {
            
            static void push( lua_State* L, const sb::shared_ptr<T>& val ) {
                stack_push_impl(L,val);
            }
            
            static sb::shared_ptr<T> get( lua_State* L, int idx ) {
                return stack_get_impl<T>(L,idx);
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
        static inline sb::intrusive_ptr<T> get_intrusive_ptr( lua_State* L, int idx ) {
            if ( lua_isuserdata(L, idx) ) {
                data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, idx));
                if ( holder->type==data_holder::intrusive_ptr ) {
                    T* ptr = get_ptr<T>(holder->info,*reinterpret_cast<void**>( holder+1));
                    return sb::intrusive_ptr<T>(ptr);
                } else if ( holder->type==data_holder::raw_ptr ) {
                    T* ptr = get_ptr<T>(holder->info, *reinterpret_cast<void**>(holder+1));
                    return sb::intrusive_ptr<T>(ptr);
                }
            } else if ( lua_isnil(L, idx) ) {
                return sb::intrusive_ptr<T>();
            }
            lua_argerror( L, idx, meta::type<T>::info()->name, 0 );
            return sb::intrusive_ptr<T>();
        }
        template <class T>
        struct stack<sb::intrusive_ptr<T> > {
            
            static void push( lua_State* L, const sb::intrusive_ptr<T>& val ) {
                stack_push_impl(L, val);
            }
            
            static sb::intrusive_ptr<T> get( lua_State* L, int idx ) {
                return get_intrusive_ptr<T>(L,idx);
            }
        };

        template <class T>
        struct stack<const sb::intrusive_ptr<T> > : stack<sb::intrusive_ptr<T> > {
        };
        template <class T>
        struct stack<const sb::intrusive_ptr<T>& > : stack<sb::intrusive_ptr<T> > {
        };
        
        template <class T>
        struct stack<sb::map<sb::string,T> > {
            static void push( lua_State* L, const sb::map<sb::string,T>& val ) {
                lua_createtable(L, 0, int(val.size()) );
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
            static void push( lua_State* L, const sb::vector<T>& val ) {
                lua_createtable(L, int(val.size()), 0 );
                lua_Integer idx = 1;
                for (typename sb::vector<T>::const_iterator it=val.begin();it!=val.end();++it) {
                    lua_pushinteger(L, idx);
                    stack<T>::push( L, *it );
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
            explicit lua_stack_check(lua_State* L,int delta) {
                m_L = L;
                m_top = lua_gettop(L) + delta;
            }
            ~lua_stack_check() {
                int top = lua_gettop(m_L);
                if (top != m_top) {
                    LogError() << "top is: " << top << " expected: " << m_top;
                }
                if (!std::uncaught_exception()) {
                    sb_assert( top == m_top );
                }
            }
            bool check_state() const {
                return lua_gettop(m_L) == m_top;
            }
        private:
            lua_State* m_L;
            int m_top;
        };
#define LUA_CHECK_STACK(d) ::Sandbox::luabind::lua_stack_check sc(L,d);
#else
#define LUA_CHECK_STACK(d)
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
        LUABIND_DECLARE_RAW_STACK_TYPE(double)
        LUABIND_DECLARE_RAW_STACK_TYPE(unsigned char)
        LUABIND_DECLARE_RAW_STACK_TYPE(unsigned long)
        LUABIND_DECLARE_RAW_STACK_TYPE(unsigned short)
        LUABIND_DECLARE_RAW_STACK_TYPE(unsigned int)
        LUABIND_DECLARE_RAW_STACK_TYPE(const char*)
        LUABIND_DECLARE_RAW_STACK_TYPE(long long)
        LUABIND_DECLARE_RAW_STACK_TYPE(unsigned long long)
        
        
        template <>
        struct stack<sb::string> {
            static void push( lua_State* L, const sb::string& val ) {
                lua_pushlstring(L, val.c_str(), val.size());
            }
            static sb::string get( lua_State* L, int idx) {
                const char* val = stack<const char*>::get(L,idx);
                return val ? val : "";
            }
        };
        
        template <>
        struct stack<const sb::string&> : stack<sb::string> {};
    
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
        struct stack<T&> {
            //            static void push( lua_State* L, const T& val ) {
            //                stack<T>::push(L,val);
            //            }
            static T& get( lua_State* L, int idx ) {
                return *stack<T*>::get(L,idx);
            }
        };

    }
}

#endif
