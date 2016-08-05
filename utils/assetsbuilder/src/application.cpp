#include "application.h"

#include <sb_lua_context.h>
#include <sb_log.h>
#include <ghl_image.h>
#include <ghl_image_decoder.h>
#include <json/sb_json.h>
#include <sb_data.h>

#include "spine_convert.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

static const double VERSION = 1.5;

#include <luabind/sb_luabind.h>

#if LUA_VERSION_NUM >= 502
static void luaL_register(lua_State* L,const char* name,const luaL_Reg *l) {
    if (name) {
        lua_getglobal(L, name);
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            lua_newtable(L);
        }
        luaL_setfuncs (L, l, 0);lua_pushvalue(L,-1);lua_setglobal(L,name);
    } else {
        luaL_setfuncs(L,l,0);
    }
}
#endif

GHL_API GHL::VFS* GHL_CALL GHL_CreateVFS();
GHL_API void GHL_CALL GHL_DestroyVFS(GHL::VFS* vfs);

extern "C" {
/* Built-in premake functions */
int path_getabsolute(lua_State* L);
int path_getrelative(lua_State* L);
int path_isabsolute(lua_State* L);
int path_join(lua_State* L);
int path_normalize(lua_State* L);
int path_translate(lua_State* L);
int os_chdir(lua_State* L);
int os_chmod(lua_State* L);
int os_copyfile(lua_State* L);
int os_getcwd(lua_State* L);
int os_getversion(lua_State* L);
int os_is64bit(lua_State* L);
int os_isdir(lua_State* L);
int os_isfile(lua_State* L);
int os_islink(lua_State* L);
//int os_locate(lua_State* L);
int os_matchdone(lua_State* L);
int os_matchisfile(lua_State* L);
int os_matchname(lua_State* L);
int os_matchnext(lua_State* L);
int os_matchstart(lua_State* L);
int os_mkdir(lua_State* L);
int os_pathsearch(lua_State* L);
int os_realpath(lua_State* L);
int os_rmdir(lua_State* L);
int os_stat(lua_State* L);
int os_uuid(lua_State* L);
int string_endswith(lua_State* L);
int string_hash(lua_State* L);
int string_sha1(lua_State* L);
int string_startswith(lua_State* L);
}

static const luaL_Reg path_functions[] = {
    { "getabsolute", path_getabsolute },
    { "getrelative", path_getrelative },
    { "isabsolute",  path_isabsolute },
    { "join", path_join },
    { "normalize", path_normalize },
    { "translate", path_translate },
    { NULL, NULL }
};

static const luaL_Reg os_functions[] = {
    { "chdir",       os_chdir       },
    { "chmod",       os_chmod       },
    { "copyfile",    os_copyfile    },
    { "_is64bit",    os_is64bit     },
    { "isdir",       os_isdir       },
    { "getcwd",      os_getcwd      },
    { "getversion",  os_getversion  },
    { "isfile",      os_isfile      },
    { "islink",      os_islink      },
    //{ "locate",      os_locate      },
    { "matchdone",   os_matchdone   },
    { "matchisfile", os_matchisfile },
    { "matchname",   os_matchname   },
    { "matchnext",   os_matchnext   },
    { "matchstart",  os_matchstart  },
    { "mkdir",       os_mkdir       },
    { "pathsearch",  os_pathsearch  },
    { "realpath",    os_realpath    },
    { "rmdir",       os_rmdir       },
    { "stat",        os_stat        },
    { "uuid",        os_uuid        },
    { NULL, NULL }
};

static const luaL_Reg string_functions[] = {
    { "endswith",  string_endswith },
    { "hash", string_hash },
    { "sha1", string_sha1 },
    { "startswith", string_startswith },
    { NULL, NULL }
};

int luaopen_xml(lua_State* L);


bool (*sb_terminate_handler)() = 0;

static sb::string append_path(const sb::string& dir, const sb::string& name) {
    if (!name.empty() && name[0]=='/')
        return name;
    sb::string res = dir;
    if (!res.empty() && res[res.length()-1]!='/')
        res.append("/");
    res.append(name);
    return res;
}

