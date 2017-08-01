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
#include "sb_extension.h"

#include "luabind/sb_luabind.h"

#include <ghl_settings.h>
#include <ghl_vfs.h>
#include <ghl_sound.h>
#include <ghl_render.h>
#include <ghl_system.h>
#include <ghl_keys.h>
#include <ghl_event.h>
#include <ghl_time.h>

#ifdef SB_USE_MYGUI
#include "mygui/sb_mygui_gui.h"
#include "MyGUI_InputManager.h"
#include "MyGUI_ClipboardManager.h"
#include "MyGUI_LanguageManager.h"
#include "MyGUI_EditBox.h"

#include "mygui/sb_mygui_keys.h"
#include "mygui/sb_mygui_data_manager.h"
#include "mygui/sb_mygui_render.h"
#endif

#ifdef SB_USE_NETWORK
#include "net/sb_network.h"
#include "net/sb_network_bind.h"
#endif

#include <sbstd/sb_platform.h>

#include "json/sb_json.h"
#include "sb_data.h"

#include "sb_particles.h"


SB_META_DECLARE_KLASS(GHL::Settings, void)
SB_META_BEGIN_KLASS_BIND(GHL::Settings)
SB_META_PROPERTY(width)
SB_META_PROPERTY(height)
SB_META_PROPERTY(fullscreen)
SB_META_PROPERTY(depth)
SB_META_PROPERTY(screen_dpi)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    static const char* MODULE = "app";
    
    int Application_CallExtension( lua_State* L ) {
        Application* app = luabind::stack<Application*>::get(L, 1, false);
        if (!app)
            return 0;
        const char* method = luabind::stack<const char*>::get(L, 2);
        if (!method)
            return 0;
        const char* args = luabind::stack<const char*>::get(L, 3);
        if (!args)
            return 0;
        SB_LOGD( "CallExtension: " << method << " " << args);
        sb::string res;
        if (app->CallExtension(method, args, res)) {
            luabind::stack<const char*>::push(L,res.c_str());
            return 1;
        }
        luabind::stack<const char*>::push(L, 0);
        luabind::stack<const char*>::push(L,res.c_str());
        return 2;
    }
    int Application_StoreProfileFile( lua_State* L ) {
        Application* app = luabind::stack<Application*>::get(L, 1, false);
        if (!app)
            return 0;
        const char* filename = luabind::stack<const char*>::get(L, 2);
        if (!filename)
            return 0;
        if (!lua_isstring(L, 3))
            return 0;
        size_t size = 0;
        const char* data = lua_tolstring(L, 3, &size);
        Sandbox::InlinedData dp(data,size);
        bool r = app->StoreProfileFile(filename, &dp);
        lua_pushboolean(L, r ? 1 : 0);
        return 1;
    }
    
    int Application_LoadProfileFile( lua_State* L) {
        Application* app = luabind::stack<Application*>::get(L, 1, false);
        if (!app)
            return 0;
        const char* filename = luabind::stack<const char*>::get(L, 2);
        if (!filename)
            return 0;
        GHL::Data* data = app->LoadProfileFile(filename);
        if (!data) {
            lua_pushnil(L);
            return 1;
        }
        lua_pushlstring(L, reinterpret_cast<const char*>(data->GetData()), data->GetSize());
        data->Release();
        return 1;
    }
}

SB_META_DECLARE_OBJECT(Sandbox::Application, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Application)
SB_META_METHOD(AddScene)
SB_META_METHOD(RemoveScene)
SB_META_METHOD(SetMouseContext)
SB_META_METHOD(SetKeyboardContext)
SB_META_METHOD(SetResourcesVariant)
SB_META_METHOD(SetRenderScale)
SB_META_METHOD(Restart)
SB_META_METHOD(RenderScreen)
SB_META_METHOD(OpenURL)
SB_META_METHOD(Exit)
SB_META_METHOD(ReportAppError)
SB_META_PROPERTY_RO(TimeUSec, GetTimeUSec)
SB_META_PROPERTY_RO(UTCOffset, GetUTCOffset)
SB_META_PROPERTY_RO(SystemLanguage, GetSystemLanguage)
SB_META_PROPERTY_WO(DrawDebugInfo,SetDrawDebugInfo)
SB_META_PROPERTY_WO(FrameInterval, SetFrameInterval)
SB_META_PROPERTY_WO(ResizeableWindow, SetResizeableWindow)
SB_META_PROPERTY_WO(ScreenKeepOn, SetScreenKeepOn)
SB_META_PROPERTY_RO(FPS, GetFPS)
bind( method( "CallExtension" , &Sandbox::Application_CallExtension ) );
bind( method( "StoreProfileFile", &Sandbox::Application_StoreProfileFile));
bind( method( "LoadProfileFile", &Sandbox::Application_LoadProfileFile));
SB_META_END_KLASS_BIND()


