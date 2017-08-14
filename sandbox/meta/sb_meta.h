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
#include <new>
#include <sbstd/sb_intrusive_ptr.h>
#include <sbstd/sb_shared_ptr.h>
#include <sbstd/sb_platform.h>

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
            type_info_parent parent;
        };
        
        class object : public sb::ref_countered_base_not_copyable {
        private:
#ifdef SB_DEBUG
            static  size_t  m_count;
#endif
        public:
            static size_t count() {
#ifdef SB_DEBUG
                return m_count;
#else
                return 0;
#endif
            }
            virtual ~object() {
 #ifdef SB_DEBUG
                --m_count;
#endif
            }
            virtual const type_info* get_type_info() const {
                return get_static_type_info();
            }
            static const type_info* get_static_type_info();
        protected:
            object() : sb::ref_countered_base_not_copyable() {
#ifdef SB_DEBUG
                ++m_count;
#endif
            }
        };
        
        template <class T>
        class object_proxy : public T {
        public:
            virtual const type_info* get_type_info() const {
                return object::get_static_type_info();
            }
            static const type_info* get_static_type_info() { return object::get_static_type_info(); }
        };
        
#define SB_META_OBJECT_BASE \
    public: \
        virtual const Sandbox::meta::type_info* get_type_info() const;\
        static const Sandbox::meta::type_info* get_static_type_info();\
    private: \

