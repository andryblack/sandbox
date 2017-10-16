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
#include <ghl_image.h>
#include <ghl_render.h>

namespace GHL {
	struct VFS;
	struct Render;
	struct Image;
	struct ImageDecoder;
	struct VertexShader;
	struct FragmentShader;
	struct DataStream;
    struct WriteStream;
    struct Data;
    struct ImageInfo;
}
namespace Sandbox {
	
	
	class Atlaser;
    
    class TextureProvider {
    public:
        virtual ~TextureProvider() {}
        virtual TexturePtr GetTexture(const char* path) = 0;
    };
    
    struct BackgroundData {
        sb::vector<Image>  images;
        GHL::UInt32 width;
        GHL::UInt32 height;
    };
    typedef sb::shared_ptr<BackgroundData> BackgroundDataPtr;
    
    
	class Resources : public FileProvider , public TextureProvider {
	public:
		Resources(GHL::VFS* vfs);
		~Resources();
        
        virtual void Init(GHL::Render* render,GHL::ImageDecoder* image);
        void ReleaseAll();
		
		void SetBasePath(const char* path);
		GHL::DataStream* OpenFile(const char* fn);
        GHL::DataStream* OpenFileVariant(const char* fn,bool& variant);
        GHL::WriteStream* OpenWrite(const char* fn,bool remove);
        
        ImagePtr CreateImageFromData( const GHL::Data* data );
		
        
		TexturePtr GetTexture(const char* filename, bool need_premultiply);
        
        virtual TexturePtr GetTexture(const char* filename) SB_OVERRIDE {
            return GetTexture(filename, false);
        }
        
		ImagePtr GetImage(const char* filename, bool need_premultiply);
		BackgroundDataPtr LoadBackground(const char* filename);
		
		ShaderPtr GetShader(const char* vfn,const char* ffn);
        ShaderPtr CreateShader(const char* vfn,
                               const char* vdata,
                               const char* ffn,
                               const char* fdata);
		
		GHL::Render* GetRender() { return m_render;}
		GHL::ImageDecoder* GetImageDecoder() {return m_image;}
		GHL::VFS* GetVFS() { return m_vfs;}
		const std::string& GetBasePath() const { return m_base_path;}
		
        TexturePtr CreateTexture( const GHL::Image* image,
                                 float scale,
                                 GHL::TextureFormat fmt);
        
        BitmaskPtr LoadBitmask( const sb::string& filename );
        
        size_t  GetLiveTicks() const { return m_live_ticks; }
        
        size_t  GetMemoryLimit() const { return m_memory_limit; }
        void SetMemoryLimit(size_t limit) { m_memory_limit = limit; }
        size_t  GetMemoryUsed() const { return m_memory_used; }
        
        void SetResourcesVariant(float scale,const sb::string& postfix);
        const sb::string& GetResourcesPostfix() const { return m_res_variant; }
        
        RenderTargetPtr CreateRenderTarget(int w, int h, float scale,bool alpha, bool depth);
        float GetScale() const { return m_scale; }
        void    ProcessMemoryMgmt();
        
        const sb::string& GetCachePath() const { return m_cache_path; }

    protected:
        virtual GHL::Image* ImageFromData( const GHL::Data* data );
        virtual GHL::Image* ImageFromStream( GHL::DataStream* ds );
        virtual bool GetImageInfo(GHL::ImageInfo &info,GHL::DataStream* ds);
    
        friend class Texture;
        GHL::Texture* ManagedLoadTexture( const sb::string& filename , bool& variant, bool premultiply);
    private:
		GHL::VFS* m_vfs;
		GHL::Render* m_render;
		GHL::ImageDecoder* m_image;
		sb::string	m_base_path;
        sb::string  m_cache_path;
		
		friend class Atlaser;
		
        GHL::Image* LoadImage(const char* file,bool& variant);
		bool ImageHaveAlpha(const GHL::Image* img) const;
		bool GetImageInfo(sb::string& file,bool& variant,GHL::UInt32& w,GHL::UInt32& h);

        typedef sb::map<sb::string,TexturePtr > TexturesCacheMap;
        TexturesCacheMap m_textures;
		sb::map<sb::string,GHL::VertexShader*>      m_vshaders;
		sb::map<sb::string,GHL::FragmentShader*>    m_fshaders;
		sb::map<sb::string,ShaderPtr >   m_shaders;

        typedef sb::weak_ptr<BackgroundData> BackgroundDataCachePtr;
        typedef sb::map<sb::string, BackgroundDataCachePtr> BackgroundsDataCache;
        BackgroundsDataCache m_backgrounds_cache;
        
        size_t    m_live_ticks;
        size_t    m_memory_limit;
        size_t    m_memory_used;
        size_t    FreeMemory(size_t mem,bool full);
        
        void GetTextureSize( GHL::UInt32 w,GHL::UInt32 h, GHL::UInt32& tw, GHL::UInt32& th , bool target) const;
        
        
        GHL::Texture* CreateTexture( GHL::Image* img , bool premultiply);
        float       m_scale;
        sb::string  m_res_variant;
	};
}

#endif /*SB_RESOURCES_H*/
