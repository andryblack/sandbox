#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include <sbstd/sb_string.h>
#include <sbstd/sb_vector.h>
#include <sb_lua.h>
#include <sb_file_provider.h>
#include <ghl_vfs.h>
#include <ghl_image_decoder.h>
#include <meta/sb_meta.h>
#include "tasks_pool.h"
#include "texture.h"

namespace GHL {
    struct ImageDecoder;
    struct Image;
}




class Application : public Sandbox::FileProvider {
    SB_META_OBJECT_BASE
public:
	Application();
	virtual ~Application();

    void set_update_only(bool u);
    void set_dst_path(const sb::string& dst);
    void set_threads(int threads);
    void set_quality(int q);
    const sb::string& get_dst_path() const { return m_dst_dir; }
    void set_options(const sb::vector<sb::string>& o) { m_options = o;}
    const sb::vector<sb::string>& get_options() const { return m_options; }

    void set_paths(const sb::vector<sb::string>& scripts, const sb::string& src, const sb::string& dst);
	void set_platform(const sb::string& platform);
    void set_arguments(const sb::vector<sb::string>& arguments);

    TexturePtr check_texture( const sb::string& file );
    TextureDataPtr load_texture( const sb::string& file );
    bool store_texture( const sb::string& file , const TextureDataPtr& data );
    virtual bool store_file(  const sb::string& file , const GHL::Data* data );
    bool write_text_file( const sb::string& file , const char* data  );
    bool premultiply_image( const sb::string& src, const sb::string& dst );
    bool rebuild_image( const sb::string& src, const sb::string& dst );
    bool encode_sound( const sb::string& src, const sb::string& dst );
    
	int run();
    
    bool strip_jpeg(const sb::string& src, const sb::string& dst);
    
    /// FileProvider
    virtual GHL::DataStream* OpenFile(const char* fn);
    virtual GHL::WriteStream* OpenDestFile(const char* fn);
    GHL::Data* LoadData(const char* fn);
    
    sb::string get_src_path(const char* fn) const;
    bool wait_tasks();
    virtual const GHL::Data* encode_texture(const TextureDataPtr& texture);
    
    void set_png_encode_settings(GHL::Int32 settings);
    GHL::Int32 get_png_encode_settings() const { return m_png_encode_settings; }
    void set_jpeg_encode_settings(GHL::Int32 settings);
    GHL::Int32 get_jpeg_encode_settings() const { return m_jpeg_encode_settings; }
protected:
    virtual double GetVersion() const;
    Sandbox::LuaVM* GetLua() { return m_lua; }
    virtual void Bind(lua_State* L);
    GHL::ImageDecoder* get_image_decoder() { return m_image_decoder; }

    virtual sb::string get_output_filename( const char* name );
    
    TasksPool* m_tasks;
private:
	Sandbox::LuaVM*	m_lua;
    GHL::VFS*   m_vfs;
    GHL::ImageDecoder* m_image_decoder;
    sb::vector<sb::string>  m_scripts_dir;
    sb::vector<sb::string>  m_options;
    sb::string  m_dst_dir;
    sb::string  m_src_dir;
    sb::string  m_platform;
    bool        m_update_only;
    sb::vector<sb::string> m_arguments;
    GHL::Int32 m_jpeg_encode_settings;
    GHL::Int32 m_png_encode_settings;
};

#endif /*APPLICATION_H_INCLUDED*/
