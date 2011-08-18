/*
 *  sb_bind_property.h
 *  SR
 *
 *  Created by Андрей Куницын on 11.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_BIND_PROPERTY_H
#define SB_BIND_PROPERTY_H

#include "sb_bind_stack.h"

namespace Sandbox {

	namespace Bind {
		
		typedef void (*PropertyFuncPtr)(void*,const void*,const StackHelper*); 
		struct PropertyData { char data[sizeof(void*)*2];} ;
		
		struct PropertyInfo {
			const char* name;
			const char* type;
			PropertyFuncPtr get;
			PropertyFuncPtr set;
			PropertyData data;
			PropertyData data_set;
		};
		
		
		
		
		
		template <class T,class P> inline PropertyData GetPropertyGetFuncData(P (T::*func)() const) {
			union InlUnion {
				P (T::*func)() const;
				PropertyData data;
			} inl_union;
			sb_assert( sizeof(func)<=sizeof( inl_union.data.data ) );
			inl_union.func = func;	
			return inl_union.data;
		}
		template <class T,class P> inline PropertyData GetPropertySetFuncData(void (T::*func)(P)) {
			union InlUnion {
				void (T::*func)(P);
				PropertyData data;
			} inl_union;
			sb_assert( sizeof(func)<=sizeof( inl_union.data.data ) );
			inl_union.func = func;	
			return inl_union.data;
		}
		template <class T,class P> inline PropertyData GetPropertyPtrData( P T::*prop ) {
			union InlUnion {
				P T::*prop;
				PropertyData data;
			} inl_union;
			sb_assert( sizeof(prop)<=sizeof( inl_union.data ) );
			inl_union.prop = prop;
			return inl_union.data;
		}
		
		template <typename Ptr> struct PropertyGetHelper;
		template <typename Ptr> struct PropertySetHelper;
		template <typename Ptr> inline PropertyFuncPtr GetPropertyGetFunc(Ptr) {
			return &PropertyGetHelper<Ptr>::Call;
		}
		template <typename Ptr> inline PropertyFuncPtr GetPropertySetFunc(Ptr) {
			return &PropertySetHelper<Ptr>::Call;
		}
		
#define SB_BIND_PROPERTY_RW(Class,Name,Get,Set,Type) { #Name,#Type"("#Type")", \
				Sandbox::Bind::GetPropertyGetFunc(&Class::Get), \
				Sandbox::Bind::GetPropertySetFunc(&Class::Set), \
				Sandbox::Bind::GetPropertyGetFuncData(&Class::Get), \
				Sandbox::Bind::GetPropertySetFuncData(&Class::Set), } ,
#define SB_BIND_PROPERTY_RO(Class,Name,Get,Type) { #Name,#Type, \
				Sandbox::Bind::GetPropertyGetFunc(&Class::Get), \
				0, \
				Sandbox::Bind::GetPropertyGetFuncData(&Class::Get), \
				{{}}, } ,	
#define SB_BIND_PROPERTY_RO_(Class,Name,Type,GetPtr) { #Name,#Type, \
				Sandbox::Bind::GetPropertyGetFunc(GetPtr), \
				0, \
				Sandbox::Bind::GetPropertyGetFuncData(GetPtr), \
				{{}}, } ,	
#define SB_BIND_PROPERTY_WO(Class,Name,Set,Type) { #Name,"("#Type")", \
				0, \
				Sandbox::Bind::GetPropertySetFunc(&Class::Set), \
				{{}}, \
				Sandbox::Bind::GetPropertySetFuncData(&Class::Set), } ,	
#define SB_BIND_PROPERTY_RAW(Class,Name,Type) { #Name,#Type"("#Type")", \
				Sandbox::Bind::GetPropertyGetFunc(&Class::Name), \
				Sandbox::Bind::GetPropertySetFunc(&Class::Name), \
				Sandbox::Bind::GetPropertyPtrData(&Class::Name), \
				Sandbox::Bind::GetPropertyPtrData(&Class::Name), } ,
/*#define SB_BIND_PROPERTY_RAW_(Class,Name,Field,Type) { #Name,#Type"("#Type")", \
				Sandbox::Bind::GetPropertyGetFunc(&(Class::Field)), \
				Sandbox::Bind::GetPropertySetFunc(&(Class::Field)), \
				Sandbox::Bind::GetPropertyPtrData(&(Class::Field)), \
				Sandbox::Bind::GetPropertyPtrData(&(Class::Field)), } ,*/
#define SB_BIND_PROPERTY_RAW_RO(Class,Name,Type) { #Name,#Type"("#Type")", \
				Sandbox::Bind::GetPropertyGetFunc(&Class::Name), \
				0, \
				Sandbox::Bind::GetPropertyPtrData(&Class::Name), \
				{{}}, } ,
#define SB_BIND_ZERO_PROPERTY { 0,0,0,0,{{}},{{}} },

/// gcc bug #21853 workaround
#if 1
#define SB_CAST_VOID_TO_MEMBER_PTR(Type,ptr) *static_cast<Ptr*>(const_cast<void*>(ptr))
#else
#define SB_CAST_VOID_TO_MEMBER_PTR(Type,ptr) *static_cast<const Ptr*>(ptr)
#endif

		template <typename T,typename Prop> struct PropertyGetHelper<Prop(T::*)()const> {
			typedef Prop(T::*FuncPtr)()const;
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				hpr->PushValue((static_cast<T*>(obj)->*func)());
			}
		};
		template <typename T,typename Prop> struct PropertyGetHelper<Prop T::*> {
			typedef Prop T::*Ptr;
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				Ptr ptr = SB_CAST_VOID_TO_MEMBER_PTR(Ptr,funcp);
				hpr->PushValue(static_cast<T*>(obj)->*ptr);
			}
		};
		template <typename T,typename Prop> struct PropertySetHelper<void(T::*)(Prop)> {
			typedef void(T::*FuncPtr)(Prop);
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Prop>()));
			}
		};
		template <typename T,typename Prop> struct PropertySetHelper<Prop T::*> {
			typedef Prop T::*Ptr;
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				Ptr ptr = SB_CAST_VOID_TO_MEMBER_PTR(Ptr,funcp);
				(static_cast<T*>(obj)->*ptr) = hpr->GetArgument(0,ArgumentTag<Prop>());
			}
		};
	
		
	};
}

#endif /*SB_BIND_PROPERTY_H*/