bool (*sb_terminate_handler)() = 0;
GHL::UInt32 sb_main_thread_id = 0;


namespace Sandbox {
    
    void register_math( lua_State* lua );
    void register_utils( lua_State* lua );
    void register_resources( lua_State* lua );
	void register_scene( lua_State* lua );
    void register_thread( lua_State* lua );
	void register_controller( lua_State* lua );
    void register_keys( lua_State* lua );
    void register_skelet( lua_State* lua );
    
#ifdef SB_USE_MYGUI
    namespace mygui {
        void register_mygui( lua_State* lua );
        
        void register_factory();
        void unregister_factory();
    }
#endif
 
    
    
	Application::Application() {

        sb_main_thread_id = GHL_GetCurrentThreadId();

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
        m_batches_rt = 0.0f;
        m_sound_enabled = true;
        m_music_enabled = true;
#ifdef SB_DEBUG
        m_draw_debug_info = true;
#else
        m_draw_debug_info = false;
#endif
        
#ifdef SB_USE_MYGUI
        m_gui_data_manager = 0;
        m_gui = 0;
        m_gui_render = 0;
#endif
#ifdef SB_USE_NETWORK
        m_network = 0;
#endif
        SetResourcesBasePath("data");
        SetLuaBasePath("scripts");
        m_sound_mgr->SetSoundsDir("sound");
        
        m_need_restart = false;
        m_need_exit = false;
        m_utc_offset = 0;
        
        //const char* test = "{\"val\":41,\"id\":\"CgkInqr15dUFEAIQAw\"}";
        //sb::map<sb::string,sb::string> res;
        //json_parse(test, res);
	}
	
	Application::~Application() {
        delete m_main_thread;
		delete m_main_scene;
		
        DestroyLua();
    	delete m_sound_mgr;

        ReleaseGUI();
        
#ifdef SB_USE_NETWORK
        delete  m_network;
#endif
		delete m_resources;
		delete m_graphics;
  	}
    
    void Application::ReleaseGUI() {
#if SB_USE_MYGUI
        if (m_gui) {
            MyGUI::LanguageManager::getInstance().eventRequestTag.clear();
            
            mygui::unregister_factory();
            UnregisterWidgets();
            m_gui->shutdown();
        }
        delete m_gui;
        m_gui = 0;
        delete m_gui_render;
        m_gui_render = 0;
        delete m_gui_data_manager;
        m_gui_data_manager = 0;
#endif
    }
        
    void Application::BindModules( LuaVM* lua) {
        register_math(lua->GetVM());
        register_utils(lua->GetVM());
        register_resources(lua->GetVM());
        register_scene(lua->GetVM());
        register_thread(lua->GetVM());
        register_controller(lua->GetVM());
        register_json(lua->GetVM());
        register_keys(lua->GetVM());
        ParticlesSystem::Bind(lua);
#ifdef SB_USE_MYGUI
        mygui::register_mygui(lua->GetVM());
#endif

        register_skelet(lua->GetVM());
#ifdef SB_USE_NETWORK
        BindNetwork(lua);
#endif
    }
    
    void Application::OnSystemSet() {
        
    }
    
    bool Application::OpenURL(const sb::string& url) {
        if (!m_system)
            return false;
        return m_system->OpenURL(url.c_str());
    }
	
