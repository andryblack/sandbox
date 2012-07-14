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
                shared_ptr
            } type;
            bool    is_const;
            void    (*destructor)(void* data);
        };
        
        void lua_set_metatable( lua_State* L, const data_holder& holder );
        void lua_set_value( lua_State* L, const char* path );
        void lua_get_create_table(lua_State* L,const char* name);
        void lua_create_metatable(lua_State* L,const meta::type_info* info);
        
        template <class T>
        inline T* get_ptr( const meta::type_info* from, void* data ) {
            const meta::type_info* to = meta::type<T>::info();
            do {
                if ( from == to ) return reinterpret_cast<T*>(data);
                data = from->downcast( data );
                from = from->parent;
            } while (from && to);
            return 0;
        }
        template <class T>
        inline sb::shared_ptr<T> get_shared_ptr( const meta::type_info* from, void* data ) {
            const meta::type_info* to = meta::type<T>::info();
            char buf1[sizeof(sb::shared_ptr<T>)];
            void (*destruct)(void*) = 0;
            char buf2[sizeof(sb::shared_ptr<T>)];
            void* p1 = data;
            void* p2 = buf1;
            do {
                if ( from == to ) {
                    sb::shared_ptr<T> res = *reinterpret_cast<sb::shared_ptr<T>*>(p1);
                    if ( destruct ) {
                        destruct( p1 );
                    }
                    return res;
                }
                void (*destruct1)(void*) = from->downcast_shared( p1, p2 );
                if ( destruct ) {
                    destruct( p1 );
                }
                destruct = destruct1;
                p1 = p2;
                if (p1 == buf1) {
                    p2 = buf2;
                } else {
                    p2 = buf1;
                }
                from = from->parent;
            } while (from && to);
            return sb::shared_ptr<T>();
        }

        
        template <class T> 
        inline T* get_object_ptr( data_holder* holder ) {
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
            } else {
                sb_assert(false);
            }
            return res;
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
                holder->destructor = &meta::destructor_helper<T>::raw;
                void* data = holder+1;
                new (data) T(val);
                lua_set_metatable( L, *holder );
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
                T* res = get_object_ptr<T>(holder);
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
                if ( val ) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_newuserdata(L, 
                                                                                         sizeof(data_holder)+
                                                                                         sizeof(sb::shared_ptr<T>)));
                    holder->type = data_holder::shared_ptr;
                    holder->is_const = false;
                    holder->info = meta::type<T>::info();
                    holder->destructor = &meta::destructor_helper<T>::shared;
                    new (holder+1) sb::shared_ptr<T>(val);
                    lua_set_metatable( L, *holder );
                } else {
                    lua_pushnil(L);
                }
            }
            static sb::shared_ptr<T> get_impl( lua_State* L,  data_holder* holder, int idx ) {
                if ( holder->type==data_holder::shared_ptr ) {
                    sb::shared_ptr<T> ptr = get_shared_ptr<T>(holder->info,holder+1);
                    if ( ptr ) {
                        return ptr;
                    }
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
        
        
    
        class stack_cleaner : public NotCopyable {
        public:
            explicit stack_cleaner( lua_State* L );
            ~stack_cleaner();
        private:
            lua_State* m_L;
            int     m_top;
        };
    }
}

#endif
