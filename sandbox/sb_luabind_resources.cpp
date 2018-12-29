//
//  sb_luabind_math.cpp
//  YinYang
//
//  Created by Андрей Куницын on 5/13/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "luabind/sb_luabind.h"
#include "meta/sb_meta.h"
#include "sb_resources.h"
#include "sb_texture.h"
#include "sb_image.h"
#include "sb_bitmask.h"
#include "sb_font.h"
#include "sb_bitmap_font.h"
#include "sb_shader.h"
#include "sb_color.h"
#include "sb_rect.h"
#include "sb_point.h"
#include "sb_size.h"
#include "sb_sound.h"
#include "sb_rendertarget.h"
#include "sb_utf.h"
#include "sb_tilemap.h"
#include "sb_system_font.h"
#include "sb_font_chain.h"

namespace Sandbox {
    namespace luabind {
        
        void stack<Color>::push( lua_State* L, const Sandbox::Color& val ) {
            stack_help<Color, false>::push(L, val);
        }
        
        
        Color stack<Color>::get( lua_State* L, int idx ) {
            int t = lua_type(L, idx);
            Sandbox::Color c;
            if (t == LUA_TTABLE) {
                LUA_CHECK_STACK(0)
                size_t indx = 0;
                lua_pushnil(L);
                while (lua_next(L, idx) != 0) {
                    if (indx==0) c.r = float(lua_tonumber(L, -1));
                    else if (indx==1) c.g = float(lua_tonumber(L, -1));
                    else if (indx==2) c.b = float(lua_tonumber(L, -1));
                    else if (indx==3) c.a = float(lua_tonumber(L, -1));
                    ++indx;
                    /* removes 'value'; keeps 'key' for next iteration */
                    lua_pop(L, 1);
                }
            } else if (t == LUA_TSTRING) {
                c = Sandbox::Color::FromString(lua_tostring(L, idx));
            } else {
                c = stack_help<Color, false>::get(L, idx);
            }
            return c;
        }
    }
}

static int sb_color_constructor(lua_State* L) {
    Sandbox::Color c;
    int t = lua_type(L, 2);
    int nargs = lua_gettop(L);
    if (t == LUA_TTABLE) {
        LUA_CHECK_STACK(0)
        size_t indx = 0;
        lua_pushnil(L);
        while (lua_next(L, 2) != 0) {
            if (indx==0) c.r = float(lua_tonumber(L, -1));
            else if (indx==1) c.g = float(lua_tonumber(L, -1));
            else if (indx==2) c.b = float(lua_tonumber(L, -1));
            else if (indx==3) c.a = float(lua_tonumber(L, -1));
            ++indx;
            /* removes 'value'; keeps 'key' for next iteration */
            lua_pop(L, 1);
        }
    } else if (t == LUA_TSTRING) {
        c = Sandbox::Color::FromString(lua_tostring(L, 2));
        if (nargs>2) {
            c.a = float(lua_tonumber(L, 3));
        }
    } else if (t == LUA_TNUMBER) {
        c.r = float(lua_tonumber(L, 2));
        c.g = float(lua_tonumber(L, 3));
        c.b = float(lua_tonumber(L, 4));
        c.a = float(lua_tonumber(L, 5));
    } else {
        Sandbox::luabind::lua_argerror(L, 2, "color", 0);
        return 0;
    }
    
    
    Sandbox::luabind::stack<Sandbox::Color>::push(L, c);
    return 1;
}