SB_META_DECLARE_OBJECT(Application, Sandbox::FileProvider);
SB_META_BEGIN_KLASS_BIND(Application)
SB_META_METHOD(check_texture)
SB_META_METHOD(load_texture)
SB_META_METHOD(store_texture)
SB_META_METHOD(convert_spine)
SB_META_METHOD(open_spine)
SB_META_METHOD(write_text_file)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(SkeletonConvert)
SB_META_METHOD(RenameAnimation)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(SpineConvert)
SB_META_METHOD(Export)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_OBJECT(Texture, Sandbox::meta::object)
SB_META_BEGIN_KLASS_BIND(Texture)
SB_META_PROPERTY_RO(width, width)
SB_META_PROPERTY_RO(height, height)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_OBJECT(TextureData, Texture)
SB_META_BEGIN_KLASS_BIND(TextureData)
SB_META_CONSTRUCTOR((int,int))
SB_META_METHOD(PremultiplyAlpha)
SB_META_METHOD(Place)
SB_META_METHOD(SetAlpha)
SB_META_END_KLASS_BIND()

TextureData::TextureData( GHL::UInt32 w, GHL::UInt32 h) : Texture(w,h), m_data(GHL_CreateImage(w, h, GHL::IMAGE_FORMAT_RGBA)) {
    m_data->Fill(0x00000000);
}

TextureData::TextureData( GHL::Image* img ) : Texture(img->GetWidth(),img->GetHeight()) , m_data(img) {
    
}

TextureData::~TextureData() {
    m_data->Release();
}

void TextureData::PremultiplyAlpha() {
    if (m_data->GetFormat()==GHL::IMAGE_FORMAT_4444) {
        m_data->Convert(GHL::IMAGE_FORMAT_RGBA);
    }
    m_data->PremultiplyAlpha();
}

void TextureData::Place( GHL::UInt32 x, GHL::UInt32 y, const sb::intrusive_ptr<TextureData>& img ) {
    m_data->Draw(x, y, img->GetImage());
}

bool TextureData::SetAlpha( const sb::intrusive_ptr<TextureData>& alpha_tex ) {
    if (!m_data) return false;
    return m_data->SetAlpha(alpha_tex->GetImage());
}

Application::Application() : m_lua(0),m_vfs(0),m_update_only(false) {
	m_lua = new Sandbox::LuaVM(this);
    m_vfs = GHL_CreateVFS();
    m_image_decoder = GHL_CreateImageDecoder();
    
    

#if defined( GHL_PLATFORM_MAC )
    m_platform = "osx";
#elif defined( GHL_PLATFORM_WIN )
    m_platform = "windows";
#else
    m_platform = "unknown";
#endif
}

double Application::GetVersion() const {
    return VERSION;
}

void Application::Bind(lua_State* L) {
    
    static const luaL_Reg loadedlibs[] = {
        {LUA_OSLIBNAME, luaopen_os},
        {LUA_IOLIBNAME, luaopen_io},
        {"XML",luaopen_xml},
        {NULL, NULL}
    };
    
    const luaL_Reg *lib;
    /* call open functions from 'loadedlibs' and set results to global table */
    for (lib = loadedlibs; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);  /* remove lib */
    }

    luaL_register(L, "path",     path_functions);
    luaL_register(L, "os",       os_functions);
    luaL_register(L, "string",   string_functions);
    
    Sandbox::luabind::Class<Texture>(L);
    Sandbox::luabind::Class<TextureData>(L);
    Sandbox::luabind::ExternClass<Application>(L);
    Sandbox::luabind::ExternClass<SkeletonConvert>(L);
    Sandbox::luabind::ExternClass<SpineConvert>(L);
    Sandbox::register_json(L);
}

Application::~Application() {
	delete m_lua;
    if (m_vfs) {
        GHL_DestroyVFS(m_vfs);
    }
    if (m_image_decoder) {
        GHL_DestroyImageDecoder(m_image_decoder);
    }
}

void Application::set_update_only(bool u){
    m_update_only = u;
}

void Application::set_paths(const sb::vector<sb::string>& scripts, const sb::string& src, const sb::string& dst) {
	m_dst_dir = dst;
    m_scripts_dir = scripts;
    m_src_dir = src;
}

void Application::set_arguments(const sb::vector<sb::string>& arguments) {
    m_arguments = arguments;
}
void Application::set_platform(const sb::string& platform) {
    if (!platform.empty()) {
        m_platform = platform;
    }
}

