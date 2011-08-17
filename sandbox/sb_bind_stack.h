/*
 *  sb_bind_stack.h
 *  SR
 *
 *  Created by Андрей Куницын on 09.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */
#ifndef SB_BIND_STACK_H
#define SB_BIND_STACK_H

#include "sb_assert.h"
#include "sb_shared_ptr.h"
#include "sb_traits.h"
#include "sb_inplace_string.h"
#include <string>

struct lua_State;

namespace Sandbox {
	namespace Bind {
		
		struct ObjectData;
		struct ClassInfo;
		
		template <class T> struct IsEnumTag{};
		template <class T> struct IsNotEnumTag{};
		
				
		template <class T> struct ArgumentTag : public SelectType<IsEnum<T>::Result,IsEnumTag<T>,IsNotEnumTag<T> >::Result{ 
		};


		
		class StackHelper {
		private:
			lua_State* m_L;
			int	m_base_index;
			const char* m_signature;
			void* get_ptr(int indx) const;
			void get_shared_ptr(int indx,void* to) const;
			
			void push_object_ptr(void* v) const;
			void* push_object_raw() const;
			void* push_object_shared_ptr() const;
			bool is_null(int indx) const;
			void push_null() const;
			template <class T> void _PushValue(const T& v , const IsNotEnumTag<T>&  ) const {
				new (push_object_raw()) T(v);
			}
			template <class T> void _PushValue(T v , const IsEnumTag<T>&   ) const {
				PushValue(static_cast<int> (v));
			}
		public:
			explicit StackHelper(lua_State* L,int base_index,const char* signature) : m_L(L),m_base_index(base_index),m_signature(signature) {}
			static ObjectData*  check_object_type(lua_State* L,int indx,const char* type,bool derived);
			static void* get_object_ptr(lua_State* L,int indx,const ClassInfo* name);
			static void rawgetfield (lua_State *L, int idx, const char *key);
			static void rawsetfield (lua_State *L, int idx, const char *key);
			template <class T> shared_ptr<T> GetArgument(int indx,const ArgumentTag<const shared_ptr<T>&>& ) const {
				shared_ptr<T> res;
				if (!is_null(indx)) get_shared_ptr(indx,&res);
				return res;
			}
			template <class T> shared_ptr<T> GetArgument(int indx,const ArgumentTag<shared_ptr<T> >& ) const {
				shared_ptr<T> res;
				if (!is_null(indx)) get_shared_ptr(indx,&res);
				return res;
			}
			template <class T> const T& GetArgument(int indx,const ArgumentTag<const T&>& ) const {
				return *static_cast<T*> (get_ptr(indx));
			}
			/*template <class T> T GetArgument(int indx,const ArgumentTag<T>& ) const {
				return *static_cast<T*> (get_ptr(indx));
			}*/
			template <class T>
			T GetArgument(int indx,const IsEnumTag<T>& ) const {
				return static_cast<T>(GetArgument(indx,ArgumentTag<int>()));
			}
			bool GetArgument(int indx,const ArgumentTag<bool>& ) const;
			int GetArgument(int indx,const ArgumentTag<int>& ) const;
			size_t GetArgument(int indx,const ArgumentTag<size_t>& ) const;
			float GetArgument(int indx,const ArgumentTag<float>& ) const;
			const char* GetArgument(int indx,const ArgumentTag<const char*>& ) const;
			
			template <class T> void PushValue(const T& v ) const {
				_PushValue(v,ArgumentTag<T>());
			}
			template <class T> void PushValue(T* v) const {
				if (v)
					push_object_ptr(v);
				else
					push_null();
			}
			template <class T> void PushValuePtr(T* v) const {
				if (v)
					push_object_ptr(v);
				else
					push_null();
			}
			template <class T> void PushValue(const shared_ptr<T>& v) const {
				if (v)
					new (push_object_shared_ptr()) shared_ptr<T>(v);
				else
					push_null();
			}
			void PushValue(int v) const;
			void PushValue(size_t v) const;
			void PushValue(float v) const;
			void PushValue(const char* v) const;
			void PushValue(const std::string& v) const;
			void PushValue(const InplaceString& v) const;
			void PushValue(bool v) const;
		};
		
		
	}
}

#endif /*SB_BIND_STACK_H*/
