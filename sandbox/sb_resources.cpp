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
#include "sb_data.h"

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
        if (!filename) return 0;
        sb::string fn = (filename[0] == '/') ? sb::string(filename) : (m_base_path + filename);
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
    
    static GHL::UInt32 get_subdiv_size( GHL::Render* render , GHL::UInt32 osize ) {
        if (osize >= 1024)
            return 1024;
        if (render->IsFeatureSupported(GHL::RENDER_FEATURE_NPOT_TEXTURES))
            return osize;
        return osize <= 16 ? osize : prev_pot(osize);
    }
    BackgroundDataPtr Resources::LoadBackground(const char* filename) {
        BackgroundDataPtr res;
        BackgroundsDataCache::iterator it = m_backgrounds_cache.find(filename);
        if (it!=m_backgrounds_cache.end()) {
            res = it->second.lock();
            if (res)
                return res;
            m_backgrounds_cache.erase(it);
        }
        
        bool variant = false;
        GHL::Image* img = LoadImage(filename,variant);
        if (!img) {
            SB_LOGE("failed LoadBackground " << filename);
            return BackgroundDataPtr();
        }
        
        res = BackgroundDataPtr(new BackgroundData());
        
        GHL::UInt32 w = img->GetWidth();
        GHL::UInt32 h = img->GetHeight();
        res->width = w;
        res->height = h;
        float scale = variant?1.0f/m_scale:1.0f;
        GHL::UInt32 y = 0;
        while ( y < h ) {
            GHL::UInt32 oh = h - y;
            GHL::UInt32 ph = get_subdiv_size(m_render,oh);
            GHL::UInt32 iph = ph > oh ? oh : ph;
            GHL::UInt32 x = 0;
            while ( x < w ) {
                GHL::UInt32 ow = w - x;
                GHL::UInt32 pw = get_subdiv_size(m_render, ow);
                GHL::UInt32 ipw = pw > ow ? ow : pw;
                GHL::Image* subimg = img;
                if (x!=0 || y!=0 || ipw != res->width || iph != res->height)
                    subimg = img->SubImage(x, y, ipw, iph);
                if (!subimg) {
                    SB_LOGE( "failed load subdiv image " << filename );
                    img->Release();
                    return BackgroundDataPtr();
                }
                GHL::Texture* texture = m_render->CreateTexture(pw, ph, GHL_ImageFormatToTextureFormat(subimg->GetFormat()),
                                                                subimg);
                if (!texture) {
                    SB_LOGE( "failed texture subdiv image " << filename);
                    img->Release();
                    return BackgroundDataPtr();
                }
                if (subimg != img)
                    subimg->Release();
                res->images.push_back(Image(TexturePtr(new Texture(texture,scale)),0,0,float(ipw)*scale,float(iph)*scale));
                res->images.back().SetHotspot(Vector2f(-float(x)*scale,-float(y)*scale));
                x+=ipw;
                texture->DiscardInternal();
            }
            y+=iph;
        }
        img->Release();
        if (variant) {
            res->width = float(res->width)/m_scale;
            res->height = float(res->height)/m_scale;
        }
        return res;
    }
    
    GHL::Image* Resources::ImageFromStream( GHL::DataStream* ds ) {
        if (!m_image) {
            LogError(MODULE) << "image decoder not initialized";
            return 0;
        }
        GHL::Image* img = m_image->Decode(ds);
        return img;
    }
    
	GHL::Image* Resources::LoadImage(const char* filename,bool &variant) {
        
        if (!m_vfs) {
            LogError(MODULE) << "VFS not initialized";
            return 0;
        }
		GHL::DataStream* ds = OpenFileVariant(filename,variant);
        if ( !ds ) {
            LogError(MODULE) <<"failed opening file " << filename;
            return 0;
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
    
    bool Resources::GetImageInfo(GHL::ImageInfo &info,GHL::DataStream* ds) {
        return m_image->GetFileInfo(ds, &info);
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
		GHL::DataStream* ds = OpenFileVariant(fn.c_str(),variant);
		if (!ds) {
			LogError(MODULE) <<"failed opening file " << fn;
            return 0;
		}
        GHL::ImageInfo info;
        if (!GetImageInfo(info,ds)) {
            ds->Release();
            LogError(MODULE) <<"error getting image info for file " << fn;
			return false;
        }
        ds->Release();
        w = info.width;
        h = info.height;
		return true;
    }
    
  
    TexturePtr Resources::CreateTexture( const GHL::Image* image,
                             float scale,
                             GHL::TextureFormat fmt) {
        if (!image)
            return TexturePtr();
        GHL::UInt32 tw = 0;
        GHL::UInt32 th = 0;
        GetTextureSize(image->GetWidth(), image->GetHeight(), tw, th, false);
        bool setData = ( tw == image->GetWidth() ) && ( th == image->GetHeight() );
        GHL::Texture* texture = m_render->CreateTexture(tw,
                                                        th,fmt,
                                                        setData ? image : 0);
        if (!texture)
            return TexturePtr();
        if (!setData)
            texture->SetData(0, 0, image);
        texture->DiscardInternal();
        return TexturePtr( new Texture(texture,scale,image->GetWidth(),image->GetHeight()));
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
    GHL::Texture* Resources::CreateTexture( GHL::Image* img , bool premultiply) {
        GHL::TextureFormat tfmt;
        int bpp = 4;
		if (img->GetFormat()==GHL::IMAGE_FORMAT_RGB) {
			tfmt = GHL::TEXTURE_FORMAT_RGB;
            bpp = 3;
        } else if (img->GetFormat()==GHL::IMAGE_FORMAT_RGBX) {
            tfmt = GHL::TEXTURE_FORMAT_RGBX;
            bpp = 4;
        } else if (img->GetFormat()==GHL::IMAGE_FORMAT_RGBA) {
		 	tfmt = GHL::TEXTURE_FORMAT_RGBA;
             bpp = 4;
		} else {
			LogError(MODULE) <<"unsupported format";
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
        if (!texture) {
            return 0;
        }
		tfmt = texture->GetFormat();
		GHL::ImageFormat ifmt;
		if (tfmt==GHL::TEXTURE_FORMAT_RGB) {
			ifmt = GHL::IMAGE_FORMAT_RGB;
			bpp = 3;
		} else if (tfmt==GHL::TEXTURE_FORMAT_RGBX) {
            ifmt = GHL::IMAGE_FORMAT_RGBX;
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
            //img->Convert(ifmt);
            texture->SetData(0,0,img);
        } else {
			//LogInfo(MODULE) << "Loaded image : " << filename << "." << ext << " " << img->GetWidth() << "x" << img->GetHeight() ;
        }
        texture->DiscardInternal();
        return texture;
    }
    GHL::Texture* Resources::ManagedLoadTexture( const sb::string& filename , bool& variant, bool premultiply ) {
        //LogDebug() << "load texture " << filename;
        GHL::Image* img = LoadImage(filename.c_str(),variant);
		if (!img) {
			return 0;
		}
        
        GHL::Texture* texture = CreateTexture(img, premultiply);
		
        img->Release();
        
        if (texture) {
            m_memory_used += texture->GetMemoryUsage();
        }
        
        return texture;
    }
        
    BitmaskPtr Resources::LoadBitmask( const sb::string& filename ) {
        bool variant = false;
        GHL::Image* img = LoadImage(filename.c_str(),variant);
        if (!img) {
            return BitmaskPtr();
        }
        if (img->GetFormat() != GHL::IMAGE_FORMAT_RGBA &&
            img->GetFormat() != GHL::IMAGE_FORMAT_GRAY) {
            img->Release();
            return BitmaskPtr();
        }
        BitmaskPtr res(new Bitmask(img));
        img->Release();
        return res;
    }
    
	bool Resources::ImageHaveAlpha(const GHL::Image* img) const {
		return img->GetFormat()==GHL::IMAGE_FORMAT_RGBA;
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
        GHL::Texture* texture = CreateTexture(img,true);
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
			m_fshaders[ffilename]=fs;
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
    
    ShaderPtr Resources::CreateShader(const char* vfn,
                           const char* vdata,
                           const char* ffn,
                           const char* fdata) {
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
            InlinedData dsd(vdata,strlen(vdata));
            vs = m_render->CreateVertexShader(&dsd);
            if (!vs) {
                LogError(MODULE) << "error create shader " << vfilename;
                //return ShaderPtr();
            }
            m_vshaders[vfilename]=vs;
        }
        if (!fs) {
            InlinedData dsd(fdata,strlen(fdata));
            fs = m_render->CreateFragmentShader(&dsd);
            if (!fs) {
                LogError(MODULE) << "error creating shader " << ffilename;
                //return ShaderPtr();
            }
            m_fshaders[ffilename]=fs;
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
        if (!rt) {
            LogError() << "failed create targer " << w << "x" << h << "->" << nw << "x" << nh;
            return RenderTargetPtr();
        }
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
                    if ( (lt && lt < m_live_ticks) || t->unique() ) {
                        
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
        m_default_pool.reset();
        for (TexturesCacheMap::iterator it = m_textures.begin();it!=m_textures.end();++it) {
            TexturePtr& t = it->second;
            if (t) {
                t->Release();
            }
        }
        m_textures.clear();
        m_shaders.clear();
        for (sb::map<sb::string,GHL::VertexShader*>::iterator it = m_vshaders.begin();it!=m_vshaders.end();++it) {
            if (it->second) it->second->Release();
        }
        m_vshaders.clear();
        for (sb::map<sb::string,GHL::FragmentShader*>::iterator it = m_fshaders.begin();it!=m_fshaders.end();++it) {
            if (it->second) it->second->Release();
        }
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
    
    TexturePoolPtr Resources::GetDefaultTexturePool() {
        if (!m_default_pool) {
            m_default_pool.reset( new TexturePool(this) );
        }
        return m_default_pool;
    }
}
