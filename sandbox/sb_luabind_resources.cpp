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

SB_META_DECLARE_KLASS(Sandbox::Shader, void)

SB_META_DECLARE_KLASS(Sandbox::Color, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Color)
SB_META_CONSTRUCTOR((float,float,float,float))
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

SB_META_DECLARE_KLASS(Sandbox::Image, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Image)
SB_META_CONSTRUCTOR((Sandbox::TexturePtr,float,float,float,float))
SB_META_METHOD(SetSize)
SB_META_PROPERTY_RO(TextureX,GetTextureX)
SB_META_PROPERTY_RO(TextureY,GetTextureY)
SB_META_PROPERTY_RO(TextureW,GetTextureW)
SB_META_PROPERTY_RO(TextureH,GetTextureH)
SB_META_PROPERTY_RO(Width,GetWidth)
SB_META_PROPERTY_RO(Height,GetHeight)
SB_META_PROPERTY_RO(Texture,GetTexture)
SB_META_PROPERTY_RW(Hotspot,GetHotspot,SetHotspot)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::FontAlign,void);
SB_META_ENUM_BIND(Sandbox::FontAlign,namespace Sandbox,
                  SB_META_ENUM_ITEM(ALIGN_LEFT)
                  SB_META_ENUM_ITEM(ALIGN_RIGHT)
                  SB_META_ENUM_ITEM(ALIGN_CENTER))

SB_META_BEGIN_KLASS_BIND(Sandbox::Font)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::BitmapFont)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(Reserve)
SB_META_METHOD(AddGlypth)
SB_META_METHOD(AddKerningPair)
SB_META_END_KLASS_BIND()


SB_META_DECLARE_KLASS(Sandbox::Resources, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Resources)
SB_META_METHOD(GetImage)
SB_META_METHOD(GetTexture)
SB_META_METHOD(GetShader)
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
        luabind::ExternClass<Resources>(lua);
        luabind::Class<Texture>(lua);
    }

}
