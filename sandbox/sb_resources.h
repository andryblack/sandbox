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
#include <map>
#include <string>

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
		bool LoadImageSubdivs(const char* filename, std::vector<Image>& output);
		
		ShaderPtr GetShader(const char* vfn,const char* ffn);
		
		GHL::Render* GetRender() { return m_render;}
		GHL::ImageDecoder* GetImageDecoder() {return m_image;}
		GHL::VFS* GetVFS() { return m_vfs;}
		const std::string& GetBasePath() const { return m_base_path;}
		
		shared_ptr<Atlaser> CreateAtlaser(int w, int h);
		
		TexturePtr CreateTexture( GHL::UInt32 w, 
                                 GHL::UInt32 h, 
                                 bool alpha, 
                                 const GHL::Data* data);
	private:
		GHL::VFS* m_vfs;
		GHL::Render* m_render;
		GHL::ImageDecoder* m_image;
		std::string	m_base_path;
		
		friend class Atlaser;
		
		TexturePtr InternalCreateTexture( int w,int h, bool alpha,bool fill);
		GHL::Image* LoadImage(const char* file);
		bool ImageHaveAlpha(const GHL::Image* img) const;
		bool ConvertImage(GHL::Image* img,const TexturePtr& tex) const;
#ifdef SB_RESOURCES_CACHE
		std::map<std::string,weak_ptr<Texture> > m_textures;
		std::map<std::string,GHL::VertexShader*> m_vshaders;
		std::map<std::string,GHL::FragmentShader*> m_fshaders;
		std::map<std::string,weak_ptr<Shader> > m_shaders;
#endif
	};
}

#endif /*SB_RESOURCES_H*/
