/*
 *  sb_lua_objects.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */


#include "sb_lua.h"
#include "sb_bind.h"

#include "sb_vector2.h"
#include "sb_rect.h"
#include "sb_vector3.h"
#include "sb_matrix4.h"
#include "sb_resources.h"
#include "sb_texture.h"
#include "sb_image.h"
#include "sb_object.h"
#include "sb_scene.h"
#include "sb_sprite.h"
#include "sb_container.h"
#include "sb_container_blend.h"
#include "sb_container_visible.h"
#include "sb_font.h"
#include "sb_label.h"
#include "sb_container_transform.h"
#include "sb_container_transform3d.h"
#include "sb_bind_enum.h"
#include "sb_graphics.h"
#include "sb_shader.h"
#include "sb_event.h"
#include "sb_container_shader.h"
#include "sb_thread.h"
#include "sb_threads_mgr.h"
#include "sb_controller.h"
#include "sb_controllers_transform.h"
#include "sb_color.h"
#include "sb_processor.h"
#include "sb_container_color.h"
#include "sb_controllers_color.h"
#include "sb_controllers_shader.h"
#include "sb_controller_split.h"
#include "sb_controller_bidirect.h"
#include "sb_controller_phase.h"
#include "sb_controller_map.h"
#include "sb_controller_elastic.h"
#include "sb_animation.h"
#include "sb_rect.h"
#include "sb_container_viewport.h"
#include "sb_background.h"
#include "sb_particles.h"
#include "sb_clear_scene.h"
#include "sb_widget.h"
#include "sb_fill_rect.h"
#include "sb_touch_info.h"

#include "sb_log.h"

extern "C" {
#include "../lua/src/lua.h"
#include "../lua/src/lauxlib.h"
}

#include <cstdio>
#ifdef _MSC_VER
#define snprintf _snprintf
#endif

namespace Sandbox {

	inline static GHL::Byte conv_hex_char(GHL::Byte c) {
		if ( (c>='0')&&(c<='9')) return c-'0';
		if ( (c>='a')&&(c<='f')) return (c-'a')+0xa;
		if ( (c>='A')&&(c<='F')) return (c-'A')+0xa;
		return 0;
	}
	
	inline static GHL::Byte conv_hex(const char* str) {
		return (conv_hex_char(*str++)<<4) | conv_hex_char(*str);
	}

	static int lua_color_from_string_func(lua_State* L) {
		Bind::StackHelper hpr(L,1,"Sandbox::Color(const char*)");
		const char* str = hpr.GetArgument(0, Bind::ArgumentTag<const char*>());
		size_t len = ::strlen(str);
		GHL::Byte c[4];
		if (len>=6) {
			c[0]=conv_hex(str);
			c[1]=conv_hex(str+2);
			c[2]=conv_hex(str+4);
			if (len>=8) {
				c[3]=conv_hex(str+6);
			} else {
				c[3]=0xff;
			}
		}
		hpr.PushValue(Color::from_bytes(c));
		return 1;
	}
	
    static const char* const LuaEventModule = "Sanbox:LuaEvent";
  	class LuaEvent : public Event {
	public:
        explicit LuaEvent(LuaHelperWeakPtr ptr) : m_ref(ptr) {}
		~LuaEvent() {
		}
		void SetFunction(lua_State* L) {
			m_ref.SetObject(L);
		}
		void Emmit() {
			if (m_ref.Valid()) {
				if ( LuaHelperPtr lua = m_ref.GetHelper()) {
					lua_State* L = lua->lua->GetVM();
					sb_assert(L);
					m_ref.GetObject(L);
					sb_assert(lua_isfunction(L,-1));
					int res = lua_pcall(L, 0, 0, 0);
					if (res) {
						LogError(LuaEventModule) << " Failed script event emmit  " ;
                        LogError(LuaEventModule) << lua_tostring(L, -1) ;
					}
				} else {
					LogError(LuaEventModule) <<" call emmit on released script" ;
				}
			}
		}
		static int constructor_func(lua_State* L) {
			if (!lua_isfunction(L,2)) {
				char buf[128];
				::snprintf(buf,127,"function expected, got %s",luaL_typename(L, 2));
				luaL_argerror(L, 2, buf);
				return 0;
			}
 			Lua* lua = Lua::GetPtr(L);
			lua_State* main_state = lua->GetVM();
			shared_ptr<LuaEvent> e = shared_ptr<LuaEvent>(new LuaEvent(lua->GetHelper()));
			lua_pushvalue(L, 2);
			if (main_state!=L) {
				lua_xmove(L, main_state, 1);
			}
			e->SetFunction(main_state);
			Bind::StackHelper hpr(L,0,"Sandbox::LuaEvent");
			hpr.PushValue(e);
			return 1;
		}
	private:
		LuaReference	m_ref;
	};
	
