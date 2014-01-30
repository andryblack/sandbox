/*
 *  sb_application.cpp
 *  YinYang
 *
 *  Created by Андрей Куницын on 04.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */

#include "sb_application.h"
#include "sb_sound.h"
#include "sb_resources.h"
#include "sb_graphics.h"
#include <sbstd/sb_algorithm.h>
#include "sb_lua_context.h"

#include "luabind/sb_luabind.h"

#include <ghl_settings.h>
#include <ghl_vfs.h>
#include <ghl_sound.h>
#include <ghl_render.h>
#include <ghl_system.h>

#include "MyGUI_Gui.h"
#include "MyGUI_InputManager.h"

#include "mygui/sb_mygui_keys.h"
#include "mygui/sb_mygui_data_manager.h"
#include "mygui/sb_mygui_render.h"
#include "mygui/sb_mygui_skin.h"
#include "mygui/widgets/sb_mygui_widgets.h"

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
SB_META_METHOD(AddScene)
SB_META_METHOD(RemoveScene)
SB_META_END_KLASS_BIND()


bool (*sb_terminate_handler)() = 0;

namespace Sandbox {
    
    void register_math( lua_State* lua );
    void register_resources( lua_State* lua );
	void register_scene( lua_State* lua );
	void register_controller( lua_State* lua );
	
    namespace mygui {
        void register_mygui( lua_State* lua );
        void setup_singletons( LuaVM* lua );
    }
    
    
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
        m_gui_data_manager = 0;
        m_gui = 0;
        m_gui_render = 0;
        SetResourcesBasePath("data");
        SetLuaBasePath("scripts");
	}
	
	Application::~Application() {
        delete m_main_thread;
		delete m_main_scene;
		delete m_lua;
    	delete m_sound_mgr;
        if (m_gui) {
            mygui::unregister_skin();
            mygui::unregister_widgets();
            m_gui->shutdown();
        }
        delete m_gui;
        delete m_gui_render;
        delete m_gui_data_manager;
		delete m_resources;
		delete m_graphics;
  	}
    
    void Application::BindModules( LuaVM* lua) {
        register_math(lua->GetVM());
        register_resources(lua->GetVM());
        register_scene(lua->GetVM());
        register_controller(lua->GetVM());
        mygui::register_mygui(lua->GetVM());
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
		
        LuaContextPtr ctx = m_lua->GetGlobalContext();
        
        luabind::ExternClass<Sandbox::Application>(m_lua->GetVM());
        ctx->SetValue("application.app", this);
        luabind::RawClass<GHL::Settings>(m_lua->GetVM());
        ctx->SetValue("settings", settings);
		
#ifdef GHL_PLATFORM_IOS
        ctx->SetValue("platform.os", "iOS");
#endif

#ifdef GHL_PLATFORM_MAC
        ctx->SetValue("platform.os", "OSX");
#endif

#ifdef GHL_PLATFORM_WIN
        ctx->SetValue("platform.os", "WIN32");
#endif
    
#ifdef GHL_PLATFORM_FLASH
        ctx->SetValue("platform.os", "FLASH");
#endif
        
		BindModules( m_lua );
		m_lua->DoFile("settings.lua");
        ctx->SetValue("settings", (GHL::Settings*)(0));
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
        
        LuaContextPtr ctx = m_lua->GetGlobalContext();
        
        ctx->SetValue("application.resources", m_resources);
        ctx->SetValue("application.sound", m_sound_mgr);

		m_lua->DoFile("load.lua");
        
        m_gui_data_manager = new mygui::DataManager(m_resources);
        m_gui_render = new mygui::RenderManager(m_resources);
        
        m_gui = new MyGUI::Gui();
        m_gui->initialise("");
        mygui::register_skin();
        mygui::register_widgets();
        mygui::setup_singletons(m_lua);
        
		if (!LoadResources(*m_resources))
			return false;
        
		m_main_scene = new Scene();
        ctx->SetValue("application.scene", m_main_scene);
		m_main_thread = new ThreadsMgr();
        ctx->SetValue("application.thread", m_main_thread);
        ctx->SetValue("application.size.width", m_render->GetWidth() );
        ctx->SetValue("application.size.height", m_render->GetHeight() );
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
        
        if (m_gui_render)
            m_gui_render->timerFrame(dt);
        
        // update targets
        for (sb::list<RTScenePtr>::const_iterator it = m_rt_scenes.begin();it!=m_rt_scenes.end();++it) {
            (*it)->Update(dt);
        }
        
        for (sb::list<RTScenePtr>::const_iterator it = m_rt_scenes.begin();it!=m_rt_scenes.end();++it) {
            (*it)->Draw(m_render, *m_graphics);
        }
        
		m_render->BeginScene(0);
		if (m_clear_buffer)
			m_render->Clear(m_clear_color.r,
							m_clear_color.g,
							m_clear_color.b,
							m_clear_color.a,0);
		m_graphics->BeginScene(m_render);
		DrawFrame(*m_graphics);
        m_graphics->SetBlendMode(BLEND_MODE_ALPHABLEND);
       
        size_t batches = m_graphics->EndScene();
        m_batches = m_batches * 0.875f + 0.125f*batches;    /// interpolate 4 frames
        m_render->SetupBlend(true,GHL::BLEND_FACTOR_SRC_ALPHA,GHL::BLEND_FACTOR_SRC_ALPHA_INV);
        
        if (m_gui_render)
            m_gui_render->drawFrame();
        
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
        m_render->DebugDrawText( 10, 43,
                                (sb::string("res:")+format_memory(m_resources->GetMemoryUsed())+
                                 sb::string("/")+format_memory(m_resources->GetMemoryLimit())).c_str());
        size_t render_mem = m_render->GetMemoryUsage();
        if (render_mem)
            m_render->DebugDrawText( 10, 54,
                                    (sb::string("tex:")+format_memory(render_mem)).c_str() );
	}
    
    void    Application::AddScene( const RTScenePtr& scene ) {
        m_rt_scenes.push_back(scene);
    }
    void    Application::RemoveScene( const RTScenePtr& scene ) {
        sb::list<RTScenePtr>::iterator it = sb::find(m_rt_scenes.begin(),m_rt_scenes.end(),scene);
        if (it!=m_rt_scenes.end()) {
            m_rt_scenes.erase(it);
        }
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
	void GHL_CALL Application::OnKeyDown( GHL::Key k ) {
        MyGUI::InputManager::getInstance().injectKeyPress(mygui::translate_key(k));
	}
	///
	void GHL_CALL Application::OnKeyUp( GHL::Key k) {
        MyGUI::InputManager::getInstance().injectKeyRelease(mygui::translate_key(k));
	}
	///
	void GHL_CALL Application::OnChar( GHL::UInt32 ) {
        
	}
	///
	void GHL_CALL Application::OnMouseDown( GHL::MouseButton key, GHL::Int32 x, GHL::Int32 y) {
        MyGUI::InputManager::getInstance().injectMousePress(x, y, mygui::translate_key(key));
    }
	///
	void GHL_CALL Application::OnMouseMove( GHL::MouseButton key, GHL::Int32 x, GHL::Int32 y) {
        MyGUI::InputManager::getInstance().injectMouseMove(x, y, 0);
    }
	///
	void GHL_CALL Application::OnMouseUp( GHL::MouseButton key, GHL::Int32 x, GHL::Int32 y) {
        MyGUI::InputManager::getInstance().injectMouseRelease(x, y, mygui::translate_key(key));
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
