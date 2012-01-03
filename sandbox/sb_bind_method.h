/*
 *  sb_bind_method.h
 *  SR
 *
 *  Created by Андрей Куницын on 09.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *ArgumentTag
 */
#ifndef SB_BIND_METHOD_H
#define SB_BIND_METHOD_H

#include "sb_shared_ptr.h"
#include "sb_assert.h"

#include "sb_bind_stack.h"

namespace Sandbox {
	
	namespace Bind {
		
		struct MethodPtrData { char data[sizeof(void*)*2]; };
		class StackHelper;
		
		typedef void (*CallMethodFuncPtr)(void*,const void*,const StackHelper*); 
		
		struct MethodInfo {
			const char* name;
			const char* signature;
			bool	is_const;
			int		ret;
			CallMethodFuncPtr call;
			MethodPtrData func;
		};
		
		
	
		
		
		
		
		template <typename FuncPtr> struct MethodHelper;
		
		template <typename FuncPtr> inline CallMethodFuncPtr GetCallMethodFunc(FuncPtr) {
			return &MethodHelper<FuncPtr>::Call;
		}
		template <typename FuncPtr> inline bool GetMethodIsConst(FuncPtr) {
			return MethodHelper<FuncPtr>::isConst;
		}
		template <typename FuncPtr> inline int GetMethodRet(FuncPtr) {
			return MethodHelper<FuncPtr>::retValues;
		}
		
		template <typename FuncPtr> inline MethodPtrData WriteMethodFunc(FuncPtr func) {
			MethodPtrData data;
			sb_assert( sizeof(func)<=sizeof( data.data ) );
			*reinterpret_cast<FuncPtr*>(data.data) = func;		  
			return data;
		}

#define SB_BIND_METHOD( Class, Method , Signature ) { \
			#Method, \
			#Signature, \
			Sandbox::Bind::GetMethodIsConst(&Class::Method), \
			Sandbox::Bind::GetMethodRet(&Class::Method), \
			Sandbox::Bind::GetCallMethodFunc(&Class::Method), \
			Sandbox::Bind::WriteMethodFunc(&Class::Method) \
		},
#define SB_BIND_METHOD_( Class, Method , Signature ,MethodPtr) { \
			#Method, \
			#Signature, \
			Sandbox::Bind::GetMethodIsConst(MethodPtr), \
			Sandbox::Bind::GetMethodRet(MethodPtr), \
			Sandbox::Bind::GetCallMethodFunc(MethodPtr), \
			Sandbox::Bind::WriteMethodFunc(MethodPtr) \
		},
#define SB_BIND_OPERATOR_ADD_( Class,Signature,FullSignature ) { \
			"__add", \
			#Signature, \
			Sandbox::Bind::GetMethodIsConst((FullSignature)(&Class::operator+)), \
			Sandbox::Bind::GetMethodRet((FullSignature)(&Class::operator+)), \
			Sandbox::Bind::GetCallMethodFunc((FullSignature)(&Class::operator+)), \
			Sandbox::Bind::WriteMethodFunc((FullSignature)(&Class::operator+)) \
		},
#define SB_BIND_OPERATOR_SUB_( Class,Signature,FullSignature ) { \
			"__sub", \
			#Signature, \
			Sandbox::Bind::GetMethodIsConst((FullSignature)(&Class::operator-)), \
			Sandbox::Bind::GetMethodRet((FullSignature)(&Class::operator-)), \
			Sandbox::Bind::GetCallMethodFunc((FullSignature)(&Class::operator-)), \
			Sandbox::Bind::WriteMethodFunc((FullSignature)(&Class::operator-)) \
},
		
#define SB_BIND_OPERATOR_MUL_( Class,Signature,FullSignature ) { \
			"__mul", \
			#Signature, \
			Sandbox::Bind::GetMethodIsConst((FullSignature)(&Class::operator*)), \
			Sandbox::Bind::GetMethodRet((FullSignature)(&Class::operator*)), \
			Sandbox::Bind::GetCallMethodFunc((FullSignature)(&Class::operator*)), \
			Sandbox::Bind::WriteMethodFunc((FullSignature)(&Class::operator*)) \
		},
