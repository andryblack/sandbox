/*
 *  sb_application.cpp
 *  YinYang
 *
 *  Created by Андрей Куницын on 04.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */

#include "sb_application.h"
#include "sb_touch_info.h"
#include "sb_sound.h"
#include "sb_resources.h"
#include "sb_graphics.h"

#include "luabind/sb_luabind.h"

#include <ghl_settings.h>
#include <ghl_vfs.h>
#include <ghl_sound.h>
#include <ghl_render.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif


SB_META_DECLARE_KLASS(GHL::Settings, void)
SB_META_BEGIN_KLASS_BIND(GHL::Settings)
SB_META_PROPERTY(width)
SB_META_PROPERTY(height)
SB_META_PROPERTY(fullscreen)
SB_META_PROPERTY(depth)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Application, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Application)
SB_META_END_KLASS_BIND()


namespace Sandbox {
    
    void register_math( lua_State* lua );
    void register_resources( lua_State* lua );
	void register_scene( lua_State* lua );
	void register_controller( lua_State* lua );
	
    
    
	Application::Application() {
		m_system = 0;
		m_vfs = 0;
		m_render = 0;
		m_image_decoder = 0;
		m_sound = 0;
		m_graphics = 0;
		m_resources = 0;
        m_sound_mgr = new SoundManager();
		m_lua = 0;
		m_frames = 0;
		m_frames_time = 0;
		m_fps = 0.0f;
		m_main_scene = 0;
		m_main_thread = 0;
		m_clear_buffer = false;
        m_batches = 0.0f;
        m_sound_enabled = true;
        m_music_enabled = true;
        SetResourcesBasePath("data");
        SetLuaBasePath("scripts");
	}
	
	Application::~Application() {
		delete m_main_thread;
		delete m_main_scene;
		delete m_lua;
        delete m_sound_mgr;
		delete m_resources;
		delete m_graphics;
	}
    
    void Application::BindModules( LuaVM* lua) {
        register_math(lua->GetVM());
        register_resources(lua->GetVM());
        register_scene(lua->GetVM());
        register_controller(lua->GetVM());
    }
	
    void GHL_CALL Application::Initialize() {
        
    }
	///
	void GHL_CALL Application::SetSystem( GHL::System* sys ) {
		m_system = sys;
	}
	///
	void GHL_CALL Application::SetVFS( GHL::VFS* vfs ) {
		m_vfs = vfs;
	}
	///
	void GHL_CALL Application::SetRender( GHL::Render* render ) {
		m_render = render;
	}
	///
	void GHL_CALL Application::SetImageDecoder( GHL::ImageDecoder* decoder ) {
		m_image_decoder = decoder;
	}
	///
	void GHL_CALL Application::SetSound( GHL::Sound* sound) {
		m_sound = sound;
	}
	///
	void GHL_CALL Application::FillSettings( GHL::Settings* settings ) {
		sb_assert( m_vfs );
        m_resources = new Resources(m_vfs);
		
        std::string base_path = m_vfs->GetDir(GHL::DIR_TYPE_DATA);
		if (!base_path.empty() && base_path[base_path.size()-1]!='/')
			base_path+="/";
		base_path+=m_resources_base_path;
		if (!base_path.empty() && base_path[base_path.size()-1]!='/')
			base_path+="/";
		m_resources->SetBasePath(base_path.c_str());
		
        
        m_lua = new LuaVM(m_resources);
        
		base_path=m_lua_base_path;
		if (!base_path.empty() && base_path[base_path.size()-1]!='/')
			base_path+="/";
		m_lua->SetBasePath(base_path.c_str());
		
        luabind::ExternClass<Sandbox::Application>(m_lua->GetVM());
        luabind::SetValue(m_lua->GetVM(), "application.app", this);
        luabind::RawClass<GHL::Settings>(m_lua->GetVM());
        luabind::SetValue(m_lua->GetVM(), "settings", settings);
		
#ifdef GHL_PLATFORM_IOS
        luabind::SetValue(m_lua->GetVM(), "platform.os", "iOS");
#endif

#ifdef GHL_PLATFORM_MAC
        luabind::SetValue(m_lua->GetVM(), "platform.os", "OSX");
#endif

#ifdef GHL_PLATFORM_WIN
        luabind::SetValue(m_lua->GetVM(), "platform.os", "WIN32");
#endif
    
#ifdef GHL_PLATFORM_FLASH
        luabind::SetValue(m_lua->GetVM(), "platform.os", "FLASH");
#endif
        
		BindModules( m_lua );
		m_lua->DoFile("settings.lua");
        
        luabind::SetValue(m_lua->GetVM(), "settings", (GHL::Settings*)(0));
	}
	///
	bool GHL_CALL Application::Load() {
		ConfigureDevice( m_system );
		m_graphics = new Graphics(m_resources);
        sb_assert(m_resources);
        sb_assert(m_render);
        sb_assert(m_image_decoder);
		m_resources->Init(m_render, m_image_decoder);
        m_sound_mgr->Init(m_sound,m_resources);
        luabind::SetValue(m_lua->GetVM(), "application.resources", m_resources);
        luabind::SetValue(m_lua->GetVM(), "application.sound", m_sound_mgr);

		m_lua->DoFile("load.lua");
		if (!LoadResources(*m_resources))
			return false;
		m_main_scene = new Scene();
        luabind::SetValue(m_lua->GetVM(), "application.scene", m_main_scene);
		m_main_thread = new ThreadsMgr();
        luabind::SetValue(m_lua->GetVM(), "application.thread", m_main_thread);
        luabind::SetValue(m_lua->GetVM(), "application.size.width", m_render->GetWidth() );
        luabind::SetValue(m_lua->GetVM(), "application.size.height", m_render->GetHeight() );
        m_graphics->Load(m_render);
		OnLoaded();
		m_lua->DoFile("main.lua");
		return true;
	}
	///
	bool GHL_CALL Application::OnFrame( GHL::UInt32 usecs ) {
		m_frames++;
		m_frames_time+=usecs;
		if (m_frames_time>=1000000) {
			m_fps = float(m_frames*1000000)/m_frames_time;
			m_frames_time = 0;
			m_frames = 0;
		}
		if (usecs>100000)
			usecs=100000;
		float dt = float(usecs)/1000000.0f;
        if (m_main_scene)
            m_main_scene->Update(dt);
		m_main_thread->Update(dt);
		Update(dt);
		m_render->BeginScene(0);
		if (m_clear_buffer)
			m_render->Clear(m_clear_color.r,
							m_clear_color.g,
							m_clear_color.b,
							m_clear_color.a,0);
		m_graphics->BeginScene(m_render);
		DrawFrame(*m_graphics);
		size_t batches = m_graphics->EndScene();
        m_batches = m_batches * 0.875f + 0.125f*batches;    /// interpolate 4 frames
		DrawDebugInfo();
		m_render->EndScene();
        
        m_resources->ProcessMemoryMgmt();
        
		return true;
	}
	
