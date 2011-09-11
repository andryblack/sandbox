/*
 *  sb_rocket.cpp
 *  YinYang
 *
 *  Created by Андрей Куницын on 10.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */

#include "sb_rocket.h"

#include <Rocket/Core.h>
#include <Rocket/Core/FileInterface.h>
#include <Rocket/Core/Context.h>
#include <Rocket/Debugger.h>
#include <Rocket/Controls.h>
#include <ghl_data_stream.h>
#include <ghl_vfs.h>
#include <ghl_render.h>
#include <string>
#include <iostream>
#include "sb_graphics.h"
#include "sb_resources.h"

namespace Sandbox {
	
	struct RocketFile {
		GHL::DataStream* ds;
	};
	
	class FileInterfaceImpl : public ::Rocket::Core::FileInterface {
	public:
		explicit FileInterfaceImpl( Resources* resources ) : m_resources(resources) {
			SetBasePath("");
		}
		void SetBasePath( const char* path ) {
			m_base_path = path;
			if (!m_base_path.empty() && m_base_path[m_base_path.length()-1]!='/')
				m_base_path+="/";
		}
		// Opens a file.
		virtual ::Rocket::Core::FileHandle Open(const ::Rocket::Core::String& path) {
			std::string full_path = m_base_path;
			full_path+=path.CString();
			GHL::DataStream* ds = m_resources->OpenFile( full_path.c_str() );
			if (!ds) return 0;
			RocketFile* file = new RocketFile;
			file->ds = ds;
			return reinterpret_cast< ::Rocket::Core::FileHandle >(file);
		}
		
		// Closes a previously opened file.
		virtual void Close(::Rocket::Core::FileHandle file) {
			RocketFile* file_p = reinterpret_cast<RocketFile*> (file);
			file_p->ds->Release();
			delete file_p;
		}
		
		// Reads data from a previously opened file.
		virtual size_t Read(void* buffer, size_t size, ::Rocket::Core::FileHandle file) {
			RocketFile* file_p = reinterpret_cast<RocketFile*> (file);
			return file_p->ds->Read( (GHL::Byte*)buffer, size );
		}
		
		// Seeks to a point in a previously opened file.
		virtual bool Seek(::Rocket::Core::FileHandle file, long offset, int origin) {
			RocketFile* file_p = reinterpret_cast<RocketFile*> (file);
			GHL::FileSeekType dir = GHL::F_SEEK_CURRENT;
			if (origin==SEEK_SET)
				dir = GHL::F_SEEK_BEGIN;
			else if (origin==SEEK_END)
				dir = GHL::F_SEEK_END;
			return file_p->ds->Seek( offset, dir );
		}
		
		// Returns the current position of the file pointer.
		virtual size_t Tell(::Rocket::Core::FileHandle file) {
			RocketFile* file_p = reinterpret_cast<RocketFile*> (file);
			return file_p->ds->Tell();
		}
	private:
		std::string	m_base_path;
		Resources*	m_resources;
	};
	
	
	class SystemInterfaceImpl : public ::Rocket::Core::SystemInterface {
	public:
		SystemInterfaceImpl() {
			m_time = 0;
		}
		virtual float GetElapsedTime() {
			return 0.000001f * m_time;
		};
		void Tick( GHL::UInt32 ticks ) {
			m_time += ticks;
		}
	private:
		GHL::UInt32	m_time;
	};
	