	///
	void GHL_CALL Application::SetSystem( GHL::System* sys ) {
        sb_ensure_main_thread();
		m_system = sys;
        if (!m_title.empty()) {
            SetTitle(m_title);
        }
        GHL::Int32 offset = 0;
        m_system->GetDeviceData(GHL::DEVICE_DATA_UTC_OFFSET, &offset);
        m_utc_offset = offset;
        char language[33];
        language[0] = 0;
        if (m_system->GetDeviceData(GHL::DEVICE_DATA_LANGUAGE, language) && language[0]) {
            language[32] = 0;
            m_system_language = language;
        }
        OnSystemSet();
	}
	///
	void GHL_CALL Application::SetVFS( GHL::VFS* vfs ) {
		m_vfs = vfs;
        Logger::StartSession(m_vfs);
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
    
    Resources* Application::CreateResourcesManager() {
        return new Resources(m_vfs);
    }
    
    void Application::CreateLua() {
        if (!m_lua) {
            sb_assert(m_resources);
            m_lua = new LuaVM(m_resources);
            
            std::string  base_path=m_lua_base_path;
            if (!base_path.empty() && base_path[base_path.size()-1]!='/')
                base_path+="/";
            m_lua->SetBasePath(base_path.c_str());
            
            OnLuaCreated();
            
            InitLua();
            
            InitResources();
        }
    }
    
    void Application::DestroyLua() {

        if (m_lua) {
            OnLuaDestroy();
            m_lua->Destroy();
            delete m_lua;
            m_lua = 0;
        }
    }
    
    Network* Application::CreateNetwork() {
#ifdef SB_USE_NETWORK
        return new Network(m_resources);
#else
        return 0;
#endif
    }
    
    ///
	void GHL_CALL Application::FillSettings( GHL::Settings* settings ) {
        sb_ensure_main_thread();
		sb_assert( m_vfs );
        SB_LOGI("FillSettings");
        
        CreateLua();
        
        sb_assert(m_lua);
        

        LuaContextPtr ctx = m_lua->GetGlobalContext();
        ctx->SetValue("settings", settings);
        m_lua->DoFile("settings.lua");
        ctx->SetValue("settings", (GHL::Settings*)(0));
        
        m_width = settings->width;
        m_height = settings->height;

        SB_LOGI("FillSettings <<<");
	}
    
    void Application::InitLua() {
        LuaContextPtr ctx = m_lua->GetGlobalContext();
        
        
        
#ifdef GHL_PLATFORM_IOS
        ctx->SetValue("platform.os", "ios");
#endif
        
#ifdef GHL_PLATFORM_MAC
        ctx->SetValue("platform.os", "osx");
#endif
        
#ifdef GHL_PLATFORM_WIN
        ctx->SetValue("platform.os", "windows");
#endif
        
#ifdef GHL_PLATFORM_FLASH
        ctx->SetValue("platform.os", "flash");
#endif
        
#ifdef GHL_PLATFORM_ANDROID
        ctx->SetValue("platform.os", "android");
#endif
        
#ifdef GHL_PLATFORM_EMSCRIPTEN
        ctx->SetValue("platform.os", "emscripten");
#endif
        {
            sb_assert(m_system);
            char data[129];
            memset(data,0,sizeof(data));
            if (m_system->GetDeviceData(GHL::DEVICE_DATA_NAME, data)) {
                ctx->SetValue<const char*>("platform.device", data);
            }
            memset(data,0,sizeof(data));
            if (m_system->GetDeviceData(GHL::DEVICE_DATA_OS, data)) {
                ctx->SetValue<const char*>("platform.os_name", data);
            }
        }
        
        luabind::ExternClass<Sandbox::Application>(m_lua->GetVM());
        luabind::RawClass<GHL::Settings>(m_lua->GetVM());
        
        
        BindModules( m_lua );
        
        ctx->SetValue("application.resources", m_resources);
        
        ctx->SetValue("application.app", this);
        
        if (!RestoreAppProfile()) {
            ctx->SetValue("application.profile.first_start", true );
        } else {
            ctx->SetValue("application.profile.first_start", false );
        }
        
#ifdef SB_USE_NETWORK
        ctx->SetValue("application.network", m_network);
#endif
        
    }
    
    void Application::InitResources() {
        
    }
    
    void Application::ReleaseResources() {
        
    }
	///
	bool GHL_CALL Application::Load() {
        sb_ensure_main_thread();
        SB_LOGI( "Application::Load" );
        ConfigureDevice( m_system );
        
        if (!m_graphics) {
            sb_assert(m_resources);
            m_graphics = new Graphics(m_resources);
        }
        
        sb_assert(m_render);
        sb_assert(m_image_decoder);
        m_resources->Init(m_render, m_image_decoder);
        m_sound_mgr->Init(m_sound,m_resources);
        
        CreateLua();
        
        LuaContextPtr ctx = m_lua->GetGlobalContext();
        
        ctx->SetValue("application.resources", m_resources);
        ctx->SetValue("application.sound", m_sound_mgr);

        
#ifdef SB_USE_MYGUI
        if (!m_gui_data_manager) {
            m_gui_data_manager = new mygui::DataManager(m_resources);
            sb_assert(!m_gui_render);
            m_gui_render = new mygui::RenderManager(m_graphics, m_resources);
        }
        
        if (!m_gui) {
            m_gui = new mygui::GUI(m_system);
            m_gui->initialize(m_lua->GetGlobalContext());
            mygui::register_factory();
            RegisterWidgets();
        }
        
#endif

        UpdateScreenSize();
        sb_assert(m_lua);
        m_lua->DoFile("load.lua");
        
        
		if (!LoadResources(*m_resources))
			return false;
        
        if (!m_main_scene) {
            m_main_scene = new Scene();
            ctx->SetValue("application.scene", m_main_scene);
        }
        
        if (!m_main_thread) {
            m_main_thread = new ThreadsMgr();
            ctx->SetValue("application.thread", m_main_thread);
        }
        m_graphics->Load(m_render);
       
        
		OnLoaded();
		
        m_lua->DoFile("main.lua");
        
        if (!m_url.empty()) {
            if (m_lua && m_lua->GetGlobalContext()->GetValue<bool>("application.onOpenURL")) {
                m_lua->GetGlobalContext()->GetValue<LuaContextPtr>("application")
                ->call("onOpenURL",m_url);
            }
            m_url.clear();
        }
        
        SB_LOGI( "Application::Load <<< ");
		return true;
	}
    
    void GHL_CALL Application::Unload() {
        sb_ensure_main_thread();
        SB_LOGI( "Application::Unload >>> ");
        
        OnDeactivated();
        ReleaseResources();
        
        m_mouse_ctx.reset();
        m_keyboard_ctx.reset();
        
        DestroyLua();
        
        delete m_main_thread;
        delete m_main_scene;
        m_main_scene = 0;
        m_main_thread = 0;
        ReleaseGUI();
        if (m_resources) {
            m_resources->ReleaseAll();
        }
        delete m_graphics;
        m_graphics = 0;
        m_sound_mgr->Deinit();
        SB_LOGI( "Application::Unload <<< " );
    }
    
    void Application::DoRestart() {
        Unload();
        Logger::StartSession(GetVFS());
        Load();
    }
    
    void Application::DoExit() {
        if (m_system) {
            m_system->Exit();
        }
    }
        
    void Application::UpdateScreenSize() {
        if (!m_lua)
            return;
        sb_assert(m_graphics);
        sb_assert(m_resources);
        float graphics_scal = m_graphics->GetScale();
        float resources_scale = m_resources->GetScale();
        SB_LOGI("UpdateScreenSize g:" << graphics_scal << " r:" << resources_scale << " s:" << m_width << "x" << m_height);
        LuaContextPtr ctx = m_lua->GetGlobalContext();
        m_draw_width = float(m_width) / (graphics_scal * resources_scale);
        m_draw_height = float(m_height) / (graphics_scal * resources_scale);
        ctx->SetValue("application.size.width", m_draw_width);
        ctx->SetValue("application.size.height", m_draw_height);
        ctx->SetValue("application.size.scale", graphics_scal * resources_scale);
#ifdef SB_USE_MYGUI
        if (m_gui_render) {
            m_gui_render->reshape(GetDrawWidth(), GetDrawHeight());
        }
#endif
    }
    
    Network* Application::GetNetwork() {
#ifdef SB_USE_NETWORK
        return m_network;
#else
        return 0;
#endif
    }
    
    ImagePtr Application::RenderScreen() {
        sb_ensure_main_thread();
        GHL::UInt32 twidth = m_render->GetWidth();
        GHL::UInt32 theight = m_render->GetHeight();
        float scale = m_resources->GetScale() * m_graphics->GetScale();
        float width = twidth / scale;
        float height = theight / scale;
        RenderTargetPtr target = m_resources->CreateRenderTarget(int(width), int(height), scale, false, false);
        m_render->BeginScene(target->GetNative());
        m_graphics->BeginScene(m_render,target);
        DoDrawScreen();
        m_graphics->EndScene();
        m_render->EndScene();
        ImagePtr img(new Image(target->GetTexture(),0,0,width,height,width,height));
        return img;
    }
    
    void Application::DoDrawScreen() {
        if (m_main_scene)
            m_main_scene->Draw(*m_graphics);
#ifdef SB_USE_MYGUI
        if (m_gui_render)
            m_gui_render->drawFrame();
#endif
        DrawFrame(*m_graphics);
    }
    
	///
	bool GHL_CALL Application::OnFrame( GHL::UInt32 usecs ) {
        sb_ensure_main_thread();
        if (m_need_exit) {
            m_need_exit = false;
            DoExit();
            return false;
        }
        if (m_need_restart) {
            m_need_restart = false;
            DoRestart();
        }
        GHL::UInt32 width = m_render->GetWidth();
        GHL::UInt32 height = m_render->GetHeight();
        if (width != m_width || height != m_height) {
            m_width = width;
            m_height = height;
            OnResize();
        }

#ifdef SB_USE_NETWORK
        if (m_network) {
            m_network->Process();
        }
#endif
        
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
        if (m_main_thread)
            m_main_thread->Update(dt);
	
        Update(dt);

        if (m_sound_mgr) {
            m_sound_mgr->Update(dt);
        }
        
        PlatformExtension::OnTimerAll(this);
        
#ifdef SB_USE_MYGUI
        if (m_gui_render)
            m_gui_render->timerFrame(dt);
#endif
        
        UpdateRenderTargets(dt,m_render);
        
		m_render->BeginScene(0);
		if (m_clear_buffer)
			m_render->Clear(m_clear_color.r,
							m_clear_color.g,
							m_clear_color.b,
							m_clear_color.a,0);
		m_graphics->BeginScene(m_render,RenderTargetPtr());
        
        DoDrawScreen();
        
        m_graphics->SetBlendMode(BLEND_MODE_ALPHABLEND);
       
        size_t batches = m_graphics->EndScene();
       
        m_batches = m_batches * 0.875f + 0.125f*batches;    /// interpolate 4 frames
        m_render->SetupBlend(true,GHL::BLEND_FACTOR_SRC_ALPHA,GHL::BLEND_FACTOR_SRC_ALPHA_INV);
        
        if (m_draw_debug_info)
            DrawDebugInfo();
		m_render->EndScene();
        
        m_resources->ProcessMemoryMgmt();
        
//        if (m_lua)
//            m_lua->DoGC();
        
		return true;
	}
    
    double Application::GetTimeUSec() const {
        GHL::TimeValue tv;
        GHL_GetTime(&tv);
        return double(tv.secs) * 1000000 + tv.usecs;
    }
		
	void Application::DrawFrame(Graphics&) const {
	}
    
    void Application::UpdateRenderTargets(float dt,GHL::Render* render) {
        // update targets
        for (sb::list<RTScenePtr>::const_iterator it = m_rt_scenes.begin();it!=m_rt_scenes.end();++it) {
            (*it)->Update(dt);
        }
        
        size_t batches = 0;
        
        for (sb::list<RTScenePtr>::const_iterator it = m_rt_scenes.begin();it!=m_rt_scenes.end();++it) {
            batches += (*it)->Draw(m_render, *m_graphics);
        }
        m_batches_rt = m_batches_rt * 0.875f + 0.125f*batches;    /// interpolate 4 frames
    }
	
	void Application::DrawDebugInfo() {
		char buf[128];
		sb::snprintf(buf,128,"fps:%0.2f",m_fps);
		m_render->SetProjectionMatrix(Matrix4f::ortho(0.0f,
			float(m_render->GetWidth()),
			float(m_render->GetHeight())
			,0.0f,-1.0f,1.0f).matrix);
		m_render->DebugDrawText( 10, 10 , buf );
        m_render->DebugDrawText( 10, 21 , m_lua->GetMemoryUsed().c_str() );
        sb::snprintf(buf,128,"batches:%0.2f/%0.2f",m_batches,m_batches_rt);
        m_render->DebugDrawText( 10, 32 , buf );
        m_render->DebugDrawText( 10, 43,
                                (sb::string("res:")+format_memory(m_resources->GetMemoryUsed())+
                                 sb::string("/")+format_memory(m_resources->GetMemoryLimit())).c_str());
        int y = 54;
        size_t render_mem = m_render->GetMemoryUsage();
        if (render_mem) {
            m_render->DebugDrawText( 10, y,
                                    (sb::string("tex:")+format_memory(render_mem)).c_str() );
            y += 11;
        }
        sb::snprintf(buf,128,"objects:%d",int(meta::object::count()));
        m_render->DebugDrawText( 10, y, buf );
        y += 11;
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
    void    Application::SetMouseContext(const LuaContextPtr& ctx ) {
        m_mouse_ctx = ctx;
    }
    
    void Application::SetKeyboardContext(const LuaContextPtr& ctx) {
        m_keyboard_ctx = ctx;
    }
	
    void    Application::SetFrameInterval(int interval) {
        if (m_system) {
            GHL::Int32 frame_interval = interval;
            m_system->SetDeviceState(GHL::DEVICE_STATE_FRAME_INTERVAL, &frame_interval);
        }
    }
    
    void    Application::SetResizeableWindow(bool v) {
        if (m_system) {
            m_system->SetDeviceState(GHL::DEVICE_STATE_RESIZEABLE_WINDOW, &v);
        }
    }
    
    void    Application::SetScreenKeepOn(bool v) {
        if (m_system) {
            m_system->SetDeviceState(GHL::DEVICE_STATE_KEEP_SCREEN_ON, &v);
        }
    }
    
	void Application::SetClearColor(const Color& c) {
		m_clear_buffer = true;
		m_clear_color = c;
	}
    
    void Application::SetTitle(const sb::string& title) {
        m_title = title;
        if (m_system) {
            m_system->SetTitle(title.c_str());
        }
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
    
    bool Application::RestoreAppProfile() {
        SB_LOGI("RestoreAppProfile");
        if (m_lua && m_vfs) {
            GHL::Data* d = LoadProfileFile("profile.json");
            if (!d) {
                return false;
            }
            LuaContextPtr decoded = convert_from_json( m_lua->GetVM(), d);
            if (!decoded) {
                ReportAppError("parse profile failed");
            }
            d->Release();
            m_lua->GetGlobalContext()->SetValue("application.profile", decoded);
            SB_LOGI( "profile loaded" );
            return true;
        }
        return false;
    }
    void Application::StoreAppProfile() {
        SB_LOGI( "StoreAppProfile" );
        if (m_lua && m_vfs) {
            LuaContextPtr profile = m_lua->GetGlobalContext()->GetValue<LuaContextPtr>("application.profile");
            if (profile) {
                sb::string json = convert_to_json(profile);
                StringData* sd( new StringData(json));
                StoreProfileFile("profile.json", sd);
                sd->Release();
            }
        }
    }
    void Application::SetDrawDebugInfo(bool draw) {
        m_draw_debug_info = draw;
    }
#ifdef SB_USE_MYGUI
    static void check_key_mods(MyGUI::InputManager& mgr,GHL::UInt32 mods) {
        if ((mods & GHL::KEYMOD_SHIFT)!=0 && !mgr.isShiftPressed()) {
            mgr.injectKeyPress(MyGUI::KeyCode::LeftShift,0);
        }
        if ((mods & GHL::KEYMOD_SHIFT)==0 && mgr.isShiftPressed()) {
            mgr.injectKeyRelease(MyGUI::KeyCode::LeftShift);
        }
        if ((mods & (GHL::KEYMOD_COMMAND|GHL::KEYMOD_CTRL))!=0 && !mgr.isControlPressed()) {
            mgr.injectKeyPress(MyGUI::KeyCode::LeftControl,0);
        }
        if ((mods & (GHL::KEYMOD_COMMAND|GHL::KEYMOD_CTRL))==0 && mgr.isControlPressed()) {
            mgr.injectKeyRelease(MyGUI::KeyCode::LeftControl);
        }
    }
#endif
    
    void Application::OnAppStarted() {
        PlatformExtension::OnAppStartedAll(this);
        sb_assert(m_vfs);
        if (!m_resources) {
            m_resources = CreateResourcesManager();
            sb_assert(m_resources);
            
            
            
            std::string base_path = m_vfs->GetDir(GHL::DIR_TYPE_DATA);
            if (!base_path.empty() && base_path[base_path.size()-1]!='/')
                base_path+="/";
            base_path+=m_resources_base_path;
            if (!base_path.empty() && base_path[base_path.size()-1]!='/')
                base_path+="/";
            m_resources->SetBasePath(base_path.c_str());
            
        }
        
#ifdef SB_USE_NETWORK
        if (!m_network) {
            m_network = CreateNetwork();
        }
#endif
        CreateLua();
    }
    
    void GHL_CALL Application::OnEvent( GHL::Event* event ) {
        sb_ensure_main_thread();
#ifdef SB_USE_MYGUI
        if (m_gui) {
            m_gui->eventGHLEvent(event);
        }
#endif
        switch( event->type ) {
            case GHL::EVENT_TYPE_KEY_PRESS:
#ifdef SB_USE_MYGUI
                if (MyGUI::InputManager::getInstancePtr()) {
                    check_key_mods(MyGUI::InputManager::getInstance(),event->data.key_press.modificators);
                    MyGUI::InputManager::getInstance().injectKeyPress(
                                                                  mygui::translate_key(event->data.key_press.key),
                                                                  event->data.key_press.charcode);
                }
#endif
                if (m_keyboard_ctx) {
                    event->data.key_press.handled = m_keyboard_ctx->call_self<bool>("onKeyDown",event->data.key_press.key,event->data.key_press.modificators);
                    if (event->data.key_press.charcode)
                        m_keyboard_ctx->call_self("onChar",event->data.key_press.charcode);
                }
                break;
            case GHL::EVENT_TYPE_KEY_RELEASE:
#ifdef SB_USE_MYGUI
                if (MyGUI::InputManager::getInstancePtr()) {
                    check_key_mods(MyGUI::InputManager::getInstance(),event->data.key_release.modificators);
                    MyGUI::InputManager::getInstance().injectKeyRelease(mygui::translate_key(event->data.key_release.key));
                }
#endif
                if (m_keyboard_ctx) {
                    event->data.key_release.handled = m_keyboard_ctx->call_self<bool>("onKeyUp",event->data.key_release.key);
                }
                break;
            case GHL::EVENT_TYPE_KEYBOARD_HIDE:
#ifdef SB_USE_MYGUI
                if (MyGUI::InputManager::getInstancePtr()) {
                    MyGUI::InputManager::getInstance().resetKeyFocusWidget();
                }
#endif
                break;
            case GHL::EVENT_TYPE_MOUSE_PRESS:
#ifdef SB_USE_MYGUI
                if (MyGUI::InputManager::getInstancePtr()) {
                    check_key_mods(MyGUI::InputManager::getInstance(),event->data.mouse_press.modificators);
                }
#endif
                OnMouseDown(event->data.mouse_press.button, event->data.mouse_press.x, event->data.mouse_press.y);
                break;
            case GHL::EVENT_TYPE_MOUSE_MOVE:
#ifdef SB_USE_MYGUI
                if (MyGUI::InputManager::getInstancePtr()) {
                    check_key_mods(MyGUI::InputManager::getInstance(),event->data.mouse_move.modificators);
                }
#endif
                OnMouseMove(event->data.mouse_move.button, event->data.mouse_move.x, event->data.mouse_move.y);
                break;
            case GHL::EVENT_TYPE_MOUSE_RELEASE:
#ifdef SB_USE_MYGUI
                if (MyGUI::InputManager::getInstancePtr()) {
                    check_key_mods(MyGUI::InputManager::getInstance(),event->data.mouse_release.modificators);
                }
#endif
                OnMouseUp(event->data.mouse_release.button, event->data.mouse_release.x, event->data.mouse_release.y);
                break;
            case GHL::EVENT_TYPE_APP_STARTED:
                OnAppStarted();
                break;
            case GHL::EVENT_TYPE_ACTIVATE:
                OnActivated();
                break;
            case GHL::EVENT_TYPE_DEACTIVATE:
                OnDeactivated();
                break;
            case GHL::EVENT_TYPE_VISIBLE_RECT_CHANGED:
                if (m_lua) {
                    if (m_lua->GetGlobalContext()->GetValue<bool>("application.onVisibleRectChanged")) {
                        float size_scale = 1.0f / (m_resources->GetScale()*m_graphics->GetScale());
                        m_lua->GetGlobalContext()->GetValue<LuaContextPtr>("application")
                        ->call("onVisibleRectChanged",
                               Recti(event->data.visible_rect_changed.x * size_scale,
                                     event->data.visible_rect_changed.y * size_scale,
                                     event->data.visible_rect_changed.w * size_scale,
                                     event->data.visible_rect_changed.h * size_scale));
                    }
                }
                break;
            case GHL::EVENT_TYPE_HANDLE_URL:
                if (m_lua && m_lua->GetGlobalContext()->GetValue<bool>("application.onOpenURL")) {
                    m_lua->GetGlobalContext()->GetValue<LuaContextPtr>("application")
                    ->call("onOpenURL",event->data.handle_url.url);
                } else {
                    m_url = event->data.handle_url.url;
                }
                break;
            default:
                break;
        }
    }
	
    void Application::TransformMouse(GHL::Int32 x,GHL::Int32 y,float& fx, float& fy) {
        if (!m_resources || !m_graphics) {
            fx = float(x);
            fy = float(y);
            return;
        }
        fx = x / (m_resources->GetScale()*m_graphics->GetScale());
        fy = y / (m_resources->GetScale()*m_graphics->GetScale());
    }
	///
	void Application::OnMouseDown( GHL::MouseButton key, GHL::Int32 x, GHL::Int32 y) {
        float fx,fy;
        TransformMouse(x,y,fx,fy);
#ifdef SB_USE_MYGUI
        if (MyGUI::InputManager::getInstancePtr()) {
            if (MyGUI::InputManager::getInstance().injectMousePress(fx, fy, mygui::translate_key(key)) ||
                MyGUI::InputManager::getInstance().isModalAny())
                return;
        }
#endif
        if (m_mouse_ctx) {
            m_mouse_ctx->call_self("onDown",key,fx,fy);
        }
    }
	///
	void Application::OnMouseMove( GHL::MouseButton key, GHL::Int32 x, GHL::Int32 y) {
        float fx,fy;
        TransformMouse(x,y,fx,fy);
#ifdef SB_USE_MYGUI
        if (MyGUI::InputManager::getInstancePtr())
        if (MyGUI::InputManager::getInstance().injectMouseMove(fx, fy, 0)||
            MyGUI::InputManager::getInstance().isModalAny())
            return;
#endif
        if (m_mouse_ctx) {
            m_mouse_ctx->call_self("onMove",key,fx,fy);
        }
    }
	///
	void Application::OnMouseUp( GHL::MouseButton key, GHL::Int32 x, GHL::Int32 y) {
        float fx,fy;
        TransformMouse(x,y,fx,fy);
#ifdef SB_USE_MYGUI
        if (MyGUI::InputManager::getInstancePtr())
        if (MyGUI::InputManager::getInstance().injectMouseRelease(fx, fy, mygui::translate_key(key))||
            MyGUI::InputManager::getInstance().isModalAny())
            return;
#endif
        if (m_mouse_ctx) {
            m_mouse_ctx->call_self("onUp",key,fx,fy);
        }
    }
	///
	void Application::OnDeactivated() {
        
        SB_LOGI( "OnDeactivated" );
        if (m_lua) {
            if (m_lua->GetGlobalContext()->GetValue<bool>("application.onDeactivated")) {
                m_lua->GetGlobalContext()->GetValue<LuaContextPtr>("application")->call("onDeactivated");
            }
        }
        if (m_sound_mgr) {
            m_sound_mgr->Pause();
        }
#ifdef SB_USE_MYGUI
        if (MyGUI::InputManager::getInstancePtr()) {
            MyGUI::InputManager::getInstance().resetKeyFocusWidget();
            MyGUI::InputManager::getInstance().resetMouseCaptureWidget();
            MyGUI::InputManager::getInstance()._resetMouseFocusWidget();
        }
#endif
        StoreAppProfile();
	}
	///
	void Application::OnActivated() {
        SB_LOGI( "OnActivated" );
        if (m_sound_mgr) {
            m_sound_mgr->Resume();
        }
        if (m_lua) {
            if (m_lua->GetGlobalContext()->GetValue<bool>("application.onActivated")) {
                m_lua->GetGlobalContext()->GetValue<LuaContextPtr>("application")->call("onActivated");
            }
        }
	}
    
    void Application::OnResize() {
        if (m_lua) {
            LuaContextPtr ctx = m_lua->GetGlobalContext();
            if (ctx) {
                UpdateScreenSize();
                if (m_lua->GetGlobalContext()->GetValue<bool>("application.onResize")) {
                    m_lua->GetGlobalContext()->GetValue<LuaContextPtr>("application")->call("onResize");
                }
            }
        }
    }
	///
	void GHL_CALL Application::Release(  ) {
        OnDeactivated();
        
        PlatformExtension::OnAppStoppedAll(this);
        
        DestroyLua();
        
        delete m_sound_mgr;
        m_sound_mgr = 0;
        SB_LOGI( "Release application" );
		delete this;
	}
	
    void Application::SetResourcesVariant(float scale,const sb::string& postfix) {
        sb_assert(m_resources);
        m_resources->SetResourcesVariant(scale, postfix);
        UpdateScreenSize();
    }
    
    void Application::SetRenderScale(float scale) {
        sb_assert(m_graphics);
        m_graphics->SetScale(scale);
        UpdateScreenSize();
    }
    
    
    bool Application::CallExtension(const char* method, const char* args, sb::string& res ) {
        return PlatformExtension::ProcessAll(this,method,args,res);
    }
    
    void Application::OnExtensionResponse(const char* method,const char* data) {
        if (m_lua) {
            if (m_lua->GetGlobalContext()->GetValue<bool>("application.onExtensionResponse")) {
                m_lua->GetGlobalContext()->GetValue<LuaContextPtr>("application")->call("onExtensionResponse",method,data);
            }
        }
    }
    
    
    bool Application::StoreProfileFile( const char* filename , const GHL::Data* data ) {
        if (!m_vfs)
            return false;
        sb::string path = m_vfs->GetDir(GHL::DIR_TYPE_USER_PROFILE);
        if (!path.empty() && path[path.length()-1]!='/') {
            path += "/";
        }
        path += filename;

        LogInfo() << "store profile to " << path;
        if (!m_vfs->WriteFile(path.c_str(), data)) {
            LogError() << "failed write " << path;
            ReportAppError("write profile failed");
            return false;
        }
        return true;
    }
    GHL::Data* Application::LoadProfileFile( const char* filename ) {
        if (!m_vfs)
            return 0;
        sb::string path = m_vfs->GetDir(GHL::DIR_TYPE_USER_PROFILE);
        if (!path.empty() && path[path.length()-1]!='/') {
            path += "/";
        }
        path += filename;
       
        SB_LOGI( "load profile from " << path );
        GHL::DataStream* ds = m_vfs->OpenFile(path.c_str());
        if (!ds) {
            LogError() << "open " << path << " failed";
            return 0;
        }
        GHL::Data* d = GHL_ReadAllData( ds );
        ds->Release();
        return d;
    }
    
    
}
