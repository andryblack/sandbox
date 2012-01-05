/*
 *  sb_lua.h
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_LUA_H
#define SB_LUA_H


#include "sb_shared_ptr.h"
#include "sb_bind_stack.h"

#include <vector>
#include <string>

#include <ghl_types.h>
#include <ghl_data_stream.h>

struct lua_State;

namespace GHL {
	struct DataStream;
	struct VFS;
}
namespace Sandbox {
	
	namespace Bind {
		struct ClassBind;
		struct EnumBind;
        struct ExtensibleClassBind;
	}
	class Lua;
	struct LuaHelper {
		Lua* lua;
	};
	typedef shared_ptr<LuaHelper> LuaHelperPtr;
	typedef weak_ptr<LuaHelper> LuaHelperWeakPtr;
	class LuaReference {
	public:
		explicit LuaReference( const LuaHelperWeakPtr& ptr );
		~LuaReference();
		void SetObject( lua_State* state );
		void UnsetObject( lua_State* state );
		void GetObject( lua_State* state );
		LuaHelperPtr GetHelper() const { return m_lua.lock();}
		const LuaHelperWeakPtr& GetHelperPtr() const { return m_lua;}
		bool Valid() const;
	private:
		LuaHelperWeakPtr m_lua;
		int	m_ref;
	};
	class LuaEnvironment;
	typedef shared_ptr<LuaEnvironment> LuaEnvironmentPtr;

	class LuaFunction {
	public:
		LuaFunction( const LuaHelperWeakPtr& ptr );
		void Call();
		LuaEnvironmentPtr GetEnv();
	private:
		LuaReference	m_ref;
	};
	typedef shared_ptr<LuaFunction> LuaFunctionPtr;
	
	class LuaEnvironment {
	public:
		LuaEnvironment( const LuaHelperWeakPtr& ptr ,int indx=0);
		~LuaEnvironment();
		void GetEnv( lua_State* state );
		LuaFunctionPtr LoadFunction( const char* content );
		LuaFunctionPtr LoadFunction( const char* name, GHL::DataStream* stream );
	private:
		LuaReference	m_ref;
	};
	
	
	
	
	class Lua {
	public:
		explicit Lua(GHL::VFS* vfs);
		~Lua();
		void SetBasePath(const char* path);
		bool DoFile(const char* fn);
		lua_State* GetVM() const { return m_state;}
		static Lua* GetPtr(lua_State* s);
		template <class T>
		void SetValue(T obj,const char* name,const char* type) {
			Bind::StackHelper stck(GetVM(),0,type);
			stck.PushValue(obj);
			register_object(name);
		}
		template <class T>
		void SetValue(T* obj,const char* name,const char* type) {
			Bind::StackHelper stck(GetVM(),0,type);
			stck.template PushValuePtr<T>(obj);
			register_object(name);
		}
        template <class T>
		void SetValue(const T* obj,const char* name,const char* type) {
			Bind::StackHelper stck(GetVM(),0,type);
			stck.PushValue(obj);
			register_object(name);
		}
		template <class T>
		void SetValue(const LuaEnvironmentPtr& env, T* obj,const char* name,const char* type) {
			Bind::StackHelper stck(GetVM(),0,type);
			stck.template PushValuePtr<T>(obj);
			register_object(env,name);
		}
		template <class T>
		void SetValue(const shared_ptr<T>& obj,const char* name,const char* type) {
			Bind::StackHelper stck(GetVM(),0,type);
			stck.PushValue(obj);
			register_object(name);
		}
		void Call(const char* func);
		template <class T>
		void Call(const char* func,const char* arg,T obj) {
			Bind::StackHelper stck(GetVM(),0,arg);
			stck.PushValue(obj);
			do_call(func,1);
		}
		template <class T1,class T2>
		void Call(const char* func,const char* arg1,T1 obj1,const char* arg2,T2 obj2) {
			{
			 Bind::StackHelper stck(GetVM(),0,arg1);
			 stck.PushValue(obj1);
			}
			{
			 Bind::StackHelper stck(GetVM(),0,arg2);
			 stck.PushValue(obj2);
			}
			do_call(func,2);
		}
		void Bind(const Bind::ClassBind* info);
        void Bind(const Bind::ExtensibleClassBind* info);
		void Bind(const Bind::EnumBind* info);
		LuaHelperWeakPtr GetHelper() const { return m_helper;}
		
		GHL::UInt32 GetMemoryUsed() const { return m_mem_use;} 
		
		LuaEnvironmentPtr CreateEnvironment();
	private:
		lua_State*	m_state;
		LuaHelperPtr m_helper;
		GHL::VFS*	m_vfs;
		std::string m_base_path;
		void RegisterSandboxObjects();
		void register_object(const char* name);
		void register_object(const LuaEnvironmentPtr& env, const char* name);
		
		bool load_file(lua_State*,const char* name);
		static int lua_dofile_func (lua_State *L);
		const char* get_table(const char* str);
		const char* get_table(const LuaEnvironmentPtr& env,const char* str);
		bool call(const char* str,int args);
		void do_call(const char* str,int args);
		static void *lua_alloc_func (void *ud, void *_ptr, size_t osize,size_t nsize);
		GHL::UInt32 m_mem_use;
		GHL::Byte* alloc(size_t size);
		void free(GHL::Byte* data,size_t size);
		void resize(GHL::Byte* data,size_t osize,size_t nsize);
	};
}

#endif /*SB_LUA_H*/
