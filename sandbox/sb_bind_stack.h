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
#include <map>
#include <ghl_types.h>

struct lua_State;

namespace Sandbox {
	namespace Bind {
		
		struct ObjectData;
		struct ClassInfo;
		
		template <class T> struct IsEnumTag{};
		template <class T> struct IsNotEnumTag{};
		
		template <class T> struct IsStringTag{
			static T to( const char* v ) {
				return v;
			}
		};
				
		template <class T> struct ArgumentTag : public SelectType<IsEnum<T>::Result,IsEnumTag<T>,IsNotEnumTag<T> >::Result{ 
		};

		template <> struct ArgumentTag<const char*> : public IsStringTag<const char*>{};

		
		class StackHelper {
		private:
			lua_State* m_L;
			int	m_base_index;
			const char* m_signature;
			void* get_ptr(int indx) const;
			void get_shared_ptr(int indx,void* to) const;
			void push_object_ptr(void* v) const;
            void push_object_ptr(const void* v) const;
			bool is_null(int indx) const;
			void push_null() const;
			template <class T> void _PushValue(const T& v , const IsNotEnumTag<T>&  ) const {
				new (new_object_raw()) T(v);
			}
			template <class T> void _PushValue(T v , const IsEnumTag<T>&   ) const {
				PushValue(static_cast<int> (v));
			}
			const char* get_string(int indx) const;
            bool begin_read_map(int indx) const;
            bool get_map_argument(int index) const;
            void end_get_map_argument() const;
            void end_read_map() const;
		public:
	explicit StackHelper(lua_State* L,int base_index,const char* signature) : m_L(L),m_base_index(base_index),m_signature(signature) {}
            lua_State* GetState() const { return m_L; }
            InplaceString get_arg_type(int num) const;
			static ObjectData*  check_object_type(lua_State* L,int indx,const char* type,bool derived);
			static void* get_object_ptr(lua_State* L,int indx,const ClassInfo* name);
			void* new_object_raw() const;
			void* new_object_shared_ptr() const;
			static void rawgetfield (lua_State *L, int idx, const char *key);
			static void rawsetfield (lua_State *L, int idx, const char *key);
			void push_string(const char* str) const;
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
            template <class T> std::map<std::string,T> GetArgument(int indx,const ArgumentTag<const std::map<std::string,T>&>& ) const {
				std::map<std::string,T> res;
                std::string   type = get_arg_type(indx).str();
                size_t p1 = type.find('<' );
                sb_assert(p1!=type.npos);
                type.replace(p1, 1, "(");
                size_t p2 = type.find('>',p1);
                sb_assert(p2!=type.npos);
                 type.replace(p2, 1, ")");
                if (begin_read_map(indx)) {
                    while (get_map_argument(indx)) {
                        StackHelper hpr(m_L,-1,type.c_str());
                        res[hpr.get_string(-1)]=hpr.GetArgument(0,ArgumentTag<T>());
                        end_get_map_argument();
                    }
                    end_read_map();
                }
				return res;
			}
			template <class T> const T& GetArgument(int indx,const IsNotEnumTag<const T&>& ) const {
				return *static_cast<T*> (get_ptr(indx));
			}
            template <class T> T* GetArgument(int indx,const ArgumentTag<T*>& ) const {
				return static_cast<T*> (get_ptr(indx));
			}
			template <class T>
			T GetArgument(int indx,const IsEnumTag<T>& ) const {
				return static_cast<T>(GetArgument(indx,ArgumentTag<int>()));
			}
			template <class T>
			T GetArgument(int indx,const IsStringTag<T>& ) const {
				return IsStringTag<T>::to(get_string(indx));
			}
			bool GetArgument(int indx,const ArgumentTag<bool>& ) const;
			int GetArgument(int indx,const ArgumentTag<int>& ) const;
			GHL::UInt32 GetArgument(int indx,const ArgumentTag<GHL::UInt32>& ) const;
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
					new (new_object_shared_ptr()) shared_ptr<T>(v);
				else
					push_null();
			}
			void PushValue(int v) const;
			void PushValue(GHL::UInt32 v) const;
			void PushValue(size_t v) const;
			void PushValue(float v) const;
			void PushValue(const char* v) const;
			void PushValue(const std::string& v) const;
			void PushValue(const InplaceString& v) const;
			void PushValue(bool v) const;
            
            void CallVoid(int numArgs) const;
		};
		
		template <class T> struct Pusher {
			static void Push( const StackHelper* helper, typename type_traits<T>::parameter_type t) {
				helper->PushValue(t);
			}
		};
		
	}
}

#endif /*SB_BIND_STACK_H*/
