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

namespace MyGUI {
    class Gui;
}

namespace Sandbox {
    
    class Resources;
    class SoundManager;
    namespace mygui {
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
  	
        sb::string GetFlashVar(const sb::string& name) const;
        GHL::System* GetSystem() const { return m_system;}
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
		
		GHL::VFS* GetVFS() const { return m_vfs;}
		GHL::Render* GetRender() const { return m_render;}
		LuaVM* GetLua() const { return m_lua;}
		ThreadsMgr* GetThreads() const { return m_main_thread;}
		Resources* GetResources() const { return m_resources;}
		
		void SetClearColor(const Color& c);
        void SetTitle(const sb::string& title);
        
        bool RestoreAppProfile();
        void StoreAppProfile();
        
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
		void DrawDebugInfo();
		ThreadsMgr*		m_main_thread;
		Scene*			m_main_scene;
        sb::list<RTScenePtr>    m_rt_scenes;
		
		bool		m_clear_buffer;
		Color		m_clear_color;
        
        float       m_batches;
        float       m_batches_rt;
        
        bool    m_sound_enabled;
        bool    m_music_enabled;
        
        sb::string  m_title;
        
#ifdef SB_USE_MYGUI
        mygui::DataManager*     m_gui_data_manager;
        mygui::RenderManager*   m_gui_render;
        MyGUI::Gui* m_gui;
#endif
#ifdef SB_USE_NETWORK
        Network*    m_network;
#endif
    public:
		///
		virtual void GHL_CALL SetSystem( GHL::System* sys );
		///
		virtual void GHL_CALL SetVFS( GHL::VFS* vfs ) ;
		///
		virtual void GHL_CALL SetRender( GHL::Render* render ) ;
		///
		virtual void GHL_CALL SetImageDecoder( GHL::ImageDecoder* decoder ) ;
		///
		virtual void GHL_CALL SetSound( GHL::Sound* sound) ;
		///
		virtual void GHL_CALL FillSettings( GHL::Settings* settings );
        ///
		virtual bool GHL_CALL Load() ;
		///
		virtual bool GHL_CALL OnFrame( GHL::UInt32 usecs ) ;
		///
		virtual void GHL_CALL OnKeyDown( GHL::Key key ) ;
		///
		virtual void GHL_CALL OnKeyUp( GHL::Key key ) ;
		///
		virtual void GHL_CALL OnChar( GHL::UInt32 ch ) ;
		///
		virtual void GHL_CALL OnMouseDown( GHL::MouseButton btn, GHL::Int32 x, GHL::Int32 y) ;
		///
		virtual void GHL_CALL OnMouseMove( GHL::MouseButton btn, GHL::Int32 x, GHL::Int32 y) ;
		///
		virtual void GHL_CALL OnMouseUp( GHL::MouseButton btn, GHL::Int32 x, GHL::Int32 y) ;
		///
		virtual void GHL_CALL OnDeactivated() ;
		///
		virtual void GHL_CALL OnActivated() ;
		///
		virtual void GHL_CALL Release(  ) ;
		
	};
	
}

#endif /*SB_APPLICATION_H_INCLUDED*/