	static const char* const LuaThreadModule = "Sanbox:LuaThread";
	class LuaThread : public Thread {
	public:
		explicit LuaThread(LuaHelperWeakPtr ptr) : m_ref(ptr) {}
		~LuaThread() {
		}
		void SetThread(lua_State* L) {
			sb_assert(lua_isthread(L,-1));
			m_ref.SetObject( L );
		}
		bool Update(float dt) {
			if (m_ref.Valid()) {
				if ( LuaHelperPtr lua = m_ref.GetHelper()) {
					lua_State* L = lua->lua->GetVM();
					sb_assert(L);
					m_ref.GetObject(L);
					if (!lua_isthread(L,-1)) {
						LogError(LuaThreadModule) << "not thread :" << luaL_typename(L,-1);
						sb_assert(lua_isthread(L,-1));
                        return false;
					} 
					lua_State* th = lua_tothread(L, -1);
					lua_pushnumber(th, dt);
					int res = lua_resume(th, 1);
					lua_pop(L,1);
					if (res==LUA_YIELD) {
						return false;
					}
					if (res) {
						LogError(LuaThreadModule) << "Failed script resume  " ;
                        LogError(LuaThreadModule) << lua_tostring(th, -1) ;
					}
				} else {
					LogError(LuaThreadModule) << "update on released script";
				}
			}
			return true;
		}
		static int constructor_func(lua_State* L) {
			int stck = lua_gettop(L);
			if (!lua_isfunction(L,2)) {
				char buf[128];
				::snprintf(buf,127,"function expected, got %s",luaL_typename(L, 2));
				luaL_argerror(L, 2, buf);
				return 0;
			}
			//int stck_L = lua_gettop(L);
			Lua* lua = Lua::GetPtr(L);
			lua_State* main_state = lua->GetVM();
			sb_assert(lua_checkstack(main_state,3));
			shared_ptr<LuaThread> e = shared_ptr<LuaThread>(new LuaThread(lua->GetHelper()));
			//sb_assert(stck==lua_gettop(L));
			lua_pushvalue(L, 2);						
			if (main_state!=L) {
				lua_xmove(L, main_state, 1);			/// (1) func
			}
			lua_State* thL = lua_newthread(main_state); /// (2) th,func
			sb_assert(lua_isthread(main_state,-1));
			lua_pushvalue(main_state, -2);				/// (3) func,th,func
			lua_xmove(main_state, thL, 1);				/// (2) th,func
			int res = lua_resume(thL, 0);
			if (res==LUA_YIELD) {
				e->SetThread(main_state);				/// (1) func
			} else {
				if (res!=0) {
					LogError(LuaThreadModule) <<"Failed thread run  ";
                    LogError(LuaThreadModule) << lua_tostring(thL, -1);
				}
				lua_pop(main_state,1);					/// (1) func
			}
			lua_pop(main_state,1);
			int new_top = lua_gettop(L);
			(void)stck;
			(void)new_top;
			sb_assert(stck==new_top);
			Bind::StackHelper hpr(L,0,"Sandbox::LuaThread");
			hpr.PushValue(e);
			return 1;
		}
	private:
		LuaReference m_ref;
	};
    
    
    
