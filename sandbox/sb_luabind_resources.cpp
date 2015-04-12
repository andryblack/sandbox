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
#include "sb_font.h"
#include "sb_bitmap_font.h"
#include "sb_shader.h"
#include "sb_color.h"
#include "sb_rect.h"
#include "sb_sound.h"
#include "sb_rendertarget.h"
#include "sb_utf.h"

SB_META_DECLARE_KLASS(Sandbox::Shader, void)

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
SB_META_STATIC_METHOD(FromString)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Rectf, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Rectf)
SB_META_CONSTRUCTOR((float,float,float,float))
SB_META_PROPERTY(x)
SB_META_PROPERTY(y)
SB_META_PROPERTY(w)
SB_META_PROPERTY(h)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Recti, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Recti)
SB_META_CONSTRUCTOR((int,int,int,int))
SB_META_PROPERTY(x)
SB_META_PROPERTY(y)
SB_META_PROPERTY(w)
SB_META_PROPERTY(h)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Texture, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Texture)
SB_META_METHOD(SetFiltered)
SB_META_PROPERTY_RO(Width, GetOriginalWidth)
SB_META_PROPERTY_RO(Height, GetOriginalHeight)
SB_META_PROPERTY_RW(Filtered, GetFiltered, SetFiltered)
SB_META_PROPERTY_RW(Tiled,GetTiled, SetTiled)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::RenderTarget, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::RenderTarget)
SB_META_PROPERTY_RO(Texture, GetTexture)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Image, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Image)
SB_META_CONSTRUCTOR((Sandbox::TexturePtr,float,float,float,float))
SB_META_METHOD(SetSize)
SB_META_METHOD(SetTextureRect)
SB_META_PROPERTY_RO(TextureX,GetTextureX)
SB_META_PROPERTY_RO(TextureY,GetTextureY)
SB_META_PROPERTY_RO(TextureW,GetTextureW)
SB_META_PROPERTY_RO(TextureH,GetTextureH)
SB_META_PROPERTY_RO(Width,GetWidth)
SB_META_PROPERTY_RO(Height,GetHeight)
SB_META_PROPERTY_RW(Texture,GetTexture,SetTexture)
SB_META_PROPERTY_RW(Hotspot,GetHotspot,SetHotspot)
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

SB_META_BEGIN_KLASS_BIND(Sandbox::Font)
SB_META_PROPERTY_RO(Height, GetHeight)
SB_META_PROPERTY_RO(Size, GetSize)
SB_META_PROPERTY_RO(Baseline, GetBaseline)
SB_META_PROPERTY_RO(XHeight, GetXHeight)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::BitmapFont)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(Reserve)
SB_META_METHOD(AddGlypth)
SB_META_METHOD(AddKerningPair)
SB_META_METHOD(SetHeight)
SB_META_METHOD(SetSize)
SB_META_METHOD(SetBaseline)
SB_META_METHOD(SetXHeight)
SB_META_METHOD(FixupChars)
SB_META_END_KLASS_BIND()


SB_META_DECLARE_KLASS(Sandbox::Resources, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Resources)
SB_META_METHOD(GetImage)
SB_META_METHOD(GetTexture)
SB_META_METHOD(GetShader)
SB_META_METHOD(CreateRenderTarget)
SB_META_PROPERTY_WO(BasePath, SetBasePath)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::SoundInstance, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::SoundInstance)
SB_META_METHOD(Stop)
SB_META_METHOD(FadeOut)
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
SB_META_PROPERTY_RW_DEF(SoundsVolume)
SB_META_PROPERTY_RW_DEF(MusicVolume)
SB_META_PROPERTY_WO(SoundsDir,SetSoundsDir)
SB_META_END_KLASS_BIND()

struct UTF8 {
    static  sb::string GetChar(const sb::string& src) {
        Sandbox::UTF32Char ch = 0;
        const char* next = Sandbox::get_char(src.c_str(), ch);
        sb::string res;
        res.assign(src.c_str(),next);
        return res;
    }
};

SB_META_DECLARE_KLASS(UTF8, void)
SB_META_BEGIN_KLASS_BIND(UTF8)
SB_META_STATIC_METHOD(GetChar)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    void register_resources( lua_State* lua ) {
        luabind::RawClass<Color>(lua);
        luabind::RawClass<Rectf>(lua);
        luabind::RawClass<Recti>(lua);
        luabind::Class<Font>(lua);
        luabind::Class<BitmapFont>(lua);
        luabind::Enum<FontAlign>(lua);
        luabind::Class<Image>(lua);
        luabind::Class<ImageBox>(lua);
        luabind::ExternClass<Resources>(lua);
        luabind::Class<Texture>(lua);
        luabind::Class<RenderTarget>(lua);
        luabind::Class<SoundInstance>(lua);
        luabind::Class<Sound>(lua);
        luabind::ExternClass<SoundManager>(lua);
        luabind::ExternClass<UTF8>(lua);
    }

}
