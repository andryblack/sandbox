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
#include "sb_rendertarget.h"
#include "sb_file_provider.h"
#include <sbstd/sb_string.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_list.h>

namespace GHL {
	struct VFS;
	struct Render;
	struct Image;
	struct ImageDecoder;
	struct VertexShader;
	struct FragmentShader;
	struct DataStream;
    struct Data;
}
namespace Sandbox {
	
	
	class Atlaser;

	class Resources : public FileProvider {
	public:
		Resources(GHL::VFS* vfs);
		~Resources();
        
        void Init(GHL::Render* render,GHL::ImageDecoder* image);
		
		void SetBasePath(const char* path);
		GHL::DataStream* OpenFile(const char* fn);
        
        ImagePtr CreateImageFromData( const GHL::Data* data );
		
		TexturePtr GetTexture(const char* filename, bool need_premultiply);
		ImagePtr GetImage(const char* filename, bool need_premultiply);
		bool LoadImageSubdivs(const char* filename,
                              sb::vector<Image>& output,
                              GHL::UInt32& width,
                              GHL::UInt32& height);
		
		ShaderPtr GetShader(const char* vfn,const char* ffn);
		
		GHL::Render* GetRender() { return m_render;}
		GHL::ImageDecoder* GetImageDecoder() {return m_image;}
		GHL::VFS* GetVFS() { return m_vfs;}
		const std::string& GetBasePath() const { return m_base_path;}
		
		TexturePtr CreateTexture( GHL::UInt32 w, 
                                 GHL::UInt32 h, 
                                 bool alpha, 
                                 const GHL::Image* data);
        GHL::Texture* LoadTexture( const sb::string& filename , bool premultiply);
        
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

        typedef sb::map<sb::string,TexturePtr > TexturesCacheMap;
        TexturesCacheMap m_textures;
		sb::map<sb::string,GHL::VertexShader*>      m_vshaders;
		sb::map<sb::string,GHL::FragmentShader*>    m_fshaders;
		sb::map<sb::string,ShaderPtr >   m_shaders;

        size_t    m_live_ticks;
        size_t    m_memory_limit;
        size_t    m_memory_used;
        size_t    FreeMemory(size_t mem,bool full);
        
        void GetTextureSize( GHL::UInt32 w,GHL::UInt32 h, GHL::UInt32& tw, GHL::UInt32& th ) const;
        
        GHL::Image* ImageFromData( const GHL::Data* data );
        GHL::Image* ImageFromStream( GHL::DataStream* ds );
        
        GHL::Texture* CreateTexture( GHL::Image* img , bool premultiply, const sb::string& file, const sb::string& ext);
	};
}

#endif /*SB_RESOURCES_H*/
