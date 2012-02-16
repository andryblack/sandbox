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

#include "sb_atlaser.h"

#include "sb_log.h"



namespace Sandbox {

    static const char* MODULE = "Sandbox:Resources";
	
	
	
	Resources::Resources(GHL::VFS* vfs) :
		m_vfs(vfs),m_render(0),m_image(0) {
			
	}
	
    void Resources::Init(GHL::Render* render,GHL::ImageDecoder* image) {
        m_render = render;
        m_image = image;
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
			LogError(MODULE) << "Failed open file " << filename;
			LogError(MODULE) << "full path : " << fn;
		}
		return ds;
	}
	
	TexturePtr Resources::InternalCreateTexture( int w,int h, bool alpha,bool fill) {
        if (!m_render) {
            LogError(MODULE) << "render not initialized";
            return TexturePtr();
        }
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
    bool Resources::LoadImageSubdivs(const char* filename, std::vector<Image>& output) {
        GHL::Image* img = LoadImage(filename);
        if (!img) return false;
        GHL::UInt32 w = img->GetWidth();
        GHL::UInt32 h = img->GetHeight();
        GHL::UInt32 y = 0;
        while ( y < h ) {
            GHL::UInt32 oh = h - y;
            GHL::UInt32 ph = oh <= 16 ? oh : prev_pot( oh );
            GHL::UInt32 iph = ph > oh ? oh : ph;
            GHL::UInt32 x = 0;
            while ( x < w ) {
                GHL::UInt32 ow = w - x;
                GHL::UInt32 pw = ow <= 16 ? ow : prev_pot( ow );
                GHL::UInt32 ipw = pw > ow ? ow : pw;
                TexturePtr texture = InternalCreateTexture(pw, ph, ImageHaveAlpha(img), false);
                if (!texture || !ConvertImage(img,texture)) {
                    LogError(MODULE) << "failed load subdiv image " << filename;
                    img->Release();
                    return false;
                }
                GHL::Image* subimg = img->SubImage(x, y, ipw, iph);
                if (!subimg) {
                    LogError(MODULE) << "failed load subdiv image " << filename;
                    img->Release();
                    return false;
                }
                texture->GetNative()->SetData(0,0,ipw,iph,subimg->GetData());
                subimg->Release();
                output.push_back(Image(texture,0,0,float(ipw),float(iph)));
                output.back().SetHotspot(Vector2f(-float(x),-float(y)));
                x+=ipw;
            }
            y+=iph;
        }
        return true;
    }
	GHL::Image* Resources::LoadImage(const char* filename) {
        if (!m_image) {
            LogError(MODULE) << "image decoder not initialized";
            return 0;
        }
		std::string fn = m_base_path + filename;
		GHL::DataStream* ds = m_vfs->OpenFile(fn.c_str());
		if (!ds) {
			std::string file = fn+".png";
			ds = m_vfs->OpenFile( file.c_str() );
			if ( !ds ) {
				file = fn+".tga";
				ds = m_vfs->OpenFile( file.c_str() );
				if ( !ds ) {
					LogError(MODULE) <<"error opening file " << fn;
					return 0;
				}
			}
		}
		GHL::Image* img = m_image->Decode(ds);
		if (!img) {
			ds->Release();
			LogError(MODULE) <<"error decoding file " << filename;
			return 0;
		}
		ds->Release();
		return img;
	}
	TexturePtr Resources::CreateTexture( GHL::UInt32 w, GHL::UInt32 h,bool alpha, const GHL::Byte* data) {
		GHL::Texture* texture = m_render->CreateTexture(next_pot(w),next_pot(h),alpha ? GHL::TEXTURE_FORMAT_RGBA:GHL::TEXTURE_FORMAT_RGB,false);
		if (data) texture->SetData(0,0,w,h,data);
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
			LogError(MODULE) <<"unsupported format file " << filename;
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
			LogWarning(MODULE) << "image " << filename << " NPOT " << 
				img->GetWidth() << "x" << img->GetHeight() << " -> " <<
				texture->GetWidth() << "x" << texture->GetHeight();
			GHL::Byte* data = new GHL::Byte[ bpp*texture->GetWidth()*texture->GetHeight() ];
			std::fill(data,data+bpp*texture->GetWidth()*texture->GetHeight(),0);
			texture->SetData(0,0,texture->GetWidth(),texture->GetHeight(),data);
			delete [] data;
		} else {
			LogInfo(MODULE) << "Loaded image : " << filename << " " << img->GetWidth() << "x" << img->GetHeight() ;
		}
		
		img->Convert(ifmt);
		
		texture->SetData(0,0,img->GetWidth(),img->GetHeight(),img->GetData());
		TexturePtr ptr(new Texture(texture,img->GetWidth(),img->GetHeight()));
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
			LogError(MODULE) <<  "unexpected texture format";
			return false;
		}

		img->Convert(ifmt);
		return true;
	}
	
	ImagePtr Resources::GetImage(const char* filename) {
        TexturePtr texture = GetTexture( filename );
        if (!texture) {
            return ImagePtr();
        }
		float imgW = texture->GetOriginalWidth();
		float imgH = texture->GetOriginalHeight();
		return ImagePtr(new Image(texture,0,0,imgW,imgH));
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
				LogError(MODULE) << "error opening file " << filename;
				return ShaderPtr();
			}
			vs = m_render->CreateVertexShader(ds);
			ds->Release();
			if (!vs) {
				LogError(MODULE) << "error loading shader " << vfilename;
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
				LogError(MODULE) << "error opening file " << filename;
				return ShaderPtr();
			}
			fs = m_render->CreateFragmentShader(ds);
			ds->Release();
			if (!fs) {
				LogError(MODULE) << "error loading shader " << ffilename;
				//return ShaderPtr();
			}
#ifdef SB_RESOURCES_CACHE
			m_fshaders[vfilename]=fs;
#endif
		}
		GHL::ShaderProgram* sp = m_render->CreateShaderProgram(vs,fs);
		if (!sp) {
			LogError(MODULE) << "error creating shader program from " << vfilename << " , " << ffilename ;
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
