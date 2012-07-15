//
//  sb_meta.h
//  YinYang
//
//  Created by Андрей Куницын on 6/5/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_meta_h
#define YinYang_sb_meta_h

#include <cstring>
#include "../sb_shared_ptr.h"

namespace Sandbox {
     
    namespace meta {
        
        struct type_info;
        
        struct type_info_parent {
            const type_info* const info;
            void* (*downcast)(void*);
            typedef void(*shared_destruct)(void*);
            shared_destruct (*downcast_shared)(void*,void*);
        };
        
        struct type_info {
            const char* const name;
            size_t size;
            const type_info_parent const* parents;
        };
        
        class object {
        public:
            virtual ~object() {}
            virtual const type_info* get_type_info() const {
                return get_static_type_info();
            }
            static const type_info* get_static_type_info();
        };
#define SB_META_OBJECT \
    public: \
        virtual const Sandbox::meta::type_info* get_type_info() const { \
            return get_static_type_info(); \
        } \
        static const Sandbox::meta::type_info* get_static_type_info();\
    private: \
       
        template <class T> struct type {
            static const type_info* info();
        };
        
        template <class T> struct type<const T> {
            static const type_info* info() { 
                return type<T>::info();
            }
        };
        template <class T> struct type<T&> {
            static const type_info* info() { 
                return type<T>::info();
            }
        };
        template <class T> struct type<T*> {
            static const type_info* info() { 
                return type<T>::info();
            }
        };
        template <class T> struct type<sb::shared_ptr<T> > {
            static const type_info* info() { 
                return type<T>::info();
            }
        };
        
                
        inline bool is_convertible( const type_info* from, const type_info* to ) {
            do {
                if ( from == to ) return true;
                from = from->parents[0].info;
            } while (from && to);
            return false;
        }
        
                
        template <class T>
        struct destructor_helper {
            static void raw( void* data ) { reinterpret_cast<T*>(data)->~T(); }
            static void shared( void* data ) { reinterpret_cast<sb::shared_ptr<T>*>(data)->~shared_ptr<T>(); }
        };
        
        template <class Klass, class Parent> struct cast_helper {
            static void* raw( void* ptr ) {
                return static_cast<Parent*>(reinterpret_cast<Klass*>(ptr));
            }
            static type_info_parent::shared_destruct shared( void* ptr1, void* ptr2 ) {
                new (ptr2) sb::shared_ptr<Parent>( *reinterpret_cast<sb::shared_ptr<Klass>*>(ptr1) );
                return &destructor_helper<Parent>::shared;
            }
        };
        template <class Klass> struct cast_helper<Klass,void> {
            static void* raw( void* ptr ) {
                return static_cast<void*>(reinterpret_cast<Klass*>(ptr));
            }
            static type_info_parent::shared_destruct shared( void* , void*  ) {
                return 0;
            }
        };
#define SB_META_DECLARE_KLASS(Klass,Parent)  \
        namespace Sandbox { namespace meta { \
            template <> const type_info* type<Klass>::info() {\
                static const type_info_parent parents[] = { \
                    { \
                        type<Parent>::info(), \
                        &cast_helper<Klass,Parent>::raw, \
                        &cast_helper<Klass,Parent>::shared \
                    }, \
                    { 0, 0, 0 } \
                };\
                static const type_info ti = { \
                    #Klass, \
                    sizeof(Klass), \
                    parents \
                }; \
                return &ti; \
            } \
        }}

#define SB_META_DECLARE_OBJECT(Klass,Parent)  \
        SB_META_DECLARE_KLASS(Klass,Parent) \
        const Sandbox::meta::type_info* Klass::get_static_type_info() {\
            return Sandbox::meta::type<Klass>::info(); \
        }

#define SB_META_DECLARE_POD_TYPE(Type) SB_META_DECLARE_KLASS(Type,void)

        
    }
        
    
    
}

#endif
