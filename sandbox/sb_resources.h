/*
 *  sb_resources.h
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_RESOURCES_H
#define SB_RESOURCES_H

#include "sb_image.h"
#include "sb_shader.h"
#include "sb_atlaser.h"
#include "sb_rendertatget.h"
#include <sbtl/sb_string.h>
#include <sbtl/sb_map.h>
#include <sbtl/sb_list.h>

namespace GHL {
	struct VFS;
	struct Render;
	struct Image;
	struct ImageDecoder;
	struct VertexShader;
	struct FragmentShader;
	struct DataStream;
}
namespace Sandbox {
	
#define SB_RESOURCES_CACHE
	
	class Atlaser;

	class Resources {
	public:
		Resources(GHL::VFS* vfs);
		~Resources();
        
        void Init(GHL::Render* render,GHL::ImageDecoder* image);
		
		void SetBasePath(const char* path);
		GHL::DataStream* OpenFile(const char* fn);
		
		TexturePtr GetTexture(const char* filename);
		ImagePtr GetImage(const char* filename);
		bool LoadImageSubdivs(const char* filename, sb::vector<Image>& output);
		
		ShaderPtr GetShader(const char* vfn,const char* ffn);
		
		GHL::Render* GetRender() { return m_render;}
		GHL::ImageDecoder* GetImageDecoder() {return m_image;}
		GHL::VFS* GetVFS() { return m_vfs;}
		const std::string& GetBasePath() const { return m_base_path;}
		
        sb::shared_ptr<Atlaser> CreateAtlaser(int w, int h);
		
		TexturePtr CreateTexture( GHL::UInt32 w, 
                                 GHL::UInt32 h, 
                                 bool alpha, 
                                 const GHL::Image* data);
        GHL::Texture* LoadTexture( const sb::string& filename );
        
        size_t  GetLiveTicks() const { return m_live_ticks; }
        
        size_t  GetMemoryLimit() const { return m_memory_limit; }
        size_t  GetMemoryUsed() const { return m_memory_used; }
        
        RenderTargetPtr CreateRenderTarget(int w, int h, bool alpha, bool depth);
        
        void    ProcessMemoryMgmt();
    private:
		GHL::VFS* m_vfs;
		GHL::Render* m_render;
		GHL::ImageDecoder* m_image;
		std::string	m_base_path;
		
		friend class Atlaser;
		
        GHL::Texture* InternalCreateTexture( int w,int h, bool alpha,bool fill);
		GHL::Image* LoadImage(const char* file,const char** ext = 0);
		bool ImageHaveAlpha(const GHL::Image* img) const;
		bool ConvertImage(GHL::Image* img,GHL::Texture* tex) const;
        bool GetImageInfo(sb::string& file,GHL::UInt32& w,GHL::UInt32& h);
#ifdef SB_RESOURCES_CACHE
		sb::map<sb::string,sb::weak_ptr<Texture> >  m_textures;
		sb::map<sb::string,GHL::VertexShader*>      m_vshaders;
		sb::map<sb::string,GHL::FragmentShader*>    m_fshaders;
		sb::map<sb::string,sb::weak_ptr<Shader> >   m_shaders;
#endif
        size_t    m_live_ticks;
        size_t    m_memory_limit;
        size_t    m_memory_used;
        sb::list<sb::weak_ptr<Texture> >   m_managed_textures;
        size_t    FreeMemory(size_t mem,bool full);
	};
}

#endif /*SB_RESOURCES_H*/