#define SB_BIND_OPERATOR_MUL( Class,Method,Signature ) { \
			"__mul", \
			#Signature, \
			Sandbox::Bind::GetMethodIsConst(&Class::Method), \
			Sandbox::Bind::GetMethodRet(&Class::Method), \
			Sandbox::Bind::GetCallMethodFunc(&Class::Method), \
			Sandbox::Bind::WriteMethodFunc(&Class::Method) \
		},
#define SB_BIND_OPERATOR_TOSTRING( Class,Method,Signature ) { \
			"__tostring", \
			#Signature, \
			Sandbox::Bind::GetMethodIsConst(&Class::Method), \
			Sandbox::Bind::GetMethodRet(&Class::Method), \
			Sandbox::Bind::GetCallMethodFunc(&Class::Method), \
			Sandbox::Bind::WriteMethodFunc(&Class::Method) \
		},
		
		
#define SB_BIND_ZERO_METHOD {0,0,false,0,0,{{}}}
		
		
		/// implement 0 args
		template <typename T> struct MethodHelper<void(T::*)()> {
			typedef void RetType;
            typedef T   ObjectType;
			typedef void(T::*FuncPtr)();
			enum { isConst = 0,retValues = 0 };
			static T* ConstructInplace(void* ptr,const StackHelper* hpr) {
				return new (ptr) T();
			}
            static T* Construct(const StackHelper* hpr) {
				return new T();
			}
			static void Call(void* obj,const void* funcp,const StackHelper* /*hpr*/) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				(static_cast<T*>(obj)->*func)();
			}
		};
		template <typename T,typename Ret> struct MethodHelper<Ret(T::*)()> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)();
			enum { isConst = 0,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				Pusher<Ret>::Push(hpr,(static_cast<T*>(obj)->*func)());
			}
		};
		template <typename T,typename Ret> struct MethodHelper<Ret(T::*)()const> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)()const;
			enum { isConst = 1,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				Pusher<Ret>::Push(hpr,(static_cast<T*>(obj)->*func)());
			}
		};
		/// implement 1 args
		template <typename T,typename Arg> struct MethodHelper<void(T::*)(Arg)> {
			typedef void RetType;
            typedef T   ObjectType;
			typedef void(T::*FuncPtr)(Arg);
			enum { isConst = 0,retValues = 0 };
			static T* ConstructInplace(void* ptr,const StackHelper* hpr) {
				return new (ptr) T(hpr->GetArgument(0,ArgumentTag<Arg>()));
			}
			static T* Construct(const StackHelper* hpr) {
				return new T(hpr->GetArgument(0,ArgumentTag<Arg>()));
			}
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg>()));
			}
            static void Call(const StackHelper* hpr,int addArgs,Arg a1) { 
                {
                    std::string argType = hpr->get_arg_type(0).str();
                    StackHelper hpr1(hpr->GetState(),1,argType.c_str());
                    hpr1.PushValue(a1);
                }
                hpr->CallVoid(addArgs+1);
			}
		};
		template <typename T,typename Ret,typename Arg> struct MethodHelper<Ret(T::*)(Arg)> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg);
			enum { isConst = 0,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				Pusher<Ret>::Push(hpr,(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg>())));
			}
		};
		template <typename T,typename Ret,typename Arg> struct MethodHelper<Ret(T::*)(Arg)const> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg)const;
			enum { isConst = 1,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				Pusher<Ret>::Push(hpr,(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg>())));
			}
		};
		/// implement 2 args
		template <typename T,typename Arg1,typename Arg2> struct MethodHelper<void(T::*)(Arg1,Arg2)> {
			typedef void RetType;
            typedef T   ObjectType;
			typedef void(T::*FuncPtr)(Arg1,Arg2);
			enum { isConst = 0,retValues = 0 };
			static T* ConstructInplace(void* ptr,const StackHelper* hpr) {
				return new (ptr) T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
							 hpr->GetArgument(1,ArgumentTag<Arg2>()));
			}
			static T* Construct(const StackHelper* hpr) {
				return new T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											   hpr->GetArgument(1,ArgumentTag<Arg2>()));
			}
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											  hpr->GetArgument(1,ArgumentTag<Arg2>()));
			}
		};
		template <typename T,typename Arg1,typename Arg2> struct MethodHelper<void(T::*)(Arg1,Arg2)const> {
			typedef void RetType;
            typedef T   ObjectType;
			typedef void(T::*FuncPtr)(Arg1,Arg2)const;
			enum { isConst = 1,retValues = 0 };
			static T* ConstructInplace(void* ptr,const StackHelper* hpr) {
				return new (ptr) T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
								 hpr->GetArgument(1,ArgumentTag<Arg2>()));
			}
			static T* Construct(const StackHelper* hpr) {
				return new T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
												   hpr->GetArgument(1,ArgumentTag<Arg2>()));
			}
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											  hpr->GetArgument(1,ArgumentTag<Arg2>()));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2> struct MethodHelper<Ret(T::*)(Arg1,Arg2)> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2);
			enum { isConst = 0,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				Pusher<Ret>::Push(hpr,(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>())));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2> 
		struct MethodHelper<Ret(T::*)(Arg1,Arg2)const> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2)const;
			enum { isConst = 1,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				Pusher<Ret>::Push(hpr,(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>())));
			}
		};
		/// implement 3 args
		template <typename T,typename Arg1,typename Arg2,typename Arg3> 
		struct MethodHelper<void(T::*)(Arg1,Arg2,Arg3)> {
			typedef void RetType;
            typedef T   ObjectType;
			typedef void(T::*FuncPtr)(Arg1,Arg2,Arg3);
			enum { isConst = 0,retValues = 0 };
			static T* ConstructInplace(void* ptr,const StackHelper* hpr) {
				return new (ptr) T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
							 hpr->GetArgument(1,ArgumentTag<Arg2>()),
							 hpr->GetArgument(2,ArgumentTag<Arg3>()));
			}
			static T* Construct(const StackHelper* hpr) {
				return new T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											   hpr->GetArgument(1,ArgumentTag<Arg2>()),
											   hpr->GetArgument(2,ArgumentTag<Arg3>()));
			}
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											  hpr->GetArgument(1,ArgumentTag<Arg2>()),
											  hpr->GetArgument(2,ArgumentTag<Arg3>()));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2,typename Arg3> 
		struct MethodHelper<Ret(T::*)(Arg1,Arg2,Arg3)> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2,Arg3);
			enum { isConst = 0,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				Pusher<Ret>::Push(hpr,(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>()),
															 hpr->GetArgument(2,ArgumentTag<Arg3>())));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2,typename Arg3> 
		struct MethodHelper<Ret(T::*)(Arg1,Arg2,Arg3)const> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2,Arg3)const;
			enum { isConst = 1,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				Pusher<Ret>::Push(hpr,(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>()),
															 hpr->GetArgument(2,ArgumentTag<Arg3>())));
			}
		};
		/// implement 4 args
		template <typename T,typename Arg1,typename Arg2,typename Arg3,typename Arg4> 
		struct MethodHelper<void(T::*)(Arg1,Arg2,Arg3,Arg4)> {
			typedef void RetType;
            typedef T   ObjectType;
			typedef void(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4);
			enum { isConst = 0,retValues = 0 };
			static T* ConstructInplace(void* ptr,const StackHelper* hpr) {
				return new (ptr) T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
							 hpr->GetArgument(1,ArgumentTag<Arg2>()),
							 hpr->GetArgument(2,ArgumentTag<Arg3>()),
							 hpr->GetArgument(3,ArgumentTag<Arg4>()));
			}
			static T* Construct(void* ptr,const StackHelper* hpr) {
				return new T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											   hpr->GetArgument(1,ArgumentTag<Arg2>()),
											   hpr->GetArgument(2,ArgumentTag<Arg3>()),
											   hpr->GetArgument(3,ArgumentTag<Arg4>()));
			}
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											  hpr->GetArgument(1,ArgumentTag<Arg2>()),
											  hpr->GetArgument(2,ArgumentTag<Arg3>()),
											  hpr->GetArgument(3,ArgumentTag<Arg4>()));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4> 
		struct MethodHelper<Ret(T::*)(Arg1,Arg2,Arg3,Arg4)> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4);
			enum { isConst = 0,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				hpr->PushValue((static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>()),
															 hpr->GetArgument(2,ArgumentTag<Arg3>()),
															 hpr->GetArgument(3,ArgumentTag<Arg4>())));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4> 
		struct MethodHelper<Ret(T::*)(Arg1,Arg2,Arg3,Arg4)const> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4)const;
			enum { isConst = 1,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				hpr->PushValue((static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>()),
															 hpr->GetArgument(2,ArgumentTag<Arg3>()),
															 hpr->GetArgument(3,ArgumentTag<Arg4>())));
			}
		};
		/// implement 5 args
		template <typename T,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5> 
		struct MethodHelper<void(T::*)(Arg1,Arg2,Arg3,Arg4,Arg5)> {
			typedef void RetType;
            typedef T   ObjectType;
			typedef void(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4,Arg5);
			enum { isConst = 0,retValues = 0 };
			static T* ConstructInplace(void* ptr,const StackHelper* hpr) {
				return new (ptr) T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
							 hpr->GetArgument(1,ArgumentTag<Arg2>()),
							 hpr->GetArgument(2,ArgumentTag<Arg3>()),
							 hpr->GetArgument(3,ArgumentTag<Arg4>()),
							 hpr->GetArgument(4,ArgumentTag<Arg5>()));
			}
			static T* Construct(const StackHelper* hpr) {
				return new T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											   hpr->GetArgument(1,ArgumentTag<Arg2>()),
											   hpr->GetArgument(2,ArgumentTag<Arg3>()),
											   hpr->GetArgument(3,ArgumentTag<Arg4>()),
											   hpr->GetArgument(4,ArgumentTag<Arg5>()));
			}
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											  hpr->GetArgument(1,ArgumentTag<Arg2>()),
											  hpr->GetArgument(2,ArgumentTag<Arg3>()),
											  hpr->GetArgument(3,ArgumentTag<Arg4>()),
											  hpr->GetArgument(4,ArgumentTag<Arg5>()));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5> 
		struct MethodHelper<Ret(T::*)(Arg1,Arg2,Arg3,Arg4,Arg5)> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4,Arg5);
			enum { isConst = 0,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				hpr->PushValue((static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>()),
															 hpr->GetArgument(2,ArgumentTag<Arg3>()),
															 hpr->GetArgument(3,ArgumentTag<Arg4>()),
															 hpr->GetArgument(4,ArgumentTag<Arg5>())));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5> 
		struct MethodHelper<Ret(T::*)(Arg1,Arg2,Arg3,Arg4,Arg5)const> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4,Arg5)const;
			enum { isConst = 1,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				hpr->PushValue((static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>()),
															 hpr->GetArgument(2,ArgumentTag<Arg3>()),
															 hpr->GetArgument(3,ArgumentTag<Arg4>()),
															 hpr->GetArgument(4,ArgumentTag<Arg5>())));
			}
		};
		
		
		
		
		/// implement 6 args
		template <typename T,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5,typename Arg6> 
		struct MethodHelper<void(T::*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)> {
			typedef void RetType;
            typedef T   ObjectType;
			typedef void(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6);
			enum { isConst = 0,retValues = 0 };
			static T* ConstructInplace(void* ptr,const StackHelper* hpr) {
				return new (ptr) T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											  hpr->GetArgument(1,ArgumentTag<Arg2>()),
											  hpr->GetArgument(2,ArgumentTag<Arg3>()),
											  hpr->GetArgument(3,ArgumentTag<Arg4>()),
											  hpr->GetArgument(4,ArgumentTag<Arg5>()),
											  hpr->GetArgument(5,ArgumentTag<Arg6>()));
			}
			static T* Construct(const StackHelper* hpr) {
				return new T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
																	   hpr->GetArgument(1,ArgumentTag<Arg2>()),
																	   hpr->GetArgument(2,ArgumentTag<Arg3>()),
																	   hpr->GetArgument(3,ArgumentTag<Arg4>()),
																	   hpr->GetArgument(4,ArgumentTag<Arg5>()),
																	   hpr->GetArgument(5,ArgumentTag<Arg6>()));
			}
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											  hpr->GetArgument(1,ArgumentTag<Arg2>()),
											  hpr->GetArgument(2,ArgumentTag<Arg3>()),
											  hpr->GetArgument(3,ArgumentTag<Arg4>()),
											  hpr->GetArgument(4,ArgumentTag<Arg5>()),
											  hpr->GetArgument(5,ArgumentTag<Arg6>()));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5,typename Arg6> 
		struct MethodHelper<Ret(T::*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6);
			enum { isConst = 0,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				hpr->PushValue((static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>()),
															 hpr->GetArgument(2,ArgumentTag<Arg3>()),
															 hpr->GetArgument(3,ArgumentTag<Arg4>()),
															 hpr->GetArgument(4,ArgumentTag<Arg5>()),
															 hpr->GetArgument(5,ArgumentTag<Arg6>())));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5,typename Arg6> 
		struct MethodHelper<Ret(T::*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)const> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)const;
			enum { isConst = 1,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				hpr->PushValue((static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>()),
															 hpr->GetArgument(2,ArgumentTag<Arg3>()),
															 hpr->GetArgument(3,ArgumentTag<Arg4>()),
															 hpr->GetArgument(4,ArgumentTag<Arg5>()),
															 hpr->GetArgument(5,ArgumentTag<Arg6>())));
			}
		};
		
		/// implement 7 args
		template <typename T,typename Arg1,typename Arg2,typename Arg3,typename Arg4,
					typename Arg5,typename Arg6,typename Arg7> 
		struct MethodHelper<void(T::*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)> {
			typedef void RetType;
            typedef T   ObjectType;
			typedef void(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7);
			enum { isConst = 0,retValues = 0 };
			static T* ConstructInplace(void* ptr,const StackHelper* hpr) {
				return new (ptr) T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											  hpr->GetArgument(1,ArgumentTag<Arg2>()),
											  hpr->GetArgument(2,ArgumentTag<Arg3>()),
											  hpr->GetArgument(3,ArgumentTag<Arg4>()),
											  hpr->GetArgument(4,ArgumentTag<Arg5>()),
											  hpr->GetArgument(5,ArgumentTag<Arg6>()),
											  hpr->GetArgument(6,ArgumentTag<Arg7>()));
			}
			static T* Construct(const StackHelper* hpr) {
				return new T(hpr->GetArgument(0,ArgumentTag<Arg1>()),
																	   hpr->GetArgument(1,ArgumentTag<Arg2>()),
																	   hpr->GetArgument(2,ArgumentTag<Arg3>()),
																	   hpr->GetArgument(3,ArgumentTag<Arg4>()),
																	   hpr->GetArgument(4,ArgumentTag<Arg5>()),
																	   hpr->GetArgument(5,ArgumentTag<Arg6>()),
																	   hpr->GetArgument(6,ArgumentTag<Arg7>()));
			}
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				(static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
											  hpr->GetArgument(1,ArgumentTag<Arg2>()),
											  hpr->GetArgument(2,ArgumentTag<Arg3>()),
											  hpr->GetArgument(3,ArgumentTag<Arg4>()),
											  hpr->GetArgument(4,ArgumentTag<Arg5>()),
											  hpr->GetArgument(5,ArgumentTag<Arg6>()),
											  hpr->GetArgument(6,ArgumentTag<Arg7>()));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4,
					typename Arg5,typename Arg6,typename Arg7> 
		struct MethodHelper<Ret(T::*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7);
			enum { isConst = 0,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				hpr->PushValue((static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>()),
															 hpr->GetArgument(2,ArgumentTag<Arg3>()),
															 hpr->GetArgument(3,ArgumentTag<Arg4>()),
															 hpr->GetArgument(4,ArgumentTag<Arg5>()),
															 hpr->GetArgument(5,ArgumentTag<Arg6>()),
															 hpr->GetArgument(6,ArgumentTag<Arg7>())));
			}
		};
		template <typename T,typename Ret,typename Arg1,typename Arg2,typename Arg3,typename Arg4,
					typename Arg5,typename Arg6,typename Arg7> 
		struct MethodHelper<Ret(T::*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)const> {
			typedef Ret RetType;
            typedef T   ObjectType;
			typedef Ret(T::*FuncPtr)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)const;
			enum { isConst = 1,retValues = 1 };
			static void Call(void* obj,const void* funcp,const StackHelper* hpr) {
				FuncPtr func = *static_cast<const FuncPtr*> (funcp);
				hpr->PushValue((static_cast<T*>(obj)->*func)(hpr->GetArgument(0,ArgumentTag<Arg1>()),
															 hpr->GetArgument(1,ArgumentTag<Arg2>()),
															 hpr->GetArgument(2,ArgumentTag<Arg3>()),
															 hpr->GetArgument(3,ArgumentTag<Arg4>()),
															 hpr->GetArgument(4,ArgumentTag<Arg5>()),
															 hpr->GetArgument(5,ArgumentTag<Arg6>()),
															 hpr->GetArgument(6,ArgumentTag<Arg7>())));
			}
		};
		
		
	}
}

#endif /*SB_BIND_METHOD_H*/