GHL::DataStream* Application::OpenFile(const char* fn) {
    if (!fn) {
        return 0;
    }
    sb_assert(m_vfs);
    if (fn[0]=='_') {
        for (sb::vector<sb::string>::const_iterator it = m_scripts_dir.begin();it!=m_scripts_dir.end();++it) {
            GHL::DataStream* r = m_vfs->OpenFile(append_path(*it, fn).c_str());
            if (r) {
                return r;
            }
        }
        return 0;
    }
    if (fn[0]=='/') {
        return m_vfs->OpenFile(fn);
    }
    return m_vfs->OpenFile(append_path(m_src_dir, fn).c_str());
}

sb::string Application::get_output_filename( const char* name ) {
    return append_path(m_dst_dir, name);
}

TexturePtr Application::check_texture( const sb::string& file ) {
    GHL::DataStream* ds = m_vfs->OpenFile(append_path(m_src_dir, file).c_str());
    if (!ds) return TexturePtr();
    GHL::ImageInfo info;
    if (!m_image_decoder->GetFileInfo(ds, &info)) {
        ds->Release();
        return TexturePtr();
    }
    return TexturePtr(new Texture(info.width,info.height));
}

TextureDataPtr Application::load_texture( const sb::string& file ) {
    GHL::DataStream* ds = m_vfs->OpenFile(append_path(m_src_dir, file).c_str());
    if (!ds) return TextureDataPtr();
    GHL::Image* img = m_image_decoder->Decode(ds);
    if (!img) {
        ds->Release();
        return TextureDataPtr();
    }
    return TextureDataPtr(new TextureData(img));
}

bool Application::store_texture( const sb::string& file , const TextureDataPtr& data ) {
    if (!data) return false;
    const GHL::Data* d = m_image_decoder->Encode(data->GetImage(), GHL::IMAGE_FILE_FORMAT_PNG);
    if (!d) {
        Sandbox::LogError() << "failed encode texture to " << file;
        return false;
    }
    bool res = store_file(file, d);
    d->Release();
    return res;
}

bool Application::write_text_file( const sb::string& file , const char* data  ) {
    Sandbox::InlinedData v(data,::strlen(data));
    return store_file(file, &v);
}
bool Application::store_file(  const sb::string& file , const GHL::Data* data ) {
    if (!data) return false;
    if (!m_vfs->WriteFile(append_path(m_dst_dir, file).c_str(), data)) {
        Sandbox::LogError() << "failed store texture to " << file;
        return false;
    }
    return true;
}

bool Application::premultiply_image( const sb::string& src, const sb::string& dst ) {
    TextureDataPtr data = load_texture(src);
    if (data) {
        data->PremultiplyAlpha();
        return store_texture(dst, data);
    }
    return false;
}

bool Application::rebuild_image( const sb::string& src, const sb::string& dst ) {
    TextureDataPtr data = load_texture(src);
    if (data) {
        return store_texture(dst, data);
    }
    return false;
}

bool Application::convert_spine(const sb::string& atlas, const sb::string& skelet, const sb::string& outfile) {
    SpineConvert convert;
    if (!convert.Load(atlas.c_str(), skelet.c_str(), this))
        return false;
    convert.Export(outfile.c_str(),this);

    return true;
}

sb::intrusive_ptr<SpineConvert> Application::open_spine(const sb::string& atlas,
                                           const sb::string& skelet ) {
    sb::intrusive_ptr<SpineConvert> res(new SpineConvert);
    if (!res->Load(atlas.c_str(), skelet.c_str(), this))
        return sb::intrusive_ptr<SpineConvert>();
    return res;
}

int Application::run() {
    Bind(m_lua->GetVM());

    m_lua->GetGlobalContext()->SetValue("application", this);
    m_lua->GetGlobalContext()->SetValue("platform",m_platform);
    m_lua->GetGlobalContext()->SetValue("host_version", GetVersion());
    m_lua->GetGlobalContext()->SetValue("update_only", m_update_only);
    m_lua->GetGlobalContext()->SetValue("app_arguments", m_arguments);
    
    if (!m_lua->DoFile("_init.lua")) {
        Sandbox::LogError() << "failed exec init script, check script paths";
        return 1;
    }
    
    
    m_lua->GetGlobalContext()->SetValue("src_path",m_src_dir);
    m_lua->GetGlobalContext()->SetValue("dst_path",m_dst_dir);
    
    if (!m_lua->DoFile("_run.lua")) {
        Sandbox::LogError() << "failed exec run script";
        return 1;
    }
	return 0;
}