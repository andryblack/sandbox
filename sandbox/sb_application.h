/*
 *  sb_application.h
 *  YinYang
 *
 *  Created by Андрей Куницын on 04.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */
#ifndef SB_APPLICATION_H_INCLUDED
#define SB_APPLICATION_H_INCLUDED

#include <ghl_application.h>
#include "sb_color.h"
#include "sb_threads_mgr.h"
#include "sb_rt_scene.h"
#include "sb_lua.h"
#include <sbstd/sb_list.h>
#include "sb_image.h"

namespace GHL {
    struct Data;
}

namespace Sandbox {
    
    class Resources;
    class SoundManager;
    namespace mygui {
        class GUI;
        class DataManager;
        class RenderManager;
    }
    class Network;
    
    
	class Application : public meta::object_proxy<GHL::Application> {
        SB_META_OBJECT
    public:
        void SetSoundEnabled( bool e );
        bool GetSoundEnabled() const;
        void SetMusicEnabled( bool e );
        bool GetMusicEnabled() const;

        void    AddScene( const RTScenePtr& scene );
        void    RemoveScene( const RTScenePtr& scene );
        void    SetMouseContext(const LuaContextPtr& ctx );
        void    SetKeyboardContext(const LuaContextPtr& ctx );
        void    SetFrameInterval(int interval);
        void    SetResizeableWindow(bool v);
        void    SetScreenKeepOn(bool o);
        
        GHL::System* GetSystem() const { return m_system;}
        
        virtual bool CallExtension( const char* method,
                                   const char* argument,
                                   sb::string& result );
        virtual void OnExtensionResponse(const char* method,const char* data);
        
        void SetDrawDebugInfo(bool draw);
        void SetResourcesVariant(float scale,const sb::string& postfix);
        void SetRenderScale(float scale);
        
        double GetTimeUSec() const;
        int GetUTCOffset() const { return m_utc_offset; }
        const sb::string& GetSystemLanguage() const { return m_system_language; }
        
        void Restart() { m_need_restart = true; }
        void Exit() { m_need_exit = true; }
        
        GHL::UInt32 GetScreenWidth() const { return m_width; }
        GHL::UInt32 GetScreenHeight() const { return m_height; }
        float GetDrawWidth() const { return m_draw_width; }
        float GetDrawHeight() const { return m_draw_height; }
        float GetFPS() const { return m_fps; }
        
        bool StoreProfileFile( const char* filename , const GHL::Data* data );
        GHL::Data* LoadProfileFile( const char* filename );
        
        ImagePtr RenderScreen();
        bool OpenURL(const sb::string& url);
        virtual void ReportAppError(const char* reason) {}
        Network* GetNetwork();
        GHL::VFS* GetVFS() const { return m_vfs;}
        
    protected:
		Application();
		virtual ~Application();
		virtual void Update(float) {}
		virtual void DrawFrame(Graphics& g) const;
        virtual void UpdateRenderTargets(float dt,GHL::Render* render);
		virtual bool LoadResources(Resources&) { return true;}
		void SetLuaBasePath(const char* path) {
			m_lua_base_path = path;
		}
		void SetResourcesBasePath( const char* path ) {
			m_resources_base_path = path;
		}
        /// bind Lua modules
		virtual void BindModules( LuaVM* );
		virtual void OnLoaded() {}
		virtual void ConfigureDevice(GHL::System*) {}
		
		
		GHL::Render* GetRender() const { return m_render;}
        GHL::ImageDecoder* GetImageDecoder() const { return m_image_decoder; }
		LuaVM* GetLua() const { return m_lua;}
		ThreadsMgr* GetThreads() const { return m_main_thread;}
		Resources* GetResources() const { return m_resources;}
        Graphics* GetGraphics() const { return m_graphics; }
        Scene* GetScene() const { return m_main_scene; }
        SoundManager* GetSoundManager() const { return m_sound_mgr; }
		
		void SetClearColor(const Color& c);
        void SetClearDepth(float d);
        void SetTitle(const sb::string& title);
        
        bool RestoreAppProfile();
        void StoreAppProfile();
        
        virtual void UpdateScreenSize();
        
        virtual void OnResize();
        
