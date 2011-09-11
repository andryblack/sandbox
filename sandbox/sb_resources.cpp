/*
 *  sb_resources.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_resources.h"
#include <ghl_vfs.h>
#include <ghl_render.h>
#include <ghl_image_decoder.h>
#include <ghl_data_stream.h>
#include <ghl_texture.h>
#include <ghl_image.h>
#include <ghl_shader.h>
#include <iostream>

#include "sb_atlaser.h"



namespace Sandbox {

	
	
	
	Resources::Resources(GHL::VFS* vfs,GHL::Render* render,GHL::ImageDecoder* image) :
		m_vfs(vfs),m_render(render),m_image(image) {
			
	}
	
	Resources::~Resources() {
		
	} 
	
	void Resources::SetBasePath(const char* path) {
		m_base_path = path;
	}

	GHL::DataStream* Resources::OpenFile(const char* filename) {
		std::string fn = m_base_path + filename;
		GHL::DataStream* ds = m_vfs->OpenFile(fn.c_str());
		if (!ds) {
			std::cout << "[RESOURCES] Failed open file " << filename << std::endl;
			std::cout << "[RESOURCES] full path : " << fn << std::endl;
		}
		return ds;
	}
	
	TexturePtr Resources::CreateTexture( int w,int h, bool alpha,bool fill) {
		GHL::TextureFormat tfmt;
		if (alpha) {
			tfmt = GHL::TEXTURE_FORMAT_RGBA;
		} else {
			tfmt = GHL::TEXTURE_FORMAT_RGB;
		}
		GHL::Texture* texture = m_render->CreateTexture(w,h,tfmt,false);
		tfmt = texture->GetFormat();
		size_t bpp = 4;
		if (tfmt==GHL::TEXTURE_FORMAT_RGB) {
			bpp = 3;
		}
		else if (tfmt==GHL::TEXTURE_FORMAT_RGBA) {
			bpp = 4;
		}
		
		if (fill) {
			GHL::Byte* data = new GHL::Byte[ bpp*texture->GetWidth()*texture->GetHeight() ];
			std::fill(data,data+bpp*texture->GetWidth()*texture->GetHeight(),0);
			texture->SetData(0,0,texture->GetWidth(),texture->GetHeight(),data);
			delete [] data;
		} 
		return TexturePtr(new Texture(texture));
	}
	GHL::Image* Resources::LoadImage(const char* filename) {
		std::string fn = m_base_path + filename;
		GHL::DataStream* ds = m_vfs->OpenFile(fn.c_str());
		if (!ds) {
			std::string file = fn+".png";
			ds = m_vfs->OpenFile( file.c_str() );
			if ( !ds ) {
				file = fn+".tga";
				ds = m_vfs->OpenFile( file.c_str() );
				if ( !ds ) {
					std::cout << "[RESOURCES] error opening file " << fn << std::endl;
					return 0;
				}
			}
		}
		GHL::Image* img = m_image->Decode(ds);
		if (!img) {
			ds->Release();
			std::cout << "[RESOURCES] error decoding file " << filename << std::endl;
			return 0;
		}
		ds->Release();
		return img;
	}
	TexturePtr Resources::CreateTexture( GHL::UInt32 w, GHL::UInt32 h, const GHL::Byte* data) {
		GHL::Texture* texture = m_render->CreateTexture(next_pot(w),next_pot(h),GHL::TEXTURE_FORMAT_RGBA,false);
		texture->SetData(0,0,w,h,data);
		TexturePtr ptr(new Texture(texture));
		return ptr;
	}
	TexturePtr Resources::GetTexture(const char* filename) {
		
#ifdef SB_RESOURCES_CACHE
		weak_ptr<Texture> al = m_textures[filename];
		if (TexturePtr tex = al.lock()) {
			return tex;
		}
#endif
		GHL::Image* img = LoadImage(filename);
		if (!img) {
			return TexturePtr();
		}
		GHL::TextureFormat tfmt;
		if (img->GetFormat()==GHL::IMAGE_FORMAT_RGB) {
			tfmt = GHL::TEXTURE_FORMAT_RGB;
		} else if (img->GetFormat()==GHL::IMAGE_FORMAT_RGBA) {
			tfmt = GHL::TEXTURE_FORMAT_RGBA;
		} else {
			img->Release();
			std::cout << "[RESOURCES] unsupported format file " << filename << std::endl;
			return TexturePtr();
		}
		GHL::Texture* texture = m_render->CreateTexture(next_pot(img->GetWidth()),next_pot(img->GetHeight()),tfmt,false);
		tfmt = texture->GetFormat();
		GHL::ImageFormat ifmt;
		size_t bpp = 4;
		if (tfmt==GHL::TEXTURE_FORMAT_RGB) {
			ifmt = GHL::IMAGE_FORMAT_RGB;
			bpp = 3;
		}
		else if (tfmt==GHL::TEXTURE_FORMAT_RGBA) {
			ifmt = GHL::IMAGE_FORMAT_RGBA;
			bpp = 4;
		}
		
		if (texture->GetWidth()!=img->GetWidth() || texture->GetHeight()!=img->GetHeight()) {
			std::cout << "[RESOURCES] Warning image " << filename << " NPOT " << 
				img->GetWidth() << "x" << img->GetHeight() << " -> " <<
				texture->GetWidth() << "x" << texture->GetHeight() << std::endl;
			GHL::Byte* data = new GHL::Byte[ bpp*texture->GetWidth()*texture->GetHeight() ];
			std::fill(data,data+bpp*texture->GetWidth()*texture->GetHeight(),0);
			texture->SetData(0,0,texture->GetWidth(),texture->GetHeight(),data);
			delete [] data;
		} else {
			std::cout << "[RESOURCES] Loaded image : " << filename << " " << img->GetWidth() << "x" << img->GetHeight() << std::endl;
		}
		
		img->Convert(ifmt);
		
		texture->SetData(0,0,img->GetWidth(),img->GetHeight(),img->GetData());
		TexturePtr ptr(new Texture(texture));
		img->Release();
#ifdef SB_RESOURCES_CACHE
		m_textures[filename]=ptr;
#endif
		return ptr;
	}
	bool Resources::ImageHaveAlpha(const GHL::Image* img) const {
		return img->GetFormat()==GHL::IMAGE_FORMAT_RGBA;
	}
	
	bool Resources::ConvertImage(GHL::Image* img,const TexturePtr& tex) const {
		GHL::TextureFormat tfmt = tex->GetNative()->GetFormat();
		GHL::ImageFormat ifmt;
		if (tfmt==GHL::TEXTURE_FORMAT_RGB) {
			ifmt = GHL::IMAGE_FORMAT_RGB;
		}
		else if (tfmt==GHL::TEXTURE_FORMAT_RGBA) {
			ifmt = GHL::IMAGE_FORMAT_RGBA;
		} else {
			std::cout << "[RESOURCES] unexpected texture format" << std::endl;
			return false;
		}

		img->Convert(ifmt);
		return true;
	}
	
	ImagePtr Resources::GetImage(const char* filename) {
		GHL::Image* img = LoadImage(filename);
		if (!img) return ImagePtr();
		GHL::UInt32 imgW = img->GetWidth();
		GHL::UInt32 imgH = img->GetHeight();
		GHL::UInt32 texW = next_pot(imgW);
		GHL::UInt32 texH = next_pot(imgH);
		TexturePtr texture = CreateTexture(texW, texH, ImageHaveAlpha(img), (texW!=imgW)||(texH!=imgH));
		if (!texture || !ConvertImage(img,texture)) {
			std::cout << "[RESOURCES] failed load image " << filename << std::endl;
			img->Release();
			return ImagePtr();
		}
		texture->GetNative()->SetData(0,0,imgW,imgH,img->GetData());
		img->Release();
		return ImagePtr(new Image(texture,0,0,float(imgW),float(imgH)));
	}
	
	ShaderPtr Resources::GetShader(const char* vfn,const char* ffn) {
		std::string vfilename = vfn;
		std::string ffilename = ffn;
#ifdef SB_RESOURCES_CACHE
		std::string name = vfilename+"+"+ffilename;
		weak_ptr<Shader> al = m_shaders[name];
		if (ShaderPtr sh = al.lock()) {
			return sh;
		}
#endif
		GHL::VertexShader* vs = 0;
		GHL::FragmentShader* fs = 0;
#ifdef SB_RESOURCES_CACHE
		{
			std::map<std::string,GHL::VertexShader*>::iterator it = m_vshaders.find(vfilename);
			if (it!=m_vshaders.end()) vs = it->second;
		}
		{
			std::map<std::string,GHL::FragmentShader*>::iterator it = m_fshaders.find(ffilename);
			if (it!=m_fshaders.end()) fs = it->second;
		}
#endif
		if (!vs) {
			std::string filename = m_base_path+vfilename;
			GHL::DataStream* ds = m_vfs->OpenFile(filename.c_str());
			if (!ds) {
				std::cout << "[RESOURCES] error opening file " << filename << std::endl;
				return ShaderPtr();
			}
			vs = m_render->CreateVertexShader(ds);
			ds->Release();
			if (!vs) {
				std::cout << "[RESOURCES] error loading shader " << vfilename << std::endl;
				//return ShaderPtr();
			}
#ifdef SB_RESOURCES_CACHE
			m_vshaders[vfilename]=vs;
#endif
		}
		if (!fs) {
			std::string filename = m_base_path+ffilename;
			GHL::DataStream* ds = m_vfs->OpenFile(filename.c_str());
			if (!ds) {
				std::cout << "[RESOURCES] error opening file " << filename << std::endl;
				return ShaderPtr();
			}
			fs = m_render->CreateFragmentShader(ds);
			ds->Release();
			if (!fs) {
				std::cout << "[RESOURCES] error loading shader " << ffilename << std::endl;
				//return ShaderPtr();
			}
#ifdef SB_RESOURCES_CACHE
			m_fshaders[vfilename]=fs;
#endif
		}
		GHL::ShaderProgram* sp = m_render->CreateShaderProgram(vs,fs);
		if (!sp) {
			std::cout << "[RESOURCES] error creating shader program from " << vfilename << " , " << ffilename << std::endl;
			//return ShaderPtr();
		}
		ShaderPtr res( new Shader(sp) );
#ifdef SB_RESOURCES_CACHE
		m_shaders[name]=res;
#endif
		return res;
	}
	
	
	shared_ptr<Atlaser> Resources::CreateAtlaser(int w,int h) {
		return shared_ptr<Atlaser>(new Atlaser(this,w,h));
	}
	
}
