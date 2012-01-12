/*
 *  sb_bind_class.h
 *  SR
 *
 *  Created by Андрей Куницын on 09.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_BIND_CLASS_H
#define SB_BIND_CLASS_H

#include "sb_shared_ptr.h"
#include "sb_bind_method.h"
#include "sb_bind_property.h"
#include "sb_bind_enum.h"

struct lua_State;

namespace Sandbox {
	namespace Bind {

		template <class T>
		struct ClassDestructHelper {
			static void ptr(void* ptr,lua_State*) {
				shared_ptr<T> p = *static_cast<shared_ptr<T>*>(ptr);
				static_cast<shared_ptr<T>*>(ptr)->~shared_ptr<T>();
			}
			static void raw(void* ptr,lua_State*) {
				static_cast<T*>(ptr)->~T();
			}
		};
		
		struct ClassInfo {
			const char* name;
			const char* parent;
			void (*destruct_ptr)(void*,lua_State*);
			void (*destruct_raw)(void*,lua_State*);
			void* (*get_ptr)(void*);
			void (*copy_ptr)(void*,void*);
			void (*cast_ptr)(void*,void*);
            size_t obj_size;
			size_t ptr_size;
		};
		template <class T>
		struct SharedPtrHelper {
			static void *get_ptr(void* ptr) {
				return static_cast<shared_ptr<T>*> (ptr)->get();
			}
			static void copy_ptr(void* from,void* to) {
				*static_cast<shared_ptr<T>*>(to) = *static_cast<shared_ptr<T>*>(from);
			}
			template <class U>
			static void cast_ptr(void* from,void* to) {
				shared_ptr<U> x = static_pointer_cast<U>(*static_cast<shared_ptr<T>*>(from));
				new (to) shared_ptr<U>(x);
			}
		};
        
        
		
#define SB_BIND_CLASS_SHARED(Name) { \
			#Name, \
			0, \
			&Sandbox::Bind::ClassDestructHelper<Name>::ptr, \
			0, \
			&Sandbox::Bind::SharedPtrHelper<Name>::get_ptr, \
			&Sandbox::Bind::SharedPtrHelper<Name>::copy_ptr, \
			0, \
			0, \
			sizeof(Sandbox::shared_ptr<Name>) \
		}
#define SB_BIND_SUBCLASS_SHARED(Name,Parent) { \
			#Name, \
			#Parent, \
			&Sandbox::Bind::ClassDestructHelper<Name>::ptr, \
			0, \
			&Sandbox::Bind::SharedPtrHelper<Name>::get_ptr, \
			&Sandbox::Bind::SharedPtrHelper<Name>::copy_ptr, \
			&Sandbox::Bind::SharedPtrHelper<Name>::cast_ptr<Parent>, \
			0, \
			sizeof(Sandbox::shared_ptr<Name>) \
		}		
#define SB_BIND_CLASS_RAW(Name) { \
			#Name, \
			0, \
			0, \
			&Sandbox::Bind::ClassDestructHelper<Name>::raw, \
			0,0,0, \
			sizeof(Name), \
			0 \
		}
#define SB_BIND_CLASS_EXTERN(Name) { \
			#Name, \
			0, \
			0, \
			0, \
			0,0,0, \
			0, \
			0 \
		}
#define SB_BIND_SUBCLASS_EXTERN(Name,Parent) { \
			#Name, \
			#Parent, \
			0, \
			0, \
			0,0,0, \
			0, \
			0 \
		}
		
		struct ConstructorInfo {
			const char* signature;
			void (*raw)(const StackHelper*);
			void (*ptr)(const StackHelper*);
		};
        template <typename Signature>
        struct ClassConstructHelper {
            typedef typename MethodHelper<Signature>::ObjectType ObjectType;
            static void ConstructRaw(const StackHelper* hpr) {
                MethodHelper<Signature>::ConstructInplace(hpr->new_object_raw(),hpr);
            }
            static void ConstructPtr(const StackHelper* hpr) {
                typename MethodHelper<Signature>::ObjectType* raw = MethodHelper<Signature>::Construct( hpr );
                new (hpr->new_object_shared_ptr() ) shared_ptr<ObjectType>( raw );
            }
        };
		
		struct ClassBind {
			const ClassInfo* info;
			const ConstructorInfo* constructor;
			const MethodInfo* methods;
			const PropertyInfo* propertys;
		};
        void register_type(lua_State* L,const ClassBind* bind);
        
        struct ExtensibleClassBind {
            const ClassInfo* info;
			const ConstructorInfo* constructor;
            typedef void (*update_reference_func)( void* obj, lua_State* L);
            update_reference_func   update_ref_func;
        };
        void register_extensible_type(lua_State* L,const ExtensibleClassBind* bind);
        int register_extensible_reference(lua_State* L, int ref, const char* name);
        bool get_extensible_method_by_reference(lua_State* L, int ref, const char* name, const char* method);
		
		namespace ClassDefaults {
			static const ConstructorInfo constructorInfo = { 
				0,0, 0 
			};
			static const MethodInfo methods[] = {SB_BIND_ZERO_METHOD};
			static const PropertyInfo propertys[] = {SB_BIND_ZERO_PROPERTY};
		}

		
#define SB_BIND_BEGIN_RAW_CLASS( Name ) static const Sandbox::Bind::ClassInfo classInfo = SB_BIND_CLASS_RAW(Name);
#define SB_BIND_BEGIN_EXTERN_CLASS( Name ) static const Sandbox::Bind::ClassInfo classInfo = SB_BIND_CLASS_EXTERN(Name);
#define SB_BIND_BEGIN_EXTERN_SUBCLASS( Name , Parent ) static const Sandbox::Bind::ClassInfo classInfo = SB_BIND_SUBCLASS_EXTERN(Name,Parent);
#define SB_BIND_BEGIN_SHARED_CLASS( Name ) static const Sandbox::Bind::ClassInfo classInfo = SB_BIND_CLASS_SHARED(Name);
#define SB_BIND_BEGIN_SHARED_SUBCLASS( Name , Parent ) static const Sandbox::Bind::ClassInfo classInfo = SB_BIND_SUBCLASS_SHARED(Name,Parent);
		
#define SB_BIND_RAW_CONSTRUCTOR( Name , Signature ) static const Sandbox::Bind::ConstructorInfo constructorInfo = { \
			#Name#Signature,&Sandbox::Bind::ClassConstructHelper<void(Name::*)Signature>::ConstructRaw, 0 \
		};
#define SB_BIND_SHARED_CONSTRUCTOR( Name , Signature ) static const Sandbox::Bind::ConstructorInfo constructorInfo = { \
			#Name#Signature,0, &Sandbox::Bind::ClassConstructHelper<void(Name::*)Signature>::ConstructPtr \
		};
#define SB_BIND_SHARED_CONSTRUCTOR_( Name , Signature , RealSignature ) static const Sandbox::Bind::ConstructorInfo constructorInfo = { \
			#Name#Signature,0, &Sandbox::Bind::ClassConstructHelper<void(Name::*)RealSignature>::ConstructPtr \
		};
#define SB_BIND_BEGIN_METHODS static const Sandbox::Bind::MethodInfo methods[] = {
#define SB_BIND_END_METHODS SB_BIND_ZERO_METHOD };
#define SB_BIND_BEGIN_PROPERTYS static const Sandbox::Bind::PropertyInfo propertys[] = {
#define SB_BIND_END_PROPERTYS SB_BIND_ZERO_PROPERTY };

#define SB_BIND_END_CLASS static const Sandbox::Bind::ClassBind bind = { &classInfo,&constructorInfo, methods,propertys }; 
		
        
#define SB_BIND_BEGIN_EXTENSIBLE( Name ) class Name##Proxy : public Name { \
        public: \
            Name##Proxy () : Name(  ) , m_state(0), m_ref(0) {}   /* only empty crts */ \
            void update_reference( lua_State* state ) { \
                m_state = state; \
                m_ref = Sandbox::Bind::register_extensible_reference( m_state,m_ref, #Name"Proxy"); \
            } \
            static void update_reference_s( void* obj, lua_State* L) {\
                reinterpret_cast<Name##Proxy*>(obj)->update_reference(L); \
            }\
        protected: \
        static void Constructor(const Sandbox::Bind::StackHelper* hpr) { \
            Name##Proxy* raw = new Name##Proxy(); \
            new (hpr->new_object_shared_ptr()) Sandbox::shared_ptr<Name##Proxy>(raw); \
        } \
        static const Sandbox::Bind::ConstructorInfo* GetConstructorInfo() { \
            static const Sandbox::Bind::ConstructorInfo constructorInfo = { \
            #Name"Proxy()",0, &Name##Proxy::Constructor \
            }; \
            return &constructorInfo; \
        }
#define SB_BIND_EXTENSIBLE_METHOD_VOID_A1(Class,Method,Signature,ArgsSignature,a1 ) \
        void Method ArgsSignature { \
            if (Sandbox::Bind::get_extensible_method_by_reference(m_state,m_ref,#Class"Proxy",#Method)) {\
                Sandbox::Bind::StackHelper hpr(m_state,1,#Signature);\
                Sandbox::Bind::MethodHelper<void(Class##Proxy::*)ArgsSignature>::Call(&hpr,1,a1); \
            } \
        }
#define SB_BIND_EXTENSIBLE_METHOD_VOID_A2(Class,Method,Signature,ArgsSignature,a1,a2 ) \
        void Method ArgsSignature { \
            if (Sandbox::Bind::get_extensible_method_by_reference(m_state,m_ref,#Class"Proxy",#Method)) {\
                Sandbox::Bind::StackHelper hpr(m_state,1,#Signature);\
                Sandbox::Bind::MethodHelper<void(Class##Proxy::*)ArgsSignature>::Call(&hpr,1,a1,a2); \
            } \
        }
#define SB_BIND_EXTENSIBLE_METHOD_VOID_A3(Class,Method,Signature,ArgsSignature,a1,a2,a3 ) \
        void Method ArgsSignature { \
            if (Sandbox::Bind::get_extensible_method_by_reference(m_state,m_ref,#Class"Proxy",#Method)) {\
                Sandbox::Bind::StackHelper hpr(m_state,1,#Signature);\
                Sandbox::Bind::MethodHelper<void(Class##Proxy::*)ArgsSignature>::Call(&hpr,1,a1,a2,a3); \
            } \
        }
#define SB_BIND_EXTENSIBLE_METHOD_VOID_NA(Class,Method,Signature) \
        void Method () { \
            if (Sandbox::Bind::get_extensible_method_by_reference(m_state,m_ref,#Class"Proxy",#Method)) {\
                Sandbox::Bind::StackHelper hpr(m_state,1,#Signature);\
                Sandbox::Bind::MethodHelper<void(Class##Proxy::*)()>::Call(&hpr,1); \
            } \
        }
#define SB_BIND_END_EXTENSIBLE( Name ) \
        public: \
            static void Register( Sandbox::Lua* lua ) { \
                static const Sandbox::Bind::ClassInfo class_info = SB_BIND_SUBCLASS_SHARED( Name##Proxy, Name); \
                static const Sandbox::Bind::ExtensibleClassBind info = { \
                    &class_info, GetConstructorInfo(), &Name##Proxy::update_reference_s }; \
                lua->Bind( &info ); \
            } \
        private: \
            lua_State*  m_state;\
            int m_ref;\
        }; 

         
#define SB_BIND_BIND_EXTENSIBLE( Name , luaPtr) \
        Name##Proxy::Register( luaPtr );
		
		enum StoreType {
			STORE_RAW,
			STORE_RAW_PTR,
			STORE_SHARED,
		};
		
		struct ObjectData {
			StoreType store_type;
		};
		
	}
}

#endif /*SB_BIND_CLASS_H*/