	static inline GHL::Vertex convert_verex( const ::Rocket::Core::Vertex v) {
		GHL::Vertex vv;
		vv.x = v.position.x;
		vv.y = v.position.y;
		vv.z = 0.5f;
		vv.tx = v.tex_coord.x;
		vv.ty = v.tex_coord.y;
		vv.color[0]=v.colour.red;
		vv.color[1]=v.colour.green;
		vv.color[2]=v.colour.blue;
		vv.color[3]=v.colour.alpha;
		return vv;
	}
	struct RocketTexture {
		TexturePtr texture;
	};
	class RenderInterfaceImpl : public ::Rocket::Core::RenderInterface {
	private:
		Resources*	m_resources;
		mutable Graphics*	m_graphics;
		std::vector<GHL::Vertex> m_vertexes;
		std::vector<GHL::UInt16> m_indexes;
		Recti	m_scisor;
		mutable Recti	m_stored_scisor;
		mutable bool	m_scisor_enabled;
	public:
		explicit RenderInterfaceImpl(Resources* resources) : m_resources(resources){
			m_graphics = 0;
			m_scisor_enabled = false;
		}
		void BeginDraw( Graphics& g) const {
			m_graphics = &g;
			m_scisor_enabled = false;
			m_stored_scisor = g.GetClipRect();
		}
		void EndDraw() const {
			if (m_graphics) {
				m_graphics->SetClipRect(m_stored_scisor);
			}
			m_graphics = 0;
		}
		/// Called by Rocket when a texture is required by the library.
		/// @param[out] texture_handle The handle to write the texture handle for the loaded texture to.
		/// @param[out] texture_dimensions The variable to write the dimensions of the loaded texture.
		/// @param[in] source The application-defined image source, joined with the path of the referencing document.
		/// @return True if the load attempt succeeded and the handle and dimensions are valid, false if not.
		virtual bool LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source) {
			TexturePtr texture = m_resources->GetTexture(source.CString());
			if (texture) {
				RocketTexture* holder = new RocketTexture();
				holder->texture = texture;
				texture_handle = reinterpret_cast<Rocket::Core::TextureHandle> (holder);
				texture_dimensions.x = texture->GetWidth();
				texture_dimensions.y = texture->GetHeight();
				return true;
			} 
			return false;
		}
		/// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
		/// @param[out] texture_handle The handle to write the texture handle for the generated texture to.
		/// @param[in] source The raw 8-bit texture data. Each pixel is made up of four 8-bit values, indicating red, green, blue and alpha in that order.
		/// @param[in] source_dimensions The dimensions, in pixels, of the source data.
		/// @return True if the texture generation succeeded and the handle is valid, false if not.
		virtual bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions) {
			TexturePtr texture = m_resources->CreateTexture(source_dimensions.x,source_dimensions.y,source);
			if (texture) {
				RocketTexture* holder = new RocketTexture();
				holder->texture = texture;
				texture_handle = reinterpret_cast<Rocket::Core::TextureHandle> (holder);
				return true;
			} 
			return false;		
		}
		/// Called by Rocket when a loaded texture is no longer required.
		/// @param texture The texture handle to release.
		virtual void ReleaseTexture(Rocket::Core::TextureHandle texture) {
			RocketTexture* holder = reinterpret_cast<RocketTexture*> (texture);
			delete holder;
		}
		/// Called by Rocket when it wants to enable or disable scissoring to clip content.
		/// @param[in] enable True if scissoring is to enabled, false if it is to be disabled.
		virtual void EnableScissorRegion(bool enable) {
			if (!m_graphics) return;
			if (enable) {
				m_graphics->SetClipRect(m_scisor);
			} else {
				m_graphics->SetClipRect(Recti(0,0,m_graphics->GetScreenWidth(),m_graphics->GetScreenHeight()));
			}
			m_scisor_enabled = enable;
		}
		/// Called by Rocket when it wants to change the scissor region.
		/// @param[in] x The left-most pixel to be rendered. All pixels to the left of this should be clipped.
		/// @param[in] y The top-most pixel to be rendered. All pixels to the top of this should be clipped.
		/// @param[in] width The width of the scissored region. All pixels to the right of (x + width) should be clipped.
		/// @param[in] height The height of the scissored region. All pixels to below (y + height) should be clipped.
		virtual void SetScissorRegion(int x, int y, int width, int height) {
			m_scisor = Recti(x,y,width,height);
			if (m_scisor_enabled) {
				EnableScissorRegion(true);
			}
		}
		/// Called by Rocket when it wants to render geometry that the application does not wish to optimise. Note that
		/// Rocket renders everything as triangles.
		/// @param[in] vertices The geometry's vertex data.
		/// @param[in] num_vertices The number of vertices passed to the function.
		/// @param[in] indices The geometry's index data.
		/// @param[in] num_indices The number of indices passed to the function. This will always be a multiple of three.
		/// @param[in] texture The texture to be applied to the geometry. This may be NULL, in which case the geometry is untextured.
		/// @param[in] translation The translation to apply to the geometry.
		virtual void RenderGeometry(::Rocket::Core::Vertex* vertices, 
									int num_vertices, 
									int* indices, 
									int num_indices, 
									::Rocket::Core::TextureHandle texture, 
									const ::Rocket::Core::Vector2f& translation){
			if (!m_graphics) return;
			m_vertexes.clear();
			for (int i=0;i<num_vertices;i++) {
				m_vertexes.push_back( convert_verex( vertices[i] ));
			}
			m_indexes.clear();
			for (int i=0;i<num_indices;i++) {
				m_indexes.push_back(indices[i]);
			}
			Transform2d transform = m_graphics->GetTransform();
			m_graphics->SetTransform(transform.translated(translation.x,translation.y));
			RocketTexture* holder = reinterpret_cast<RocketTexture*> (texture);
			m_graphics->DrawBuffer(holder ? holder->texture : TexturePtr(),GHL::PRIMITIVE_TYPE_TRIANGLES,
										m_vertexes,
										m_indexes);
			m_graphics->SetTransform(transform);
		}
	};
	
	struct RocketLib::Data {
		FileInterfaceImpl file;
		SystemInterfaceImpl system;
		RenderInterfaceImpl render;
		Data( Resources* resources ) : file(resources),render(resources) {
		}
	};
	
	template <class T> 
	struct  RocketDestroyHelper {
		static void destroy( const void* ptr ) {
			if (!ptr) return;
			const T* cont_ptr = reinterpret_cast< const T* >( ptr );
			const_cast< T* >(cont_ptr)->RemoveReference();
		}
	};
	
	RocketLib::RocketLib( Resources* resources ) {
		m_data = new Data( resources);
		::Rocket::Core::SetFileInterface( &(m_data->file) );
		::Rocket::Core::SetSystemInterface( &(m_data->system) );
		::Rocket::Core::SetRenderInterface( &(m_data->render) );
		::Rocket::Core::Initialise();
		::Rocket::Controls::Initialise();
		m_context = RocketContextPtr( ::Rocket::Core::CreateContext("main",
																	Rocket::Core::Vector2i(
																						   resources->GetRender()->GetWidth(), 
																						   resources->GetRender()->GetHeight())),
									 &RocketDestroyHelper<Rocket::Core::Context>::destroy );
#ifdef SB_DEBUG
		Rocket::Debugger::Initialise(m_context.get());
#endif
	};
	
	void RocketLib::SetBasePath( const char* path ) {
		m_data->file.SetBasePath( path );
	}
	void RocketLib::Draw( Graphics& g ) const {
		if (m_context) {
			BlendMode blend = g.GetBlendMode();
			g.SetBlendMode(BLEND_MODE_ALPHABLEND);
			m_data->render.BeginDraw( g );
			m_context->Render();
			m_data->render.EndDraw();
			g.SetBlendMode(blend);
		}
	}
	
	bool RocketLib::LoadFont( const char* path ) {
		return Rocket::Core::FontDatabase::LoadFontFace( path );
	}
	RocketDocumentPtr RocketLib::LoadDocument( const char* file ) {
		if (m_context) {
			return RocketDocumentPtr( m_context->LoadDocument( file ), &RocketDestroyHelper<Rocket::Core::ElementDocument>::destroy );
		}
		return RocketDocumentPtr();
	}
	
	RocketLib::~RocketLib() {
		m_context = RocketContextPtr();
		::Rocket::Core::Shutdown();
		delete m_data;
	}
	
	static inline int convert_modifiers( GHL::UInt32 mods ) {
		int res = 0;
		if (mods&GHL::KEYMOD_SHIFT) res|=Rocket::Core::Input::KM_SHIFT;
		if (mods&GHL::KEYMOD_ALT) res|=Rocket::Core::Input::KM_ALT;
		if (mods&GHL::KEYMOD_CTRL) res|=Rocket::Core::Input::KM_CTRL;
		if (mods&GHL::KEYMOD_COMMAND) res|=Rocket::Core::Input::KM_META;
		return res;
	}
	static inline int convert_mouse_button( GHL::MouseButton btn ) {
		if (btn==GHL::MOUSE_BUTTON_LEFT)
			return 0;
		if (btn==GHL::MOUSE_BUTTON_RIGHT)
			return 1;
		return 0;
	}
	
	void RocketLib::OnMouseDown( GHL::MouseButton key, int x,int y, GHL::UInt32 mods) {
		m_context->ProcessMouseMove( x,y, convert_modifiers(mods) ); 
		m_context->ProcessMouseButtonDown( convert_mouse_button(key), convert_modifiers(mods) );
	}
	void RocketLib::OnMouseMove( int x,int y, GHL::UInt32 mods ) {
		m_context->ProcessMouseMove( x,y, convert_modifiers(mods) );
	}
	void RocketLib::OnMouseUp( GHL::MouseButton key, int x,int y, GHL::UInt32 mods) {
		m_context->ProcessMouseMove( x,y, convert_modifiers(mods) );
		m_context->ProcessMouseButtonUp( convert_mouse_button(key), convert_modifiers(mods) );
	}
	
	void RocketLib::SetDebuggerVisible( bool v) {
#ifdef SB_DEBUG
		Rocket::Debugger::SetVisible( v );
#endif
	}
	bool RocketLib::GetDebuggerVisible() const {
#ifdef SB_DEBUG
		return Rocket::Debugger::IsVisible();
#else
		return false;
#endif
	}
	
}