#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include <sbstd/sb_string.h>
#include <sb_lua.h>
#include <sb_file_provider.h>
#include <ghl_vfs.h>
#include <meta/sb_meta.h>

namespace GHL {
    struct ImageDecoder;
    struct Image;
}

class Texture : public Sandbox::meta::object {
    SB_META_OBJECT
private:
    GHL::UInt32         m_width;
    GHL::UInt32         m_height;
public:
    explicit Texture( GHL::UInt32 w, GHL::UInt32 h) :
        m_width(w), m_height(h) {}
    GHL::UInt32 width() const { return m_width; }
    GHL::UInt32 height() const { return m_height; }
};
typedef sb::intrusive_ptr<Texture> TexturePtr;

class TextureData : public Texture {
    SB_META_OBJECT
private:
    GHL::Image* m_data;
public:
    explicit TextureData( GHL::UInt32 w, GHL::UInt32 h );
    explicit TextureData( GHL::Image* img );
    ~TextureData();
    void PremultiplyAlpha();
    const GHL::Image* GetImage() const { return m_data; }
    void Place( GHL::UInt32 x, GHL::UInt32 y, const sb::intrusive_ptr<TextureData>& img );
};
typedef sb::intrusive_ptr<TextureData> TextureDataPtr;

class Application : public Sandbox::FileProvider {
public:
	Application();
	~Application();

	void set_paths(const sb::string& scripts, const sb::string& src, const sb::string& dst);
	void set_platform(const sb::string& platform);

    TexturePtr check_texture( const sb::string& file );
    TextureDataPtr load_texture( const sb::string& file );
    bool store_texture( const sb::string& file , const TextureDataPtr& data );
    
	int run();
protected:
    /// FileProvider
    virtual GHL::DataStream* OpenFile(const char* fn);
private:
	Sandbox::LuaVM*	m_lua;
    GHL::VFS*   m_vfs;
    GHL::ImageDecoder* m_image_decoder;
    sb::string  m_scripts_dir;
    sb::string  m_dst_dir;
    sb::string  m_src_dir;
    sb::string  m_platform;
};

#endif /*APPLICATION_H_INCLUDED*/
