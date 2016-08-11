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
#include <ghl_data.h>

#include "sb_atlaser.h"
#include "sb_bitmask.h"

#include "sb_log.h"



namespace Sandbox {

    static const char* MODULE = "Sandbox:Resources";
	
	
	Resources::Resources(GHL::VFS* vfs) :
		m_vfs(vfs),m_render(0),m_image(0) {
			
        m_memory_limit = 20 * 1024 * 1024;
        m_memory_used = 0;
        m_scale = 1.0f;
	}
	
    void Resources::Init(GHL::Render* render,GHL::ImageDecoder* image) {
        m_render = render;
        m_image = image;
        m_cache_path = m_vfs->GetDir(GHL::DIR_TYPE_CACHE);
    }
	Resources::~Resources() {
		
	} 
	
	void Resources::SetBasePath(const char* path) {
		m_base_path = path;
	}

	GHL::DataStream* Resources::OpenFile(const char* filename) {
		std::string fn = m_base_path + filename;
        return m_vfs->OpenFile(fn.c_str());
	}
    
    GHL::WriteStream* Resources::OpenWrite(const char* filename,bool remove) {
        if (remove) {
            m_vfs->DoRemoveFile(filename);
        }
        return m_vfs->OpenFileWrite(filename);
    }
    
    GHL::DataStream* Resources::OpenFileVariant(const char* fn,bool& variant) {
        variant = false;
        GHL::DataStream* ds = 0;
        if (!m_res_variant.empty()) {
            const char* dotpos = ::strchr(fn, '.');
            if (dotpos) {
                sb::string var = sb::string(fn,dotpos) + m_res_variant + "." + sb::string(dotpos+1);
                ds = OpenFile(var.c_str());
                if (ds) {
                    variant = true;
                    return ds;
                }
            }
        }
        ds = OpenFile(fn);
        if (!ds) {
            //LogError(MODULE) << "Failed open file " << fn;
        }
        return ds;
    }
	
    GHL::Texture* Resources::InternalCreateTexture( int w,int h, bool alpha,bool fill) {
        if (!m_render) {
            LogError(MODULE) << "render not initialized";
            return 0;
        }
		GHL::TextureFormat tfmt;
		int bpp = 4;
		if (alpha) {
			tfmt = GHL::TEXTURE_FORMAT_RGBA;
			bpp = 4;
		} else {
			tfmt = GHL::TEXTURE_FORMAT_RGB;
			bpp = 3;
        }
        GHL::Image* img = 0;
        if ( fill ) {
            img = GHL_CreateImage(w, h, alpha ? GHL::IMAGE_FORMAT_RGBA : GHL::IMAGE_FORMAT_RGB);
            img->Fill(0);
        }
		GHL::Texture* texture = m_render->CreateTexture(w,h,tfmt,img);
		if (img) img->Release();
		return texture;
	}
    