	void Application::DrawScene() const {
		m_main_scene->Draw(*m_graphics);
	}
	
	void Application::DrawFrame(Graphics&) const {
		DrawScene();
	}
	
	void Application::DrawDebugInfo() {
		char buf[128];
		::snprintf(buf,128,"fps:%0.2f",m_fps);
		m_render->SetProjectionMatrix(Matrix4f::ortho(0.0f,
			float(m_render->GetWidth()),
			float(m_render->GetHeight())
			,0.0f,-1.0f,1.0f).matrix);
		m_render->DebugDrawText( 10, 10 , buf );
        m_render->DebugDrawText( 10, 21 , m_lua->GetMemoryUsed().c_str() );
        ::snprintf(buf,128,"batches:%0.2f",m_batches);
        m_render->DebugDrawText( 10, 32 , buf );
        m_render->DebugDrawText( 10, 43, (sb::string("tex:")+format_memory(m_render->GetTexturesMemory())).c_str());
	}
	
	void Application::SetClearColor(const Color& c) {
		m_clear_buffer = true;
		m_clear_color = c;
	}
    
    void Application::SetSoundEnabled( bool e ) {
        m_sound_enabled = e;
        if (m_sound) {
            /// @todo
        }
    }
    
    bool Application::GetSoundEnabled() const {
        return m_sound_enabled;
    }
    
    void Application::SetMusicEnabled( bool e ) {
        m_music_enabled = e;
        if (m_sound) {
            /*
            if (m_music_enabled)
                m_sound->Music_Play(true);
            else
                m_sound->Music_Stop();
             */
        }
    }
    
    bool Application::GetMusicEnabled() const {
        return m_music_enabled;
    }
    
	///
	void GHL_CALL Application::OnKeyDown( GHL::Key ) {
	}
	///
	void GHL_CALL Application::OnKeyUp( GHL::Key ) {
	}
	///
	void GHL_CALL Application::OnChar( GHL::UInt32 ) {
	}
	///
	void GHL_CALL Application::OnMouseDown( GHL::MouseButton, GHL::Int32 x, GHL::Int32 y) {
        m_main_scene->HandleTouch( TouchInfo(TouchInfo::BEGIN,Vector2f(float(x),float(y))) );
	}
	///
	void GHL_CALL Application::OnMouseMove( GHL::MouseButton, GHL::Int32 x, GHL::Int32 y) {
        m_main_scene->HandleTouch( TouchInfo(TouchInfo::MOVE,Vector2f(float(x),float(y))) );
    }
	///
	void GHL_CALL Application::OnMouseUp( GHL::MouseButton, GHL::Int32 x, GHL::Int32 y) {
        m_main_scene->HandleTouch( TouchInfo(TouchInfo::END,Vector2f(float(x),float(y))) );
    }
	///
	void GHL_CALL Application::OnDeactivated() {
	}
	///
	void GHL_CALL Application::OnActivated() {
	}
	///
	void GHL_CALL Application::Release(  ) {
		delete this;
	}
	
}