SB_META_DECLARE_KLASS(Sandbox::Color, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Color)
bind( constructor(&sb_color_constructor) );
SB_META_PROPERTY(r)
SB_META_PROPERTY(g)
SB_META_PROPERTY(b)
SB_META_PROPERTY(a)
SB_META_OPERATOR_MUL_(Sandbox::Color(Sandbox::Color::*)(float)const)
bind(method("mul", static_cast<Sandbox::Color(Sandbox::Color::*)(const Sandbox::Color&)const>(&Sandbox::Color::operator*)));
SB_META_METHOD(ToStringRGB)
SB_META_STATIC_METHOD(FromString)
SB_META_METHOD(Premultiply)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Rectf, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Rectf)
SB_META_CONSTRUCTOR((float,float,float,float))
SB_META_PROPERTY(x)
SB_META_PROPERTY(y)
SB_META_PROPERTY(w)
SB_META_PROPERTY(h)
SB_META_METHOD(PointInside)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Recti, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Recti)
SB_META_CONSTRUCTOR((int,int,int,int))
SB_META_PROPERTY(x)
SB_META_PROPERTY(y)
SB_META_PROPERTY(w)
SB_META_PROPERTY(h)
SB_META_METHOD(PointInside)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Pointf, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Pointf)
SB_META_CONSTRUCTOR((float,float))
SB_META_PROPERTY(x)
SB_META_PROPERTY(y)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Pointi, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Pointi)
SB_META_CONSTRUCTOR((int,int))
SB_META_PROPERTY(x)
SB_META_PROPERTY(y)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Sizef, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Sizef)
SB_META_CONSTRUCTOR((float,float))
SB_META_PROPERTY(w)
SB_META_PROPERTY(h)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Sizei, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Sizei)
SB_META_CONSTRUCTOR((int,int))
SB_META_PROPERTY(w)
SB_META_PROPERTY(h)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Texture, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Texture)
SB_META_METHOD(SetFiltered)
SB_META_PROPERTY_RO(Width, GetOriginalWidth)
SB_META_PROPERTY_RO(Height, GetOriginalHeight)
SB_META_PROPERTY_RO(OriginalWidth, GetOriginalWidth)
SB_META_PROPERTY_RO(OriginalHeight, GetOriginalHeight)
SB_META_PROPERTY_RO(ScaledWidth, GetWidth)
SB_META_PROPERTY_RO(ScaledHeight, GetHeight)
SB_META_PROPERTY_RW(Filtered, GetFiltered, SetFiltered)
SB_META_PROPERTY_RW(Tiled,GetTiled, SetTiled)
SB_META_PROPERTY_RO(RealWidth, GetRealWidth)
SB_META_PROPERTY_RO(RealHeight, GetRealHeight)
SB_META_METHOD(GetBitmask)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::RenderTarget, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::RenderTarget)
SB_META_PROPERTY_RO(Texture, GetTexture)
SB_META_METHOD(Discard)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Image, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Image)
SB_META_CONSTRUCTOR((Sandbox::TexturePtr,float,float,float,float))
SB_META_METHOD(SetSize)
SB_META_METHOD(SetTextureRect)
SB_META_METHOD(Clone)
SB_META_METHOD(CheckBit)
SB_META_PROPERTY_RO(TextureX,GetTextureX)
SB_META_PROPERTY_RO(TextureY,GetTextureY)
SB_META_PROPERTY_RO(TextureW,GetTextureW)
SB_META_PROPERTY_RO(TextureH,GetTextureH)
SB_META_PROPERTY_RO(Width,GetWidth)
SB_META_PROPERTY_RO(Height,GetHeight)
SB_META_PROPERTY_RW(Texture,GetTexture,SetTexture)
SB_META_PROPERTY_RW(Hotspot,GetHotspot,SetHotspot)
SB_META_PROPERTY_RO(Bounds, GetBounds)
SB_META_END_KLASS_BIND()


SB_META_DECLARE_KLASS(Sandbox::Bitmask, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Bitmask)
SB_META_STATIC_METHOD(GetThreshold)
SB_META_STATIC_METHOD(SetThreshold)
SB_META_METHOD(Get)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ShaderUniform)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ShaderFloatUniform)
SB_META_PROPERTY_RW_DEF(Value)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ShaderVec2Uniform)
SB_META_PROPERTY_RW_DEF(Value)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ShaderMat4Uniform)
SB_META_PROPERTY_RW_DEF(Value)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ShaderColorUniform)
SB_META_PROPERTY_RW_DEF(Value)
SB_META_PROPERTY_RW_DEF(Alpha)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::Shader)
SB_META_METHOD(Clone)
SB_META_METHOD(GetFloatUniform)
SB_META_METHOD(GetVec2Uniform)
SB_META_METHOD(GetColorUniform)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ImageBox, Sandbox::Image)
SB_META_BEGIN_KLASS_BIND(Sandbox::ImageBox)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RO(OffsetL, GetOffsetL)
SB_META_PROPERTY_RO(OffsetR, GetOffsetR)
SB_META_PROPERTY_RO(OffsetT, GetOffsetT)
SB_META_PROPERTY_RO(OffsetB, GetOffsetB)
SB_META_PROPERTY_RO(TileV, GetTileV)
SB_META_PROPERTY_RO(TileH, GetTileH)
SB_META_METHOD(SetOffsets)
SB_META_METHOD(SetTile)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::FontAlign,void);
SB_META_ENUM_BIND(Sandbox::FontAlign,namespace Sandbox,
                  SB_META_ENUM_ITEM(ALIGN_LEFT)
                  SB_META_ENUM_ITEM(ALIGN_RIGHT)
                  SB_META_ENUM_ITEM(ALIGN_CENTER))