    static const char* const LuaWidgetModule = "Sanbox:LuaWidget";
    class LuaWidget : public Widget {
	public:
        explicit LuaWidget(LuaHelperWeakPtr ptr) : m_ref(ptr) {}
		~LuaWidget() {
		}
		void SetObject(lua_State* L) {
			m_ref.SetObject(L);
		}
		bool HandleTouchInside(const TouchInfo& touch) {
			if (m_ref.Valid()) {
				if ( LuaHelperPtr lua = m_ref.GetHelper()) {
					lua_State* L = lua->lua->GetVM();
					sb_assert(L);
					m_ref.GetObject(L);
					sb_assert(lua_istable(L,-1));
                    lua_getfield(L, -1, "HandleTouchInside");
                    if (!lua_isfunction(L, -1)) {
                        lua_pop(L, 2);
                        return false;
                    }
                    lua_pushvalue(L, -2);
                    Bind::StackHelper stck(L,0,"Sandbox::TouchInfo");
                    stck.PushValue(touch);
					int res = lua_pcall(L, 2, 1, 0);
					if (res) {
						LogError(LuaWidgetModule) << " Failed call HandleTouchInside  " ;
                        LogError(LuaWidgetModule) << lua_tostring(L, -1) ;
					} else {
                        if (!lua_isboolean(L, -1)) {
                            LogError(LuaWidgetModule) << " HandleTouchInside need bool result " ;
                        } else {
                            bool res = lua_toboolean(L, -1);
                            lua_pop(L, 2);
                            return res;
                        }
                    }
				} else {
					LogError(LuaEventModule) <<" call HandleTouchInside on released script" ;
				}
			}
            return false;
		}
        void OnTouchEnter() {
			if (m_ref.Valid()) {
				if ( LuaHelperPtr lua = m_ref.GetHelper()) {
					lua_State* L = lua->lua->GetVM();
					sb_assert(L);
					m_ref.GetObject(L);
					sb_assert(lua_istable(L,-1));
                    lua_getfield(L, -1, "OnTouchEnter");
                    if (!lua_isfunction(L, -1)) {
                        lua_pop(L, 2);
                        return ;
                    }
                    lua_pushvalue(L, -2);
                    int res = lua_pcall(L, 1, 0, 0);
					if (res) {
						LogError(LuaWidgetModule) << " Failed call OnTouchEnter  " ;
                        LogError(LuaWidgetModule) << lua_tostring(L, -1) ;
					} else {
                       
                    }
				} else {
					LogError(LuaEventModule) <<" call OnTouchEnter on released script" ;
				}
			}
            return ;
		}
        void OnTouchLeave() {
			if (m_ref.Valid()) {
				if ( LuaHelperPtr lua = m_ref.GetHelper()) {
					lua_State* L = lua->lua->GetVM();
					sb_assert(L);
					m_ref.GetObject(L);
					sb_assert(lua_istable(L,-1));
                    lua_getfield(L, -1, "OnTouchLeave");
                    if (!lua_isfunction(L, -1)) {
                        lua_pop(L, 2);
                        return ;
                    }
                    lua_pushvalue(L, -2);
                    int res = lua_pcall(L, 1, 0, 0);
					if (res) {
						LogError(LuaWidgetModule) << " Failed call OnTouchLeave  " ;
                        LogError(LuaWidgetModule) << lua_tostring(L, -1) ;
					} else {
                        
                    }
				} else {
					LogError(LuaEventModule) <<" call OnTouchLeave on released script" ;
				}
			}
            return ;
		}
		static int constructor_func(lua_State* L) {
			if (!lua_istable(L,2)) {
				char buf[128];
				::snprintf(buf,127,"table expected, got %s",luaL_typename(L, 2));
				luaL_argerror(L, 2, buf);
				return 0;
			}
 			Lua* lua = Lua::GetPtr(L);
			lua_State* main_state = lua->GetVM();
			shared_ptr<LuaWidget> e = shared_ptr<LuaWidget>(new LuaWidget(lua->GetHelper()));
			lua_pushvalue(L, 2);
			if (main_state!=L) {
				lua_xmove(L, main_state, 1);
			}
			e->SetObject(main_state);
			Bind::StackHelper hpr(L,0,"Sandbox::LuaWidget");
			hpr.PushValue(e);
			return 1;
		}
	private:
		LuaReference	m_ref;
	};
	
