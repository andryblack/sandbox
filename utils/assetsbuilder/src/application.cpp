#include "application.h"
#include <sb_lua_context.h>
#include <sb_log.h>
#include <ghl_image.h>
#include <ghl_image_decoder.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

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

GHL_API GHL::VFS* GHL_CALL GHL_CreateVFSPosix(const char* data,const char* docs);
GHL_API void GHL_CALL GHL_DestroyVFSPosix(GHL::VFS* vfs);

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
int os_locate(lua_State* L);
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
    { "locate",      os_locate      },
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


bool (*sb_terminate_handler)() = 0;

static sb::string append_path(const sb::string& dir, const sb::string& name) {
    sb::string res = dir;
    if (!res.empty() && res[res.length()-1]!='/')
        res.append("/");
    res.append(name);
    return res;
}

SB_META_DECLARE_KLASS(Application, void);
SB_META_BEGIN_KLASS_BIND(Application)
SB_META_METHOD(check_texture)
SB_META_METHOD(load_texture)
SB_META_METHOD(store_texture)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_OBJECT(Texture, Sandbox::meta::object)
SB_META_BEGIN_KLASS_BIND(Texture)
SB_META_PROPERTY_RO(width, width)
SB_META_PROPERTY_RO(height, height)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_OBJECT(TextureData, Texture)
SB_META_BEGIN_KLASS_BIND(TextureData)
SB_META_METHOD(PremultiplyAlpha)
SB_META_END_KLASS_BIND()



TextureData::TextureData( const sb::string& file, GHL::Image* img ) : Texture(file,img->GetWidth(),img->GetHeight()) , m_data(img) {
    
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

Application::Application() : m_lua(0),m_vfs(0) {
	m_lua = new Sandbox::LuaVM(this);
    m_vfs = GHL_CreateVFSPosix("/","/");
    m_image_decoder = GHL_CreateImageDecoder();
    
    lua_State* L = m_lua->GetVM();
    
    static const luaL_Reg loadedlibs[] = {
        {LUA_OSLIBNAME, luaopen_os},
        {LUA_IOLIBNAME, luaopen_io},
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

#ifdef GHL_PLATFORM_MAC
    m_platform = "osx";
#elif GHL_PLATFORM_WIN
    m_platform = "windows";
#else
    m_platform = "unknown";
#endif
}

Application::~Application() {
	delete m_lua;
    if (m_vfs) {
        GHL_DestroyVFSPosix(m_vfs);
    }
    if (m_image_decoder) {
        GHL_DestroyImageDecoder(m_image_decoder);
    }
}

void Application::set_paths(const sb::string& scripts, const sb::string& src, const sb::string& dst) {
	m_dst_dir = dst;
    m_scripts_dir = scripts;
    m_src_dir = src;
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
        return m_vfs->OpenFile(append_path(m_scripts_dir, fn).c_str());
    }
    return m_vfs->OpenFile(append_path(m_src_dir, fn).c_str());
}


TexturePtr Application::check_texture( const sb::string& file ) {
    GHL::DataStream* ds = m_vfs->OpenFile(append_path(m_src_dir, file).c_str());
    if (!ds) return TexturePtr();
    GHL::ImageInfo info;
    if (!m_image_decoder->GetFileInfo(ds, &info)) {
        ds->Release();
        return TexturePtr();
    }
    return TexturePtr(new Texture(file,info.width,info.height));
}

TextureDataPtr Application::load_texture( const sb::string& file ) {
    GHL::DataStream* ds = m_vfs->OpenFile(append_path(m_src_dir, file).c_str());
    if (!ds) return TextureDataPtr();
    GHL::Image* img = m_image_decoder->Decode(ds);
    if (!img) {
        ds->Release();
        return TextureDataPtr();
    }
    return TextureDataPtr(new TextureData(file,img));
}

bool Application::store_texture( const sb::string& file , const TextureDataPtr& data ) {
    if (!data) return false;
    const GHL::Data* d = m_image_decoder->Encode(data->GetImage(), GHL::IMAGE_FILE_FORMAT_PNG);
    if (!d)
        return false;
    if (!m_vfs->WriteFile(append_path(m_dst_dir, file).c_str(), d)) {
        d->Release();
        return false;
    }
    d->Release();
    return true;
}

int Application::run() {
    m_lua->GetGlobalContext()->SetValue("application", this);
    m_lua->GetGlobalContext()->SetValue("platform",m_platform);
    
    if (!m_lua->DoFile("_init.lua")) {
        Sandbox::LogError() << "failed exec init script, check path " << m_scripts_dir;
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