    bool Resources::LoadImageSubdivs(const char* filename, std::vector<Image>& output,GHL::UInt32& width,GHL::UInt32& height) {
        bool variant = false;
        GHL::Image* img = LoadImage(filename,variant);
        if (!img) return false;
        GHL::UInt32 w = img->GetWidth();
        GHL::UInt32 h = img->GetHeight();
        width = w;
        height = h;
        float scale = variant?1.0f/m_scale:1.0f;
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
                GHL::Texture* texture = InternalCreateTexture(pw, ph, ImageHaveAlpha(img), false);
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
                texture->SetData(0,0,subimg);
                subimg->Release();
                output.push_back(Image(TexturePtr(new Texture(texture,scale)),0,0,float(ipw)*scale,float(iph)*scale));
                output.back().SetHotspot(Vector2f(-float(x)*scale,-float(y)*scale));
                x+=ipw;
                texture->DiscardInternal();
            }
            y+=iph;
        }
        img->Release();
        if (variant) {
            width = float(width)/m_scale;
            height = float(height)/m_scale;
        }
        return true;
    }
    
    GHL::Image* Resources::ImageFromStream( GHL::DataStream* ds ) {
        if (!m_image) {
            LogError(MODULE) << "image decoder not initialized";
            return 0;
        }
        GHL::Image* img = m_image->Decode(ds);
        return img;
    }
    
	GHL::Image* Resources::LoadImage(const char* filename,bool &variant,const char** ext) {
        
        if (!m_vfs) {
            LogError(MODULE) << "VFS not initialized";
            return 0;
        }
		std::string fn = filename;
		GHL::DataStream* ds = 0;
        size_t last_dot = fn.find_last_of('.');
        size_t last_slash = fn.find_last_of('/');
        
		if (last_dot!=fn.npos && ( last_slash == fn.npos || last_dot > last_slash) ) {
            ds = OpenFileVariant(filename,variant);
		}
		if (!ds) {
			std::string file = fn+".png";
			ds = OpenFileVariant( file.c_str() , variant );
			if ( !ds ) {
				file = fn+".jpg";
				ds = OpenFileVariant( file.c_str() , variant);
				if ( !ds ) {
					LogError(MODULE) <<"failed opening file " << fn;
					return 0;
				} else {
                    if (ext) *ext = "jpg";
                }
			} else {
                if (ext) *ext = "png";
            }
		}
		GHL::Image* img = ImageFromStream(ds);
		if (!img) {
			ds->Release();
			LogError(MODULE) <<"error decoding file " << filename;
			return 0;
		}
		ds->Release();
		return img;
	}
    
    bool Resources::GetImageInfo(sb::string& file,bool &variant,GHL::UInt32& w,GHL::UInt32& h) {
        if (!m_image) {
            LogError(MODULE) << "image decoder not initialized";
            return false;
        }
        if (!m_vfs) {
            LogError(MODULE) << "VFS not initialized";
            return false;
        }
        
        std::string fn = file;
		GHL::DataStream* ds = 0;
        size_t dotpos = file.find_last_of('.');
		if (dotpos!=file.npos) {
			ds = OpenFileVariant(fn.c_str(),variant);
            fn.resize(dotpos);
        }
		if (!ds) {
			std::string ifile = fn+".png";
			ds = OpenFileVariant( ifile.c_str() , variant);
			if ( !ds ) {
				ifile = fn+".jpg";
				ds = OpenFileVariant( ifile.c_str() , variant );
				if ( !ds ) {
					LogError(MODULE) <<"failed opening file " << fn;
					return 0;
				} else {
                    file = ifile;
                }
			} else {
                file = ifile;
            }
		}
        GHL::ImageInfo info;
        if (!m_image->GetFileInfo(ds, &info)) {
            ds->Release();
            LogError(MODULE) <<"error getting image info for file " << fn;
			return false;
        }
        ds->Release();
        w = info.width;
        h = info.height;
		return true;
    }
    
	TexturePtr Resources::CreateTexture( GHL::UInt32 w, GHL::UInt32 h,float scale,bool alpha, const GHL::Image* data) {
        GHL::UInt32 tw = 0;
        GHL::UInt32 th = 0;
        GetTextureSize(w, h, tw, th, false);
        bool setData = ( tw == w ) && ( th == h );
		GHL::Texture* texture = m_render->CreateTexture(tw,
                                                        th,alpha ? GHL::TEXTURE_FORMAT_RGBA:GHL::TEXTURE_FORMAT_RGB,
                                                        setData ? data : 0);
		if (data && !setData) texture->SetData(0,0,data);
        return TexturePtr( new Texture(texture,scale,w,h));
	}
	
	
	TexturePtr Resources::GetTexture(const char* filename, bool need_premultiply) {
        if (!filename)
            return TexturePtr();
        
        sb::string fn = filename;
        
        if (TexturePtr tex = m_textures[fn]) {
			return tex;
		}
    
        GHL::UInt32 img_w = 0;
        GHL::UInt32 img_h = 0;
        bool variant = false;
        
        sb::string original_name = fn;
        
        if (!GetImageInfo(fn,variant,img_w,img_h)) {
            return TexturePtr();
        }
        
        TexturePtr ptr = TexturePtr(new Texture(fn, (variant?1.0/m_scale:1.0), need_premultiply,img_w,img_h));
		
        GHL::UInt32 tw = 0;
        GHL::UInt32 th = 0;
        
        GetTextureSize(img_w, img_h, tw, th,false);
        
        ptr->SetTextureSize(tw,th);
        
		m_textures[original_name]=ptr;
        return ptr;
	}
    
    void Resources::GetTextureSize( GHL::UInt32 w,GHL::UInt32 h, GHL::UInt32& tw, GHL::UInt32& th , bool target) const {
        sb_assert(m_render);
        if (m_render->IsFeatureSupported(target ? GHL::RENDER_FEATURE_NPOT_TARGET : GHL::RENDER_FEATURE_NPOT_TEXTURES)) {
            tw = w;
            th = h;
        } else {
            tw = next_pot( w );
            th = next_pot( h );
        }
    }
    GHL::Texture* Resources::CreateTexture( GHL::Image* img , bool premultiply, const sb::string& filename,const sb::string& ext) {
        GHL::TextureFormat tfmt;
        int bpp = 4;
		if (img->GetFormat()==GHL::IMAGE_FORMAT_RGB) {
			tfmt = GHL::TEXTURE_FORMAT_RGB;
            bpp = 3;
		} else if (img->GetFormat()==GHL::IMAGE_FORMAT_RGBA) {
			tfmt = GHL::TEXTURE_FORMAT_RGBA;
            if (premultiply) {
                img->PremultiplyAlpha();
            }
            bpp = 4;
		} else {
			LogError(MODULE) <<"unsupported format file " << filename;
			return 0;
		}
		GHL::UInt32 tw = 0;
        GHL::UInt32 th = 0;
        
        GetTextureSize(img->GetWidth(), img->GetHeight(), tw, th,false);
        
        size_t mem = tw * th * bpp;
        size_t need_release = 0;
        if ((m_memory_used+mem)>m_memory_limit) {
            need_release = m_memory_used + mem - m_memory_limit;
        }
        if (need_release) {
            FreeMemory(need_release, false);
        }
        
        bool setData = ( tw == img->GetWidth() ) && ( th == img->GetHeight() );
		GHL::Image* fillData = setData ? 0 : GHL_CreateImage(tw,th,img->GetFormat());
        if (fillData) fillData->Fill(0);
        GHL::Texture* texture = m_render->CreateTexture(tw,th,tfmt,setData ? img : fillData);
		tfmt = texture->GetFormat();
		GHL::ImageFormat ifmt;
		if (tfmt==GHL::TEXTURE_FORMAT_RGB) {
			ifmt = GHL::IMAGE_FORMAT_RGB;
			bpp = 3;
		}
		else if (tfmt==GHL::TEXTURE_FORMAT_RGBA) {
			ifmt = GHL::IMAGE_FORMAT_RGBA;
			bpp = 4;
		}
		
        if (!setData) {
            fillData->Release();
            //LogWarning(MODULE) << "image " << filename << "." << ext<< " NPOT " <<
            //img->GetWidth() << "x" << img->GetHeight() << " -> " <<
            //tw << "x" << th;
            img->Convert(ifmt);
            texture->SetData(0,0,img);
        } else {
			//LogInfo(MODULE) << "Loaded image : " << filename << "." << ext << " " << img->GetWidth() << "x" << img->GetHeight() ;
        }
        texture->DiscardInternal();
        return texture;
    }
    GHL::Texture* Resources::LoadTexture( const sb::string& filename , bool& variant, bool premultiply ) {
        LogDebug() << "load texture " << filename;
        const char* ext = "";
		GHL::Image* img = LoadImage(filename.c_str(),variant,&ext);
		if (!img) {
			return 0;
		}
        
        GHL::Texture* texture = CreateTexture(img, premultiply, filename, ext);
		
        img->Release();
        
        if (texture) {
            m_memory_used += texture->GetMemoryUsage();
        }
        
        return texture;
    }
    
    BitmaskPtr Resources::LoadBitmask( const sb::string& filename ) {
        const char* ext = "";
        bool variant = false;
        GHL::Image* img = LoadImage(filename.c_str(),variant,&ext);
        if (!img) {
            return BitmaskPtr();
        }
        return BitmaskPtr(new Bitmask(img));
    }
    
	bool Resources::ImageHaveAlpha(const GHL::Image* img) const {
		return img->GetFormat()==GHL::IMAGE_FORMAT_RGBA;
	}
	
	bool Resources::ConvertImage(GHL::Image* img,GHL::Texture* tex) const {
		GHL::TextureFormat tfmt = tex->GetFormat();
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
	
	ImagePtr Resources::GetImage(const char* filename,bool need_premultiply) {
        TexturePtr texture = GetTexture( filename , need_premultiply);
        if (!texture) {
            return ImagePtr();
        }
		GHL::UInt32 imgW = texture->GetOriginalWidth();
		GHL::UInt32 imgH = texture->GetOriginalHeight();
        return ImagePtr( new Image(texture,0,0,float(imgW),float(imgH)));
	}
    
    GHL::Image* Resources::ImageFromData( const GHL::Data* data ) {
        GHL::DataStream* ds = GHL_CreateMemoryStream(data);
        if (!ds)
            return 0;
        GHL::Image* img = ImageFromStream(ds);
        ds->Release();
        return img;
    }
    
    ImagePtr Resources::CreateImageFromData( const GHL::Data* data ) {
        GHL::Image* img = ImageFromData(data);
        if (!img) {
            LogError() << "failed create image from data";
            return ImagePtr();
        }
        GHL::Texture* texture = CreateTexture(img, true, "data", "mem");
        GHL::UInt32 img_w = img->GetWidth();
        GHL::UInt32 img_h = img->GetHeight();
        img->Release();
        if (!texture) {
            LogError() << "failed create texture from data";
            return ImagePtr();
        }
        
        
        TexturePtr tptr = TexturePtr(new Texture(texture,1.0,img_w,img_h));
		
        return ImagePtr( new Image(tptr,0,0,float(img_w),float(img_h)));
        
    }
    
	ShaderPtr Resources::GetShader(const char* vfn,const char* ffn) {
		std::string vfilename = vfn;
		std::string ffilename = ffn;
        
        
		std::string name = vfilename+"+"+ffilename;
        if (ShaderPtr sh = m_shaders[name]) {
			return sh;
		}

		GHL::VertexShader* vs = 0;
		GHL::FragmentShader* fs = 0;

		{
			std::map<std::string,GHL::VertexShader*>::iterator it = m_vshaders.find(vfilename);
			if (it!=m_vshaders.end()) vs = it->second;
		}
		{
			std::map<std::string,GHL::FragmentShader*>::iterator it = m_fshaders.find(ffilename);
			if (it!=m_fshaders.end()) fs = it->second;
		}

		if (!vs) {
			GHL::DataStream* ds = this->OpenFile(vfilename.c_str());
			if (!ds) {
				LogError(MODULE) << "error opening file " << vfilename;
				return ShaderPtr();
			}
            GHL::Data* dsd = GHL_ReadAllData( ds );
            ds->Release();
            if (!dsd) {
                LogError(MODULE) << "error loading shader " << vfilename;
            }
			vs = m_render->CreateVertexShader(dsd);
			dsd->Release();
			if (!vs) {
				LogError(MODULE) << "error create shader " << vfilename;
				//return ShaderPtr();
			}
			m_vshaders[vfilename]=vs;
		}
		if (!fs) {
			GHL::DataStream* ds = this->OpenFile(ffilename.c_str());
			if (!ds) {
				LogError(MODULE) << "error opening file " << ffilename;
				return ShaderPtr();
			}
            GHL::Data* dsd = GHL_ReadAllData( ds );
            ds->Release();
            if (!dsd) {
                LogError(MODULE) << "error loading shader " << ffilename;
            }
			fs = m_render->CreateFragmentShader(dsd);
			dsd->Release();
			if (!fs) {
				LogError(MODULE) << "error creating shader " << ffilename;
				//return ShaderPtr();
			}
			m_fshaders[vfilename]=fs;
		}
		GHL::ShaderProgram* sp = m_render->CreateShaderProgram(vs,fs);
		if (!sp) {
			LogError(MODULE) << "error creating shader program from " << vfilename << " , " << ffilename ;
			//return ShaderPtr();
		}
		ShaderPtr res = ShaderPtr(new Shader(sp));
		m_shaders[name]=res;
		return res;
	}
	
	    
    RenderTargetPtr Resources::CreateRenderTarget(int w, int h, float scale, bool alpha, bool depth) {
        sb_assert(w>0);
        sb_assert(h>0);
        GHL::UInt32 nw = 0;
        GHL::UInt32 nh = 0;
        GetTextureSize(w*scale, h*scale, nw, nh,true);
        sb_assert(m_render);
        GHL::RenderTarget* rt = m_render->CreateRenderTarget(nw, nh, alpha ? GHL::TEXTURE_FORMAT_RGBA : GHL::TEXTURE_FORMAT_RGB, depth);
        /*
        if (rt) {
            m_render->BeginScene(rt);
            m_render->Clear(1, 0, 0, 1, 0);
            m_render->EndScene();
        }*/
        return RenderTargetPtr( new RenderTarget(rt,w*scale,h*scale,scale));
    }
    
    void Resources::SetResourcesVariant(float scale,const sb::string& postfix) {
        m_scale = scale;
        m_res_variant = postfix;
    }
    
    size_t    Resources::FreeMemory(size_t need_release,bool full) {
        size_t memory_used = 0;
        for (TexturesCacheMap::iterator it = m_textures.begin();it!=m_textures.end();) {
            TexturePtr& t = it->second;
            TexturesCacheMap::iterator next = it;
            ++next;
            if (t) {
                memory_used += t->GetMemoryUsage();
                if (need_release) {
                    size_t lt = t->GetLiveTicks();
                    if ( lt && lt < m_live_ticks ) {
                        
                        size_t released = t->Release();
                        memory_used-=released;
                        if (released>need_release) {
                            need_release = 0;
                        } else {
                            need_release -= released;
                        }
                        
                        if (t->unique()) {
                            t.reset();
                            m_textures.erase(it);
                        }
                    }
                } else {
                    if (!full) break;
                }
            } else {
               m_textures.erase(it);
            }
            it = next;
        }
        return memory_used;
    }
    
    void Resources::ReleaseAll() {
        for (TexturesCacheMap::iterator it = m_textures.begin();it!=m_textures.end();++it) {
            TexturePtr& t = it->second;
            if (t) {
                t->Release();
            }
        }
        m_textures.clear();
        m_shaders.clear();
        m_vshaders.clear();
        m_fshaders.clear();
        m_memory_used = 0;
    }

	void    Resources::ProcessMemoryMgmt() {
        ++m_live_ticks;
        size_t need_release = 0;
        if (m_memory_used>m_memory_limit) {
            need_release = m_memory_used - m_memory_limit;
        }
        m_memory_used = FreeMemory(need_release,true);
    }
}