	void Lua::RegisterSandboxObjects() {
		SB_BIND_BEGIN_BIND
		{
			SB_BIND_BEGIN_RAW_CLASS(Sandbox::Recti)
			SB_BIND_RAW_CONSTRUCTOR(Sandbox::Recti,(int,int,int,int))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RAW(Sandbox::Recti,x,int)
			SB_BIND_PROPERTY_RAW(Sandbox::Recti,y,int)
			SB_BIND_PROPERTY_RAW(Sandbox::Recti,w,int)
			SB_BIND_PROPERTY_RAW(Sandbox::Recti,h,int)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_RAW_CLASS(Sandbox::Vector2f)
			SB_BIND_RAW_CONSTRUCTOR(Sandbox::Vector2f,(float,float))
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Vector2f,length,float())
			SB_BIND_METHOD(Sandbox::Vector2f,unit,Sandbox::Vector2f())
			SB_BIND_METHOD(Sandbox::Vector2f,normal,Sandbox::Vector2f())
			SB_BIND_OPERATOR_ADD_(Sandbox::Vector2f,Sandbox::Vector2f(Sandbox::Vector2f),Sandbox::Vector2f(Sandbox::Vector2f::*)(const Sandbox::Vector2f&)const)
			SB_BIND_OPERATOR_SUB_(Sandbox::Vector2f,Sandbox::Vector2f(Sandbox::Vector2f),Sandbox::Vector2f(Sandbox::Vector2f::*)(const Sandbox::Vector2f&)const)
			SB_BIND_OPERATOR_MUL_(Sandbox::Vector2f,Sandbox::Vector2f(float),Sandbox::Vector2f(Sandbox::Vector2f::*)(float)const)
			SB_BIND_END_METHODS
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RAW(Sandbox::Vector2f,x,float)
			SB_BIND_PROPERTY_RAW(Sandbox::Vector2f,y,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
        {
			SB_BIND_BEGIN_RAW_CLASS(Sandbox::Vector2f)
			SB_BIND_RAW_CONSTRUCTOR(Sandbox::Vector2f,(float,float))
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Vector2f,length,float())
			SB_BIND_METHOD(Sandbox::Vector2f,unit,Sandbox::Vector2f())
			SB_BIND_METHOD(Sandbox::Vector2f,normal,Sandbox::Vector2f())
			SB_BIND_OPERATOR_ADD_(Sandbox::Vector2f,Sandbox::Vector2f(Sandbox::Vector2f),Sandbox::Vector2f(Sandbox::Vector2f::*)(const Sandbox::Vector2f&)const)
			SB_BIND_OPERATOR_SUB_(Sandbox::Vector2f,Sandbox::Vector2f(Sandbox::Vector2f),Sandbox::Vector2f(Sandbox::Vector2f::*)(const Sandbox::Vector2f&)const)
			SB_BIND_OPERATOR_MUL_(Sandbox::Vector2f,Sandbox::Vector2f(float),Sandbox::Vector2f(Sandbox::Vector2f::*)(float)const)
			SB_BIND_END_METHODS
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RAW(Sandbox::Vector2f,x,float)
			SB_BIND_PROPERTY_RAW(Sandbox::Vector2f,y,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_RAW_CLASS(Sandbox::Rectf)
			SB_BIND_RAW_CONSTRUCTOR(Sandbox::Rectf,(float,float,float,float))
			SB_BIND_BEGIN_METHODS
			SB_BIND_END_METHODS
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RAW(Sandbox::Rectf,x,float)
			SB_BIND_PROPERTY_RAW(Sandbox::Rectf,y,float)
			SB_BIND_PROPERTY_RAW(Sandbox::Rectf,w,float)
            SB_BIND_PROPERTY_RAW(Sandbox::Rectf,h,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_RAW_CLASS(Sandbox::Matrix4f)
			SB_BIND_RAW_CONSTRUCTOR(Sandbox::Matrix4f,())
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Matrix4f,get,float(size_t,size_t))
			SB_BIND_METHOD(Sandbox::Matrix4f,set,void(size_t,size_t,float))
			SB_BIND_METHOD(Sandbox::Matrix4f,inverted,Sandbox::Matrix4f())
			SB_BIND_METHOD(Sandbox::Matrix4f,transform_point,Sandbox::Vector3f(Sandbox::Vector3f))
			SB_BIND_OPERATOR_MUL(Sandbox::Matrix4f,mul,Sandbox::Matrix4f(Sandbox::Matrix4f))
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_RAW_CLASS(Sandbox::Color)
			SB_BIND_RAW_CONSTRUCTOR(Sandbox::Color,(float,float,float,float))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RAW(Sandbox::Color,r,float)
			SB_BIND_PROPERTY_RAW(Sandbox::Color,g,float)
			SB_BIND_PROPERTY_RAW(Sandbox::Color,b,float)
			SB_BIND_PROPERTY_RAW(Sandbox::Color,a,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
			lua_pushcclosure(GetVM(), &lua_color_from_string_func, 0);
			register_object("Sandbox.Color.FromString");
		}
		{
			
			SB_BIND_BEGIN_RAW_CLASS(Sandbox::Vector2fList)
			SB_BIND_RAW_CONSTRUCTOR(Sandbox::Vector2fList,())
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Vector2fList,reserve,void(size_t))
			SB_BIND_METHOD(Sandbox::Vector2fList,size,size_t())
			SB_BIND_METHOD(Sandbox::Vector2fList,add,void(Sandbox::Vector2f))
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_CLASS(Sandbox::Texture)
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Texture,SetFiltered,void(bool))
			SB_BIND_END_METHODS
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_WO(Sandbox::Texture,Filtered,SetFiltered,bool)
            SB_BIND_PROPERTY_WO(Sandbox::Texture,Tiled,SetTiled,bool)
			SB_BIND_PROPERTY_RO(Sandbox::Texture,Width,GetWidth,float)
			SB_BIND_PROPERTY_RO(Sandbox::Texture,Height,GetHeight,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_CLASS(Sandbox::ShaderFloatUniform)
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::ShaderFloatUniform,SetValue,void(float))
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_CLASS(Sandbox::Shader)
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Shader,GetFloatUniform,Sandbox::ShaderFloatUniform(const char*))
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_CLASS(Sandbox::Image)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::Image,(Sandbox::Texture,float,float,float,float),(Sandbox::TexturePtr,float,float,float,float))
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Image,SetSize,void(float,float))
			SB_BIND_END_METHODS
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RO(Sandbox::Image,TextureX,GetTextureX,float)
			SB_BIND_PROPERTY_RO(Sandbox::Image,TextureY,GetTextureY,float)
			SB_BIND_PROPERTY_RO(Sandbox::Image,TextureW,GetTextureW,float)
			SB_BIND_PROPERTY_RO(Sandbox::Image,TextureH,GetTextureH,float)
			SB_BIND_PROPERTY_RO(Sandbox::Image,Width,GetWidth,float)
			SB_BIND_PROPERTY_RO(Sandbox::Image,Height,GetHeight,float)
			SB_BIND_PROPERTY_RO(Sandbox::Image,Texture,GetTexture,Sandbox::Texture)
			SB_BIND_PROPERTY_RW(Sandbox::Image,Hotspot,GetHotspot,SetHotspot,Sandbox::Vector2f)
            SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_CLASS(Sandbox::Font)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::Font,())
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Font,Reserve,void(size_t))
			SB_BIND_METHOD(Sandbox::Font,AddGlypth,void(Sandbox::Image,const char*,float))
			SB_BIND_METHOD(Sandbox::Font,AddKerningPair,void(const char*,const char*,float))
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
			
		}
		{
			SB_BIND_BEGIN_ENUM(Sandbox::FontAlign)
			SB_BIND_ENUM_ITEM(ALIGN_LEFT,Sandbox)
			SB_BIND_ENUM_ITEM(ALIGN_CENTER,Sandbox)
			SB_BIND_ENUM_ITEM(ALIGN_RIGHT,Sandbox)
			SB_BIND_END_ENUM
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_CLASS(Sandbox::Event)
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Event,Emmit,void())
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::LuaEvent,Sandbox::Event)
			SB_BIND_END_CLASS
			SB_BIND(this);
			get_table("Sandbox.LuaEvent.");
			sb_assert(lua_istable(m_state,-1));
			luaL_getmetatable(m_state, "Sandbox::LuaEvent");
			sb_assert(lua_istable(m_state,-1));
			lua_pushcclosure(m_state,&LuaEvent::constructor_func, 1);
			Bind::StackHelper::rawsetfield(m_state, -2, "__call");
			lua_pop(m_state, 1);
		}
		
		{
			SB_BIND_BEGIN_SHARED_CLASS(Sandbox::Object)
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::Object,Visible,GetVisible,SetVisible,bool)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::Sprite,Sandbox::Object)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::Sprite,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::Sprite,Image,GetImage,SetImage,Sandbox::Image)
			SB_BIND_PROPERTY_RW(Sandbox::Sprite,Pos,GetPos,SetPos,Sandbox::Vector2f)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
        {
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ColorizedSprite,Sandbox::Sprite)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::ColorizedSprite,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ColorizedSprite,Color,GetColor,SetColor,Sandbox::Color)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
        {
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ClearScene,Sandbox::Object)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::ClearScene,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ClearScene,Color,GetColor,SetColor,Sandbox::Color)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
        {
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::FillRect,Sandbox::Object)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::FillRect,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::FillRect,Texture,GetTexture,SetTexture,Sandbox::Texture)
            SB_BIND_PROPERTY_RW(Sandbox::FillRect,Rect,GetRect,SetRect,Sandbox::Rectf)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::Background,Sandbox::Object)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::Background,())
			SB_BIND_BEGIN_METHODS
            SB_BIND_METHOD_(Sandbox::Background, Load, bool(const char*,Sandbox::Resources),&Sandbox::Background::Load)
            SB_BIND_END_METHODS
            SB_BIND_END_CLASS
			SB_BIND(this)
		}
        {
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::Label,Sandbox::Object)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::Label,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::Label,Font,GetFont,SetFont,Sandbox::Font)
			SB_BIND_PROPERTY_RW(Sandbox::Label,Pos,GetPos,SetPos,Sandbox::Vector2f)
            SB_BIND_PROPERTY_RW(Sandbox::Label,Text,GetText,SetText,const char*)
            SB_BIND_PROPERTY_RW(Sandbox::Label,Align,GetAlign,SetAlign,Sandbox::FontAlign)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::Container,Sandbox::Object)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::Container,())
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Container,Reserve,void(size_t))
			SB_BIND_METHOD(Sandbox::Container,AddObject,void(Sandbox::Object))
			SB_BIND_METHOD(Sandbox::Container,RemoveObject,void(Sandbox::Object))
			SB_BIND_METHOD(Sandbox::Container,Clear,void())
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_ENUM(Sandbox::BlendMode)
			SB_BIND_ENUM_ITEM(BLEND_MODE_COPY,Sandbox)
			SB_BIND_ENUM_ITEM(BLEND_MODE_ALPHABLEND,Sandbox)
			SB_BIND_ENUM_ITEM(BLEND_MODE_ADDITIVE,Sandbox)
			SB_BIND_ENUM_ITEM(BLEND_MODE_ADDITIVE_ALPHA,Sandbox)
			SB_BIND_ENUM_ITEM(BLEND_MODE_SCREEN,Sandbox)
			SB_BIND_END_ENUM
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ContainerBlend,Sandbox::Container)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::ContainerBlend,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ContainerBlend,Mode,GetMode,SetMode,Sandbox::BlendMode)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ContainerVisible,Sandbox::Container)
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ContainerVisible,Invert,GetInvert,SetInvert,bool)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ContainerShader,Sandbox::Container)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::ContainerShader,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ContainerShader,Shader,GetShader,SetShader,Sandbox::Shader)
			SB_BIND_PROPERTY_RW(Sandbox::ContainerShader,Enabled,GetEnabled,SetEnabled,bool)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ContainerTransform,Sandbox::Container)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::ContainerTransform,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ContainerTransform,Translate,GetTranslate,SetTranslate,Sandbox::Vector2f)
			SB_BIND_PROPERTY_WO(Sandbox::ContainerTransform,Scale,SetScale,float)
			SB_BIND_PROPERTY_RW(Sandbox::ContainerTransform,ScaleX,GetScaleX,SetScaleX,float)
			SB_BIND_PROPERTY_RW(Sandbox::ContainerTransform,ScaleY,GetScaleY,SetScaleY,float)
			SB_BIND_PROPERTY_RW(Sandbox::ContainerTransform,Angle,GetAngle,SetAngle,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ContainerTransform3d,Sandbox::Container)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::ContainerTransform3d,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ContainerTransform3d,ProjectionMatrix,GetProjectionMatrix,SetProjectionMatrix,Sandbox::Matrix4f)
			SB_BIND_PROPERTY_RW(Sandbox::ContainerTransform3d,ViewMatrix,GetViewMatrix,SetViewMatrix,Sandbox::Matrix4f)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ContainerViewport,Sandbox::Container)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::ContainerViewport,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ContainerViewport,Viewport,Get,Set,Sandbox::Recti)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ContainerColor,Sandbox::Container)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::ContainerColor,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ContainerColor,Color,GetColor,SetColor,Sandbox::Color)
			SB_BIND_PROPERTY_RW(Sandbox::ContainerColor,Alpha,GetAlpha,SetAlpha,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_EXTERN_CLASS(Sandbox::Scene)
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::Scene,Root,GetRoot,SetRoot,Sandbox::Object)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_EXTERN_CLASS(Sandbox::Resources)
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Resources,GetTexture,Sandbox::Texture(const char*))
			SB_BIND_METHOD(Sandbox::Resources,GetImage,Sandbox::Image(const char*))
			SB_BIND_METHOD(Sandbox::Resources,GetShader,Sandbox::Shader(const char*,const char*))
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_CLASS(Sandbox::Thread)
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_CLASS(Sandbox::AnimationData)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::AnimationData,())
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::AnimationData,Reserve,void(size_t))
			SB_BIND_METHOD(Sandbox::AnimationData,AddFrame,void(Sandbox::Image))
			SB_BIND_METHOD(Sandbox::AnimationData,GetImage,Sandbox::Image(size_t))
			SB_BIND_END_METHODS
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::AnimationData,Speed,GetSpeed,SetSpeed,float)
			SB_BIND_PROPERTY_RW(Sandbox::AnimationData,LoopFrame,GetLoopFrame,SetLoopFrame,size_t)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::Animation,Sandbox::Thread)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::Animation,(Sandbox::AnimationData),(const Sandbox::AnimationDataPtr&))
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Animation,Start,void(bool))
			SB_BIND_METHOD(Sandbox::Animation,Stop,void())
			SB_BIND_METHOD(Sandbox::Animation,AddSprite,void(Sandbox::Sprite))
			SB_BIND_METHOD(Sandbox::Animation,ClearSprites,void())
			SB_BIND_END_METHODS
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RO(Sandbox::Animation,Complete,Complete,bool)
			SB_BIND_PROPERTY_RO(Sandbox::Animation,PlayedOnce,PlayedOnce,bool)
			SB_BIND_PROPERTY_WO(Sandbox::Animation,EndEvent,SetEndEvent,Sandbox::Event)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::LuaThread,Sandbox::Thread)
			SB_BIND_END_CLASS
			SB_BIND(this)
			get_table("Sandbox.LuaThread.");
			sb_assert(lua_istable(m_state,-1));
			luaL_getmetatable(m_state, "Sandbox::LuaThread");
			sb_assert(lua_istable(m_state,-1));
			lua_pushcclosure(m_state,&LuaThread::constructor_func, 1);
			Bind::StackHelper::rawsetfield(m_state, -2, "__call");
			lua_pop(m_state, 1);
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ThreadsMgr,Sandbox::Thread)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::ThreadsMgr,())
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::ThreadsMgr,AddThread,void(Sandbox::Thread))
			SB_BIND_METHOD(Sandbox::ThreadsMgr,RemoveThread,void(Sandbox::Thread))
			SB_BIND_METHOD(Sandbox::ThreadsMgr,Clear,void())
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::Processor,Sandbox::Thread)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::Processor,())
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Processor,Start,void())
			SB_BIND_METHOD(Sandbox::Processor,Stop,void())
			SB_BIND_END_METHODS
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::Processor,Controller,GetController,SetController,Sandbox::Controller)
			SB_BIND_PROPERTY_WO(Sandbox::Processor,EndEvent,SetEndEvent,Sandbox::Event)
			SB_BIND_PROPERTY_RW(Sandbox::Processor,Time,GetTime,SetTime,float)
			SB_BIND_PROPERTY_RW(Sandbox::Processor,Loop,GetLoop,SetLoop,bool)
			SB_BIND_PROPERTY_RW(Sandbox::Processor,Inverted,GetInverted,SetInverted,bool)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_CLASS(Sandbox::Controller)
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::Controller,Set,void(float))
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerSplit,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::ControllerSplit,())
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::ControllerSplit,Reserve,void(size_t))
			SB_BIND_METHOD(Sandbox::ControllerSplit,AddController,void(Sandbox::Controller))
			SB_BIND_METHOD(Sandbox::ControllerSplit,RemoveController,void(Sandbox::Controller))
			SB_BIND_METHOD(Sandbox::ControllerSplit,Clear,void())
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerBidirect,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerBidirect,(Sandbox::Controller),(const Sandbox::ControllerPtr&))
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerPhase,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerPhase,(Sandbox::Controller),(const Sandbox::ControllerPtr&))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ControllerPhase,Phase,GetPhase,SetPhase,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerElastic,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerElastic,(Sandbox::Controller),(const Sandbox::ControllerPtr&))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ControllerElastic,Hard,GetHard,SetHard,float)
			SB_BIND_PROPERTY_RW(Sandbox::ControllerElastic,End,GetEnd,SetEnd,float)
			SB_BIND_PROPERTY_RW(Sandbox::ControllerElastic,Amplitude,GetAmplitude,SetAmplitude,float)
			SB_BIND_PROPERTY_RW(Sandbox::ControllerElastic,Phases,GetPhases,SetPhases,size_t)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerMap,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerMap,(Sandbox::Controller),(const Sandbox::ControllerPtr&))
			SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::ControllerMap,SetPoint,void(float,float))
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerTranslate,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerTranslate,(Sandbox::ContainerTransform),(const Sandbox::ContainerTransformPtr&))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ControllerTranslate,Begin,GetBegin,SetBegin,Sandbox::Vector2f)
			SB_BIND_PROPERTY_RW(Sandbox::ControllerTranslate,End,GetEnd,SetEnd,Sandbox::Vector2f)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerScale,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerScale,(Sandbox::ContainerTransform),(const Sandbox::ContainerTransformPtr&))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ControllerScale,Begin,GetBegin,SetBegin,float)
			SB_BIND_PROPERTY_RW(Sandbox::ControllerScale,End,GetEnd,SetEnd,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerScaleX,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerScaleX,(Sandbox::ContainerTransform),(const Sandbox::ContainerTransformPtr&))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ControllerScaleX,Begin,GetBegin,SetBegin,float)
			SB_BIND_PROPERTY_RW(Sandbox::ControllerScaleX,End,GetEnd,SetEnd,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerScaleY,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerScaleY,(Sandbox::ContainerTransform),(const Sandbox::ContainerTransformPtr&))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ControllerScaleY,Begin,GetBegin,SetBegin,float)
			SB_BIND_PROPERTY_RW(Sandbox::ControllerScaleY,End,GetEnd,SetEnd,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerAngle,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerAngle,(Sandbox::ContainerTransform),(const Sandbox::ContainerTransformPtr&))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ControllerAngle,Begin,GetBegin,SetBegin,float)
			SB_BIND_PROPERTY_RW(Sandbox::ControllerAngle,End,GetEnd,SetEnd,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerColor,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerColor,(Sandbox::ContainerColor),(const Sandbox::ContainerColorPtr&))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ControllerColor,Begin,GetBegin,SetBegin,Sandbox::Color)
			SB_BIND_PROPERTY_RW(Sandbox::ControllerColor,End,GetEnd,SetEnd,Sandbox::Color)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerAlpha,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerAlpha,(Sandbox::ContainerColor),(const Sandbox::ContainerColorPtr&))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ControllerAlpha,Begin,GetBegin,SetBegin,float)
			SB_BIND_PROPERTY_RW(Sandbox::ControllerAlpha,End,GetEnd,SetEnd,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		{
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ControllerShaderFloatUniform,Sandbox::Controller)
			SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ControllerShaderFloatUniform,(Sandbox::ShaderFloatUniform),(const shared_ptr<Sandbox::ShaderFloatUniform>&))
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::ControllerShaderFloatUniform,Begin,GetBegin,SetBegin,float)
			SB_BIND_PROPERTY_RW(Sandbox::ControllerShaderFloatUniform,End,GetEnd,SetEnd,float)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
        {
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::Widget,Sandbox::Container)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::Widget,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RW(Sandbox::Widget,Rect,GetRect,SetRect,Sandbox::Rectf)
            SB_BIND_PROPERTY_RW(Sandbox::Widget,Active,GetActive,SetActive,bool)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
        {
			SB_BIND_BEGIN_ENUM(Sandbox::TouchInfo::Type)
			SB_BIND_ENUM_ITEM(BEGIN,Sandbox::TouchInfo)
			SB_BIND_ENUM_ITEM(MOVE,Sandbox::TouchInfo)
			SB_BIND_ENUM_ITEM(END,Sandbox::TouchInfo)
			SB_BIND_END_ENUM
			SB_BIND(this)
		}
        {
			SB_BIND_BEGIN_RAW_CLASS(Sandbox::TouchInfo)
			SB_BIND_BEGIN_METHODS
			SB_BIND_END_METHODS
            SB_BIND_BEGIN_PROPERTYS
            SB_BIND_PROPERTY_RO(Sandbox::TouchInfo, Type, GetType, Sandbox::TouchInfo::Type)
            SB_BIND_PROPERTY_RO(Sandbox::TouchInfo, Position, GetPosition, Sandbox::Vector2f)
            SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
        {
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::LuaWidget,Sandbox::Widget)
			SB_BIND_END_CLASS
			SB_BIND(this)
			get_table("Sandbox.LuaWidget.");
			sb_assert(lua_istable(m_state,-1));
			luaL_getmetatable(m_state, "Sandbox::LuaWidget");
			sb_assert(lua_istable(m_state,-1));
			lua_pushcclosure(m_state,&LuaWidget::constructor_func, 1);
			Bind::StackHelper::rawsetfield(m_state, -2, "__call");
			lua_pop(m_state, 1);
		}
        {
			SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::TouchButtonWidget,Sandbox::Widget)
			SB_BIND_SHARED_CONSTRUCTOR(Sandbox::TouchButtonWidget,())
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_WO(Sandbox::TouchButtonWidget,ActivateEvent,SetActivateEvent,Sandbox::Event)
            SB_BIND_PROPERTY_WO(Sandbox::TouchButtonWidget,DeactivateEvent,SetDeactivateEvent,Sandbox::Event)
            SB_BIND_PROPERTY_WO(Sandbox::TouchButtonWidget,TouchUpInsideEvent,SetTouchUpInsideEvent,Sandbox::Event)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(this)
		}
		SB_BIND_END_BIND
        ParticlesSystem::Bind(this);
	}
}
