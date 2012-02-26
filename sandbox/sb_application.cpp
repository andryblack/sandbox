/*
 *  sb_application.cpp
 *  YinYang
 *
 *  Created by Андрей Куницын on 04.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */

#include "sb_application.h"
#include "sb_bind.h"
#include "sb_touch_info.h"

#include <ghl_settings.h>
#include <ghl_vfs.h>
#include <ghl_sound.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

namespace Sandbox {
	
    static void format_memory( char* buf, size_t size, GHL::UInt32 mem,const char* caption ) {
        if ( mem > 1024*1024 ) {
            ::snprintf(buf, size, "%s:%0.2fM", caption,float(mem)/(1024*1024));
        } else if ( mem > 1024 ) {
            ::snprintf(buf, size, "%s:%0.2fK", caption,float(mem)/(1024));
        }
        else {
            ::snprintf(buf, size, "%s:%0.2fb", caption,float(mem));
        }
    }
    
	Application::Application() {
		m_system = 0;
		m_vfs = 0;
		m_render = 0;
		m_image_decoder = 0;
		m_sound = 0;
		m_graphics = 0;
		m_resources = 0;
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
		delete m_resources;
		delete m_graphics;
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
		
        
        m_lua = new Lua(m_resources);
        
		base_path=m_lua_base_path;
		if (!base_path.empty() && base_path[base_path.size()-1]!='/')
			base_path+="/";
		m_lua->SetBasePath(base_path.c_str());
		
        SB_BIND_BEGIN_BIND
		{
			SB_BIND_BEGIN_RAW_CLASS( GHL::Settings )
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RAW(GHL::Settings,width,GHL::UInt32)
			SB_BIND_PROPERTY_RAW(GHL::Settings,height,GHL::UInt32)
			SB_BIND_PROPERTY_RAW(GHL::Settings,fullscreen,bool)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(m_lua)
		}
		{
			SB_BIND_BEGIN_EXTERN_CLASS( Sandbox::Application )
			SB_BIND_BEGIN_PROPERTYS
            SB_BIND_PROPERTY_RW(Sandbox::Application, SoundEnabled, GetSoundEnabled, SetSoundEnabled, bool)
            SB_BIND_PROPERTY_RW(Sandbox::Application, MusicEnabled, GetMusicEnabled, SetMusicEnabled, bool)
            SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(m_lua)
		}
		SB_BIND_END_BIND
		
		m_lua->SetValue(settings, "settings", "GHL::Settings");
		m_lua->SetValue(this, "application.app", "Sandbox::Application");
        
#ifdef GHL_PLATFORM_IOS
        m_lua->SetValue("iOS", "platform.os", "const char*");
#endif

#ifdef GHL_PLATFORM_MAC
        m_lua->SetValue("OSX", "platform.os", "const char*");
#endif

#ifdef GHL_PLATFORM_WIN
        m_lua->SetValue("WIN32", "platform.os", "const char*");
#endif
		BindModules( m_lua );
		m_lua->DoFile("settings.lua");
	}
	///
	bool GHL_CALL Application::Load() {
		ConfigureDevice( m_system );
		m_graphics = new Graphics();
		m_resources->Init(m_render, m_image_decoder);
        m_lua->SetValue(m_resources, "application.resources", "Sandbox::Resources");
		m_lua->DoFile("load.lua");
		if (!LoadResources(*m_resources))
			return false;
		m_main_scene = new Scene();
		m_lua->SetValue(m_main_scene, "application.scene", "Sandbox::Scene");
		m_main_thread = new ThreadsMgr();
		m_lua->SetValue(m_main_thread, "application.thread", "Sandbox::ThreadsMgr");
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
							m_clear_color.a);
		m_graphics->BeginScene(m_render);
		DrawFrame(*m_graphics);
		size_t batches = m_graphics->EndScene();
        m_batches = m_batches * 0.875f + 0.125f*batches;    /// interpolate 4 frames
		DrawDebugInfo();
		m_render->EndScene();
		return true;
	}
	
	void Application::DrawScene() const {
		m_main_scene->Draw(*m_graphics);
	}
	
	void Application::DrawFrame(Graphics& g) const {
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
        format_memory(buf,128,m_lua->GetMemoryUsed(),"lua");
        m_render->DebugDrawText( 10, 21 , buf );
        ::snprintf(buf,128,"batches:%0.2f",m_batches);
        m_render->DebugDrawText( 10, 32 , buf );
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
            if (m_music_enabled)
                m_sound->Music_Play(true);
            else
                m_sound->Music_Stop();
        }
    }
    
    bool Application::GetMusicEnabled() const {
        return m_music_enabled;
    }
    
	///
	void GHL_CALL Application::OnKeyDown( GHL::Key key ) {
	}
	///
	void GHL_CALL Application::OnKeyUp( GHL::Key key ) {
	}
	///
	void GHL_CALL Application::OnChar( GHL::UInt32 ch ) {
	}
	///
	void GHL_CALL Application::OnMouseDown( GHL::MouseButton btn, GHL::Int32 x, GHL::Int32 y) {
        m_main_scene->HandleTouch( TouchInfo(TouchInfo::BEGIN,Vector2f(float(x),float(y))) );
	}
	///
	void GHL_CALL Application::OnMouseMove( GHL::MouseButton btn, GHL::Int32 x, GHL::Int32 y) {
        m_main_scene->HandleTouch( TouchInfo(TouchInfo::MOVE,Vector2f(float(x),float(y))) );
    }
	///
	void GHL_CALL Application::OnMouseUp( GHL::MouseButton btn, GHL::Int32 x, GHL::Int32 y) {
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
