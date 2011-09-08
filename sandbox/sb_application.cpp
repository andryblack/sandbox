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
#include <ghl_settings.h>
#include <ghl_vfs.h>

namespace Sandbox {
	
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
		m_lua = new Lua(m_vfs);
		std::string base_path = m_vfs->GetDir(GHL::DIR_TYPE_DATA);
		if (!base_path.empty() && base_path[base_path.size()-1]!='/')
			base_path+="/";
		base_path+=m_lua_base_path;
		if (!base_path.empty() && base_path[base_path.size()-1]!='/')
			base_path+="/";
		m_lua->SetBasePath(base_path.c_str());
		{
			SB_BIND_BEGIN_RAW_CLASS( GHL::Settings )
			SB_BIND_NO_CONSTRUCTOR
			SB_BIND_BEGIN_METHODS
			SB_BIND_END_METHODS
			SB_BIND_BEGIN_PROPERTYS
			SB_BIND_PROPERTY_RAW(GHL::Settings,width,GHL::UInt32)
			SB_BIND_PROPERTY_RAW(GHL::Settings,height,GHL::UInt32)
			SB_BIND_PROPERTY_RAW(GHL::Settings,fullscreen,bool)
			SB_BIND_END_PROPERTYS
			SB_BIND_END_CLASS
			SB_BIND(m_lua)
		}
		m_lua->SetValue(settings, "settings", "GHL::Settings");
		BindModules( m_lua );
		m_lua->DoFile("settings.lua");
	}
	///
	bool GHL_CALL Application::Load() {
		ConfigureDevice( m_system );
		m_graphics = new Graphics();
		m_resources = new Resources(m_vfs,m_render,m_image_decoder);
		std::string base_path = m_vfs->GetDir(GHL::DIR_TYPE_DATA);
		if (!base_path.empty() && base_path[base_path.size()-1]!='/')
			base_path+="/";
		base_path+=m_resources_base_path;
		if (!base_path.empty() && base_path[base_path.size()-1]!='/')
			base_path+="/";
		m_resources->SetBasePath(base_path.c_str());
		m_lua->SetValue(m_resources, "main.resources", "Sandbox::Resources");
		m_lua->DoFile("load.lua");
		if (!LoadResources(*m_resources))
			return false;
		m_main_scene = new Scene();
		m_lua->SetValue(m_main_scene, "main.scene", "Sandbox::Scene");
		m_main_thread = new ThreadsMgr();
		m_lua->SetValue(m_main_thread, "main.thread", "Sandbox::ThreadsMgr");
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
		float dt = float(usecs)/1000000.0f;
		m_main_thread->Update(dt);
		Update(dt);
		m_render->BeginScene(0);
		m_render->Clear(0,0,0,0);
		m_graphics->BeginScene(m_render);
		m_main_scene->Draw(*m_graphics);
		DrawFrame(*m_graphics);
		m_graphics->EndScene();
		DrawDebugInfo();
		m_render->EndScene();
		return true;
	}
	
	void Application::DrawDebugInfo() {
		char buf[128];
		::snprintf(buf,128,"fps:%0.2f",m_fps);
		m_render->SetProjectionMatrix(Matrix4f::ortho(0,m_render->GetWidth(),m_render->GetHeight(),0,-1,1).matrix);
		m_render->DebugDrawText( 10, 10 , buf );
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
	}
	///
	void GHL_CALL Application::OnMouseMove( GHL::MouseButton btn, GHL::Int32 x, GHL::Int32 y) {
	}
	///
	void GHL_CALL Application::OnMouseUp( GHL::MouseButton btn, GHL::Int32 x, GHL::Int32 y) {
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
