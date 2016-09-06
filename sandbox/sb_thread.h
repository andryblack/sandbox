/*
 *  sb_thread.h
 *  SR
 *
 *  Created by Андрей Куницын on 15.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_THREAD_H
#define SB_THREAD_H

#include "meta/sb_meta.h"
#include "luabind/sb_luabind_ref.h"
#include "luabind/sb_luabind_stack.h"
#include "luabind/sb_luabind.h"


namespace Sandbox {
	
	class Thread : public meta::object {
        SB_META_OBJECT
	public:
		virtual ~Thread();
		virtual bool Update(float dt) = 0;
        virtual void Clear() {}
    protected:
        Thread();
	};
	typedef sb::intrusive_ptr<Thread> ThreadPtr;
    
    class LuaThread : public Thread {
    public:
        ~LuaThread() {}
		bool Update(float dt);
		static int constructor_func(lua_State* L);
        static sb::intrusive_ptr<LuaThread> construct(lua_State* L,int idx);
        virtual void Clear();
    private:
        explicit LuaThread(luabind::LuaVMHelperWeakPtr ptr) : m_ref(ptr) {}
		void SetThread(lua_State* L);
		luabind::LuaReference m_ref;
	};

    namespace luabind {
        template <>
        struct stack<ThreadPtr> {
            static void push( lua_State* L, const ThreadPtr& val ) {
                stack_push_impl(L, val);
            }
            static ThreadPtr get( lua_State* L, int idx ) {
                if (lua_isfunction(L, idx)) {
                    return LuaThread::construct(L, idx);
                }
                return get_intrusive_ptr<Thread>(L,idx);
            }
        };
        template <>
        struct stack<const ThreadPtr&> : stack<ThreadPtr> {};
        
    }
	
}

#endif /*SB_THREAD_H*/
