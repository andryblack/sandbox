#include "sb_freetype_bind.h"
#include "sb_freetype_font.h"
#include "meta/sb_meta.h"
#include "luabind/sb_luabind.h"

static int Sandbox_FreeTypeFont_Load(lua_State* L) {
    Sandbox::Resources* res = Sandbox::luabind::stack<Sandbox::Resources*>::get(L, 1);
    if (!res) {
        luaL_argerror(L, 1, "need Resources");
    }
    if (!lua_isstring(L, 2)) {
        luaL_argerror(L, 2, "need string");
    }
    if (!lua_istable(L, 3)) {
        luaL_argerror(L, 3, "need table");
    }
    const char* filename = lua_tostring(L, 2);
    Sandbox::FreeTypeFontConfig config;
    lua_getfield(L, 3, "size");
    config.size = lua_tonumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 3, "outline");
    config.outline = lua_toboolean(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 3, "dpi");
    config.dpi = float(lua_tonumber(L, -1));
    lua_pop(L, 1);
    lua_getfield(L, 3, "scale");
    if (lua_isnumber(L, -1))
        config.scale = float(lua_tonumber(L, -1));
    else
        config.scale = 1.0f;
    lua_pop(L, 1);
    lua_getfield(L, 3, "x_scale");
    if (lua_isnumber(L, -1))
        config.x_scale = float(lua_tonumber(L, -1));
    else
        config.x_scale = 1.0f;
    lua_pop(L, 1);
    lua_getfield(L, 3, "substitute_code");
    config.substitute_code = lua_tounsigned(L, -1);
    lua_pop(L, 1);
    Sandbox::luabind::stack<sb::intrusive_ptr<Sandbox::FreeTypeFont> >::push(L, Sandbox::FreeTypeFont::Load(res, filename, config));
    return 1;
}

SB_META_BEGIN_KLASS_BIND(Sandbox::FreeTypeFontChild)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::FreeTypeFont)
bind( static_method( "Load" , &Sandbox_FreeTypeFont_Load ) );
SB_META_PROPERTY_RO(MainData, GetMainData)
SB_META_PROPERTY_RO(OutlineData,GetOutlineData)
SB_META_METHOD(CreateMainChild)
SB_META_METHOD(CreateOutlineChild)
SB_META_METHOD(SetCharImage)
SB_META_METHOD(AddCharImage)
SB_META_END_KLASS_BIND()


namespace Sandbox {
    
    void register_freetype( lua_State* lua ) {
        luabind::ExternClass<FreeTypeFontChild>(lua);
        luabind::ExternClass<FreeTypeFont>(lua);
    }
    
}
