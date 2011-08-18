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
			const size_t obj_size;
			const size_t ptr_size;
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
		
		struct ConstructorInfo {
			const char* signature;
			void (*raw)(const StackHelper*);
			void (*ptr)(const StackHelper*);
		};
		
		struct ClassBind {
			const ClassInfo* info;
			const ConstructorInfo* constructor;
			const MethodInfo* methods;
			const PropertyInfo* propertys;
		};
		
		
		
#define SB_BIND_BEGIN_RAW_CLASS( Name ) static const Sandbox::Bind::ClassInfo classInfo = SB_BIND_CLASS_RAW(Name);
#define SB_BIND_BEGIN_EXTERN_CLASS( Name ) static const Sandbox::Bind::ClassInfo classInfo = SB_BIND_CLASS_EXTERN(Name);
#define SB_BIND_BEGIN_SHARED_CLASS( Name ) static const Sandbox::Bind::ClassInfo classInfo = SB_BIND_CLASS_SHARED(Name);
#define SB_BIND_BEGIN_SHARED_SUBCLASS( Name , Parent ) static const Sandbox::Bind::ClassInfo classInfo = SB_BIND_SUBCLASS_SHARED(Name,Parent);
		
#define SB_BIND_RAW_CONSTRUCTOR( Name , Signature ) static const Sandbox::Bind::ConstructorInfo constructorInfo = { \
			#Name#Signature,&Sandbox::Bind::MethodHelper<void(Name::*)Signature>::ConstructInplace, 0 \
		};
#define SB_BIND_SHARED_CONSTRUCTOR( Name , Signature ) static const Sandbox::Bind::ConstructorInfo constructorInfo = { \
			#Name#Signature,0, &Sandbox::Bind::MethodHelper<void(Name::*)Signature>::ConstructInPtr \
		};
#define SB_BIND_SHARED_CONSTRUCTOR_( Name , Signature , RealSignature ) static const Sandbox::Bind::ConstructorInfo constructorInfo = { \
			#Name#Signature,0, &Sandbox::Bind::MethodHelper<void(Name::*)RealSignature>::ConstructInPtr \
		};
#define SB_BIND_NO_CONSTRUCTOR static const Sandbox::Bind::ConstructorInfo constructorInfo = { \
			0,0, 0 \
		};
#define SB_BIND_BEGIN_METHODS static const Sandbox::Bind::MethodInfo methods[] = {
#define SB_BIND_END_METHODS SB_BIND_ZERO_METHOD };
#define SB_BIND_NO_METHODS	static const Sandbox::Bind::MethodInfo* methods = 0;		
#define SB_BIND_BEGIN_PROPERTYS static const Sandbox::Bind::PropertyInfo propertys[] = {
#define SB_BIND_END_PROPERTYS SB_BIND_ZERO_PROPERTY };
#define SB_BIND_NO_PROPERTYS	static const Sandbox::Bind::PropertyInfo* propertys = 0;

#define SB_BIND_END_CLASS static const Sandbox::Bind::ClassBind bind = { &classInfo,&constructorInfo, methods,propertys };
		
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