SB_META_BEGIN_KLASS_BIND(Sandbox::FontData)
SB_META_METHOD(FixupChars)
SB_META_METHOD(SetSubsituteCode)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::FontDataProvider)
SB_META_PROPERTY_RO(MainData, GetMainData)
SB_META_METHOD(AddCharImage)
SB_META_METHOD(SetCharImage)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::OutlineFontDataProvider)
SB_META_PROPERTY_RO(OutlineData, GetOutlineData)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::FontChain)
SB_META_CONSTRUCTOR((const Sandbox::FontDataProviderPtr&))
SB_META_METHOD(Add)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::Font)
SB_META_CONSTRUCTOR((const FontDataProviderPtr&))
SB_META_PROPERTY_RO(Height, GetHeight)
SB_META_PROPERTY_RO(Size, GetSize)
SB_META_PROPERTY_RO(Baseline, GetBaseline)
SB_META_PROPERTY_RO(XHeight, GetXHeight)
SB_META_METHOD(GetTextWidth)
SB_META_METHOD(ClearPasses)
SB_META_METHOD(AddPass)
SB_META_END_KLASS_BIND()


SB_META_BEGIN_KLASS_BIND(Sandbox::FontPass)
SB_META_CONSTRUCTOR((const Sandbox::FontDataPtr&,const sb::string&))
SB_META_PROPERTY_RW(Offset, GetOffset, SetOffset)
SB_META_PROPERTY_RW(UseColor, GetUseColor, SetUseColor)
SB_META_PROPERTY_RW(Color, GetColor, SetColor)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::BitmapFont)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(AddGlypth)
SB_META_METHOD(AddKerningPair)
SB_META_METHOD(SetHeight)
SB_META_METHOD(SetSize)
SB_META_METHOD(SetBaseline)
SB_META_METHOD(SetXHeight)
SB_META_END_KLASS_BIND()


static int Sandbox_SystemFont_Load(lua_State* L) {
    Sandbox::Application* app = Sandbox::luabind::stack<Sandbox::Application*>::get(L, 1);
    if (!app) {
        luaL_argerror(L, 1, "need Application");
    }
    if (!lua_isstring(L, 2)) {
        luaL_argerror(L, 2, "need string");
    }
    if (!lua_istable(L, 3)) {
        luaL_argerror(L, 3, "need table");
    }
    const char* filename = lua_tostring(L, 2);
    Sandbox::SystemFontConfig config;
    lua_getfield(L, 3, "size");
    config.size = lua_tonumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 3, "outline");
    config.outline = lua_toboolean(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 3, "outline_width");
    if (lua_isnumber(L, -1))
        config.outline_width = float(lua_tonumber(L, -1));
    else
        config.outline_width = 1.0f;
    lua_pop(L, 1);
//    lua_getfield(L, 3, "dpi");
//    config.dpi = float(lua_tonumber(L, -1));
//    lua_pop(L, 1);
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
//    lua_getfield(L, 3, "substitute_code");
//    config.substitute_code = lua_tounsigned(L, -1);
//    lua_pop(L, 1);
    Sandbox::luabind::stack<sb::intrusive_ptr<Sandbox::SystemFont> >::push(L, Sandbox::SystemFont::Load(app, filename, config));
    return 1;
}

SB_META_BEGIN_KLASS_BIND(Sandbox::SystemFont)
bind( static_method( "Load" , &Sandbox_SystemFont_Load ) );
SB_META_END_KLASS_BIND()


SB_META_BEGIN_KLASS_BIND(Sandbox::FileProvider)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Resources, Sandbox::FileProvider)
SB_META_BEGIN_KLASS_BIND(Sandbox::Resources)
SB_META_METHOD(GetImage)
bind( method( "GetTexture" ,
             static_cast<Sandbox::TexturePtr(Sandbox::Resources::*)(const char*,bool)>(&Sandbox::Resources::GetTexture) ) );
