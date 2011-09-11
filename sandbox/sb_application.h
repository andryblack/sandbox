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
#include "sb_graphics.h"
#include "sb_resources.h"
#include "sb_threads_mgr.h"
#include "sb_scene.h"
#include "sb_lua.h"

namespace Sandbox {
	
	class Application : public GHL::Application {
	protected:
		Application();
		virtual ~Application();
		virtual void Update(float dt) {}
		virtual void DrawFrame(Graphics& g) const;
		virtual bool LoadResources(Resources& res) { return true;}
		void SetLuaBasePath(const char* path) {
			m_lua_base_path = path;
		}
		void SetResourcesBasePath( const char* path ) {
			m_resources_base_path = path;
		}
		virtual void BindModules( Lua* lua ) {}
		virtual void OnLoaded() {}
		virtual void ConfigureDevice(GHL::System* system) {}
		GHL::System* GetSystem() const { return m_system;}
		GHL::VFS* GetVFS() const { return m_vfs;}
		GHL::Render* GetRender() const { return m_render;}
		Lua* GetLua() const { return m_lua;}
		ThreadsMgr* GetThreads() const { return m_main_thread;}
		Resources* GetResources() const { return m_resources;}
		
		void DrawScene() const;
	private:
		GHL::System*	m_system;
		GHL::VFS*		m_vfs;
		GHL::Render*	m_render;
		GHL::ImageDecoder*	m_image_decoder;
		GHL::Sound*		m_sound;
		Lua*			m_lua;
		Graphics*		m_graphics;
		Resources*		m_resources;
		std::string		m_lua_base_path;
		std::string		m_resources_base_path;
		size_t			m_frames;
		GHL::UInt32		m_frames_time;
		float			m_fps;
		void DrawDebugInfo();
		ThreadsMgr*		m_main_thread;
		Scene*			m_main_scene;
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