#define SB_META_OBJECT \
    public: \
        virtual const Sandbox::meta::type_info* get_type_info() const SB_OVERRIDE;\
        static const Sandbox::meta::type_info* get_static_type_info();\
    private: \
     
    

        template <class T> struct type {
            static const type_info* private_info;
            static inline const type_info* info() { return private_info; }
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
        template <class T> struct type<sb::intrusive_ptr<T> > {
            static const type_info* info() {
                return type<T>::info();
            }
        };
        
#define SB_META_PRIVATE_CLASS(Type) \
        namespace Sandbox { namespace meta { \
            template <> struct type<Type> { \
                typedef void* info; \
            }; \
        } }
        
        inline bool is_convertible( const type_info* from, const type_info* to ) {
            do {
                if ( from == to ) return true;
                from = from->parent.info;
            } while (from && to);
            return false;
        }
        
                
        template <class T>
        struct destructor_helper {
            static void raw( void* data ) { reinterpret_cast<T*>(data)->~T(); }
            static void shared( void* data ) {
                typedef sb::shared_ptr<T> ptr_type;
                reinterpret_cast<ptr_type*>(data)->~ptr_type();
            }
            static void intrusive( void* data ) {
                reinterpret_cast<T*>(data)->remove_ref();
            }
        };
        
        template <class Klass, class Parent> struct cast_helper {
            static void* raw( void* ptr ) {
                return static_cast<Parent*>(static_cast<Klass*>(ptr));
            }
            static type_info_parent::shared_destruct shared( void* storage_ptr1, void* storage_ptr2 ) {
                typedef sb::shared_ptr<Klass> klass_ptr;
				klass_ptr temp_ptr = *reinterpret_cast<klass_ptr*>(storage_ptr1);
				typedef sb::shared_ptr<Parent> parent_ptr;
                new(storage_ptr2) parent_ptr(temp_ptr);
                return &destructor_helper<Parent>::shared;
            }
        };
        template <class Klass> struct cast_helper<Klass,void> {
            static void* raw( void* ptr ) {
                return static_cast<void*>(static_cast<Klass*>(ptr));
            }
            static type_info_parent::shared_destruct shared( void* , void*  ) {
                return 0;
            }
        };
#define SB_META_DECLARE_KLASS_X(Klass,Parent,Line,KlassName)  \
        namespace Sandbox { namespace meta { \
            namespace Line{ \
                static const type_info ti = { \
                    KlassName, \
                    sizeof(Klass), \
                    { \
                        type<Parent>::private_info, \
                        &cast_helper<Klass,Parent>::raw, \
                        &cast_helper<Klass,Parent>::shared \
                    } \
                }; \
            } \
            template <> const type_info* type<Klass>::private_info = &Line::ti; \
        }}


#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, CONCATENATE(__LINE__,__COUNTER__))
#define SB_META_DECLARE_KLASS(Type,Parent) SB_META_DECLARE_KLASS_X(Type,Parent,ANONYMOUS_VARIABLE(private_),#Type)

#define SB_META_DECLARE_NAMED_KLASS(Type,Name,Parent) SB_META_DECLARE_KLASS_X(Type,Parent,ANONYMOUS_VARIABLE(private_),Name)
#define SB_META_DECLARE_OBJECT_IMPL(Klass,Prefix)\
    Prefix\
    const Sandbox::meta::type_info* Klass::get_static_type_info() {\
        return Sandbox::meta::type<Klass>::info(); \
    }\
    Prefix\
    const Sandbox::meta::type_info* Klass::get_type_info() const { \
        return get_static_type_info(); \
    }
        
#define SB_META_DECLARE_OBJECT(Klass,Parent)  \
        SB_META_DECLARE_KLASS(Klass,Parent) \
        SB_META_DECLARE_OBJECT_IMPL(Klass,)

#define SB_META_DECLARE_OBJECT_T(NS,Klass,Parent)  \
    SB_META_DECLARE_NAMED_KLASS(NS::Klass,#NS"::"#Klass,Parent) \
    namespace NS { SB_META_DECLARE_OBJECT_IMPL(Klass,template<>) }

#define SB_META_DECLARE_OBJECT2(Klass,Parent1,Parent2)  \
    SB_META_DECLARE_KLASS2(Klass,Parent1,Parent2) \
    SB_META_DECLARE_OBJECT_IMPL(Klass,)


        
#define SB_META_DECLARE_POD_TYPE(Type) SB_META_DECLARE_KLASS(Type,void)

        
        namespace implementation {
            template <class Type>
            class has_get_type_info
            {
                class yes { char m;};
                class no { yes m[2];};

                struct BaseMixin
                {
                    const type_info* get_type_info() const { return 0;}
                };
                struct Base : public Type, public BaseMixin {};
                template <typename T, T t>  class Helper{};
                template <typename U>
                static no deduce(U*, Helper<const type_info* (BaseMixin::*)()const, &U::get_type_info>* = 0);
                static yes deduce(...);

            public:
                static const bool result = sizeof(yes) == sizeof(deduce((Base*)(0)));

            };

            template<class T,bool> struct get_type_info {
                static const type_info* get(const T*) {
                    return type<T>::info();
                }
            };
            template<class T> struct get_type_info<T,true> {
                static const type_info* get(const T* v) {
                    return v->get_type_info();
                }
            };
        }

        template <class T,class U>
        inline T* sb_dynamic_cast(U* o) {
            const type_info* rt = T::get_static_type_info();
            const type_info* ti = o->get_type_info();
            void* vo = o;
            while (ti) {
                if (ti == rt) return static_cast<T*>(vo);
                if (!ti->parent.info)
                    return 0;
                vo = ti->parent.downcast(vo);
                ti = ti->parent.info;               
            }
            return 0;
        }
        
        template <class T,class U>
        inline const T* sb_dynamic_cast(const U* o) {
            const type_info* rt = T::get_static_type_info();
            const type_info* ti = o->get_type_info();
            void* vo = const_cast<U*>(o);
            while (ti) {
                if (ti == rt) return static_cast<const T*>(vo);
                if (!ti->parent.info)
                    return 0;
                vo = ti->parent.downcast(vo);
                ti = ti->parent.info;
                
            }
            return 0;
        }


        template <class T>
        inline const type_info* get_type_info(const T* v) {
            return implementation::get_type_info<T,implementation::has_get_type_info<T>::result>::get(v);
        }
        
        template <class T>
        inline const type_info* get_top_type_info(const T* v) {
            const type_info* dyn = implementation::get_type_info<T,implementation::has_get_type_info<T>::result>::get(v);
            const type_info* stat = type<T>::info();
            while (stat) {
                if (stat == dyn) {
                    return type<T>::info();
                }
                stat = stat->parent.info;
            }
            return dyn;
        }
    }
    
}

#endif