SB_META_METHOD(GetShader)
SB_META_METHOD(CreateShader)
SB_META_METHOD(CreateRenderTarget)
SB_META_METHOD(RemoveFile)
SB_META_PROPERTY_RO(Scale, GetScale)
SB_META_PROPERTY_WO(BasePath, SetBasePath)
SB_META_PROPERTY_RO(CachePath, GetCachePath)
SB_META_PROPERTY_RO(ResourcesPostfix, GetResourcesPostfix)
SB_META_PROPERTY_RO(MemoryUsed, GetMemoryUsed)
SB_META_PROPERTY_RW(MemoryLimit, GetMemoryLimit, SetMemoryLimit)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::SoundInstance, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::SoundInstance)
SB_META_METHOD(Stop)
SB_META_METHOD(FadeOut)
SB_META_PROPERTY_WO(Volume, SetVolume)
SB_META_PROPERTY_WO(Pan, SetPan)
SB_META_PROPERTY_WO(Pitch, SetPitch)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Sound, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Sound)
SB_META_METHOD(Play)
SB_META_METHOD(PlayEx)
SB_META_METHOD(PlayExControl)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::SoundManager, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::SoundManager)
SB_META_METHOD(GetSound)
SB_META_METHOD(PlayMusic)
SB_META_METHOD(PlayMusicEx)
SB_META_METHOD(ClearCache)
SB_META_METHOD(Pause)
SB_META_METHOD(Resume)
SB_META_PROPERTY_RW_DEF(SoundsVolume)
SB_META_PROPERTY_RW_DEF(MusicVolume)
SB_META_PROPERTY_WO(SoundsDir,SetSoundsDir)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::TileMapLayer)
SB_META_CONSTRUCTOR((size_t,size_t))
SB_META_METHOD(GetAt)
SB_META_METHOD(SetAt)
SB_META_PROPERTY_RO(Width, GetWidth)
SB_META_PROPERTY_RO(Height, GetHeight)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::TileMap)
SB_META_CONSTRUCTOR((size_t,size_t))
SB_META_METHOD(GetLayer)
SB_META_METHOD(SetLayer)
SB_META_PROPERTY_RO(Width, GetWidth)
SB_META_PROPERTY_RO(Height, GetHeight)
bind( static_method( "LoadTileMapTMX" , &Sandbox::LoadTileMapTMX ) );
SB_META_END_KLASS_BIND()

struct UTF8 {
    static  sb::string GetChar(const char* src) {
        Sandbox::UTF32Char ch = 0;
        const char* next = Sandbox::get_char(src, ch);
        if (!ch) return "";
        sb::string res;
        res.assign(src,next);
        return res;
    }
    static GHL::UInt32 GetCode(const char* src) {
        Sandbox::UTF32Char ch = 0;
        Sandbox::get_char(src, ch);
        return ch;
    }
    static size_t GetLength(const char* src) {
        Sandbox::UTF32Char ch = 0;
        size_t len = 0;
        while (*src) {
            src = Sandbox::get_char(src, ch);
            ++len;
        }
        return len;
    }
    static int CharIterate(lua_State* L) {
        const char* str = Sandbox::luabind::stack<const char*>::get(L, 1);
        int pos = luaL_optint(L, 2, 1);
        str += pos - 1;
        if (*str == 0) {
            lua_pushnil(L);
            return 1;
        }
        Sandbox::UTF32Char ch = 0;
        const char* next = Sandbox::get_char(str, ch);
        lua_pushlstring(L, str, next-str);
        lua_pushinteger(L, pos + next-str);
        return 2;
    }
};

SB_META_DECLARE_KLASS(UTF8, void)
SB_META_BEGIN_KLASS_BIND(UTF8)
SB_META_STATIC_METHOD(GetChar)
SB_META_STATIC_METHOD(GetCode)
SB_META_STATIC_METHOD(GetLength)
SB_META_STATIC_METHOD(CharIterate)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    void register_resources( lua_State* lua ) {
        luabind::RawClass<Color>(lua);
        luabind::RawClass<Rectf>(lua);
        luabind::RawClass<Recti>(lua);
        luabind::RawClass<Sizef>(lua);
        luabind::RawClass<Sizei>(lua);
        luabind::RawClass<Pointf>(lua);
        luabind::RawClass<Pointi>(lua);
        luabind::ExternClass<FontData>(lua);
        luabind::Class<FontPass>(lua);
        luabind::Class<Font>(lua);
        luabind::Class<FontDataProvider>(lua);
        luabind::Class<OutlineFontDataProvider>(lua);
        luabind::Class<BitmapFont>(lua);
        luabind::Class<SystemFont>(lua);
        luabind::Class<FontChain>(lua);
        luabind::Enum<FontAlign>(lua);
        luabind::Class<Image>(lua);
        luabind::Class<ImageBox>(lua);
        luabind::ExternClass<FileProvider>(lua);
        luabind::ExternClass<Resources>(lua);
        luabind::Class<Texture>(lua);
        luabind::Class<RenderTarget>(lua);
        luabind::Class<SoundInstance>(lua);
        luabind::Class<Sound>(lua);
        luabind::ExternClass<SoundManager>(lua);
        luabind::ExternClass<UTF8>(lua);
        luabind::Class<TileMapLayer>(lua);
        luabind::Class<TileMap>(lua);
        luabind::ExternClass<ShaderUniform>(lua);
        luabind::ExternClass<ShaderFloatUniform>(lua);
        luabind::ExternClass<ShaderVec2Uniform>(lua);
        luabind::ExternClass<ShaderColorUniform>(lua);
        luabind::ExternClass<ShaderMat4Uniform>(lua);
        luabind::ExternClass<Shader>(lua);
        luabind::ExternClass<Bitmask>(lua);
        
    }

}