        void TransformMouse(GHL::Int32 x,GHL::Int32 y, float& fx, float& fy);
        ///
        virtual void OnMouseDown( GHL::MouseButton btn, GHL::Int32 x, GHL::Int32 y) ;
        ///
        virtual void OnMouseMove( GHL::MouseButton btn, GHL::Int32 x, GHL::Int32 y) ;
        ///
        virtual void OnMouseUp( GHL::MouseButton btn, GHL::Int32 x, GHL::Int32 y) ;
        ///
        virtual void OnDeactivated() ;
        ///
        
        virtual void OnAppStarted();
        
        virtual void OnActivated() ;
        
        virtual void OnSuspended();
        virtual void OnResumed();
        
        virtual void TrimMemory();
        
        virtual void OnSystemSet();
        
        virtual Resources* CreateResourcesManager();
        virtual Network* CreateNetwork();
#ifdef SB_USE_MYGUI
        virtual mygui::GUI* CreateGUI();
#endif
        virtual void InitResources();
        virtual void ReleaseResources();
        
        
        void InitLua();
        
        void ReleaseGUI();
        virtual void DoRestart();
        virtual void DoExit();
        
        
        
        void DoDrawScreen();
        
        // called after lua created
        virtual void OnLuaCreated() {}
        // called before lua destroyed
        virtual void OnLuaDestroy() {}
    private:
		GHL::System*	m_system;
		GHL::VFS*		m_vfs;
		GHL::Render*	m_render;
		GHL::ImageDecoder*	m_image_decoder;
		GHL::Sound*		m_sound;
		LuaVM*			m_lua;
        LuaContextPtr   m_mouse_ctx;
        LuaContextPtr   m_keyboard_ctx;
		Graphics*		m_graphics;
		Resources*		m_resources;
        SoundManager*   m_sound_mgr;
		sb::string		m_lua_base_path;
		sb::string		m_resources_base_path;
		size_t			m_frames;
		GHL::UInt32		m_frames_time;
		float			m_fps;
        bool            m_draw_debug_info;
		void DrawDebugInfo();
		ThreadsMgr*		m_main_thread;
		Scene*			m_main_scene;
        sb::list<RTScenePtr>    m_rt_scenes;
        GHL::UInt32     m_width;
        GHL::UInt32     m_height;
        float   m_draw_width;
        float   m_draw_height;
        
		bool		m_clear_buffer;
		Color		m_clear_color;
        float       m_clear_depth;
        
        float       m_batches;
        float       m_batches_rt;
        
        bool    m_sound_enabled;
        bool    m_music_enabled;
        
        sb::string  m_title;
        sb::string  m_system_language;
        int m_utc_offset;
        
        void CreateLua();
        void DestroyLua();
        
#ifdef SB_USE_MYGUI
        mygui::DataManager*     m_gui_data_manager;
        mygui::RenderManager*   m_gui_render;
        mygui::GUI* m_gui;
        
        virtual void RegisterWidgets() {}
        virtual void UnregisterWidgets() {}
#endif
#ifdef SB_USE_NETWORK
        Network*    m_network;
#endif
        bool    m_need_restart;
        bool    m_need_exit;
        sb::string  m_url;
        
        bool m_need_resize;
    public:
		///
		virtual void GHL_CALL SetSystem( GHL::System* sys ) SB_OVERRIDE;
		///
		virtual void GHL_CALL SetVFS( GHL::VFS* vfs ) SB_OVERRIDE ;
		///
		virtual void GHL_CALL SetRender( GHL::Render* render ) SB_OVERRIDE;
		///
		virtual void GHL_CALL SetImageDecoder( GHL::ImageDecoder* decoder ) SB_OVERRIDE;
		///
		virtual void GHL_CALL SetSound( GHL::Sound* sound) SB_OVERRIDE;
		///
		virtual void GHL_CALL FillSettings( GHL::Settings* settings ) SB_OVERRIDE;
        ///
		virtual bool GHL_CALL Load() SB_OVERRIDE;
        ///
        virtual void GHL_CALL Unload() SB_OVERRIDE;
		///
		virtual bool GHL_CALL OnFrame( GHL::UInt32 usecs ) SB_OVERRIDE;
        ///
        virtual void GHL_CALL OnEvent( GHL::Event* event ) SB_OVERRIDE;
		///
		virtual void GHL_CALL Release(  ) SB_OVERRIDE;
		
	};
	
}

#endif /*SB_APPLICATION_H_INCLUDED*/
