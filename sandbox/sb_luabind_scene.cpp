//
//  sb_luabind_scene.cpp
//  YinYang
//
//  Created by Андрей Куницын on 7/9/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "luabind/sb_luabind.h"
#include "meta/sb_meta.h"
#include "sb_scene_object.h"
#include "sb_sprite.h"
#include "sb_clear_scene.h"
#include "sb_fill_rect.h"
#include "sb_background.h"
#include "sb_label.h"
#include "sb_container.h"
#include "sb_container_blend.h"
#include "sb_container_visible.h"
#include "sb_container_transform3d.h"
#include "sb_container_shader.h"
#include "sb_scene.h"
#include "sb_debug_geometry.h"
#include "sb_rt_scene.h"
#include "sb_geomerty_buffer.h"
#include "sb_container_clip.h"
#include "sb_container_scroll.h"
#include "sb_text_box.h"

SB_META_BEGIN_KLASS_BIND(Sandbox::DrawAttributes)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::SceneObject)
SB_META_PROPERTY_RW(Visible,GetVisible,SetVisible)
SB_META_PROPERTY_RW(Order, GetOrder, SetOrder)
SB_META_PROPERTY_RW_DEF(DrawAttributes)
SB_META_METHOD(GlobalToLocal)
SB_META_METHOD(LocalToGlobal)
SB_META_METHOD(RemoveFromScene)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::SceneObjectWithPosition)
SB_META_PROPERTY_RW(Pos,GetPos,SetPos)
SB_META_METHOD(Move)
SB_META_METHOD(MoveX)
SB_META_METHOD(MoveY)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::Sprite)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Image,GetImage,SetImage)
SB_META_METHOD(CheckHit)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::SpriteBox)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Image,GetImage,SetImage)
SB_META_PROPERTY_RW(Size,GetSize,SetSize)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::SpriteFill)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Size,GetSize,SetSize)
SB_META_PROPERTY_RW(Rect,GetRect,SetRect)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ColorizedSprite)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Color,GetColor,SetColor)
SB_META_PROPERTY_RW_DEF(Alpha)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ColorizedSpriteBox)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW_DEF(Color)
SB_META_PROPERTY_RW_DEF(Alpha)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::SpriteWithMask)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Mask, GetMask, SetMask)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ClearScene)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Color,GetColor,SetColor)
SB_META_PROPERTY_RW(Depth,GetDepth,SetDepth)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::FillRect)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Texture,GetTexture,SetTexture)
SB_META_PROPERTY_RW(Rect,GetRect,SetRect)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::Background)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(Load)
SB_META_METHOD(Clear)
SB_META_PROPERTY_RO(Width, GetWidth)
SB_META_PROPERTY_RO(Height, GetHeight)
SB_META_PROPERTY_WO(Fullscreen, SetFullScreen)
SB_META_PROPERTY_WO(Filtered,SetFiltered)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::Label)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Font,GetFont,SetFont)
SB_META_PROPERTY_RW(Text,GetText,SetText)
SB_META_PROPERTY_RW(Align,GetAlign,SetAlign)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::FontDrawAttributes)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ColorizedLabel)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Color,GetColor,SetColor)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::TextBox)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Width,GetWidth,SetWidth)
SB_META_PROPERTY_RO(Size,GetSize)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::TextDrawAttributes)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(SetPassColor)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::CircleObject)
SB_META_CONSTRUCTOR((const Sandbox::Vector2f&,float))
SB_META_PROPERTY_RW_DEF(R)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::LineObject)
SB_META_CONSTRUCTOR((const Sandbox::Vector2f&,const Sandbox::Vector2f&))
SB_META_PROPERTY_RW_DEF(A)
SB_META_PROPERTY_RW_DEF(B)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::Container)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(Reserve)
SB_META_METHOD(AddObject)
SB_META_METHOD(RemoveObject)
SB_META_METHOD(Clear)
SB_META_METHOD(GetTransformModificator)
SB_META_METHOD(SetTransformModificator)
SB_META_METHOD(GetColorModificator)
SB_META_METHOD(SetColorModificator)
SB_META_PROPERTY_RW_DEF(Translate)
SB_META_PROPERTY_RW_DEF(Scale)
SB_META_PROPERTY_RW_DEF(Angle)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::BlendMode,void);
SB_META_ENUM_BIND(Sandbox::BlendMode,namespace Sandbox,
                        SB_META_ENUM_ITEM(BLEND_MODE_COPY)
                        SB_META_ENUM_ITEM(BLEND_MODE_ALPHABLEND)
                        SB_META_ENUM_ITEM(BLEND_MODE_ADDITIVE)
                        SB_META_ENUM_ITEM(BLEND_MODE_ADDITIVE_ALPHA)
                        SB_META_ENUM_ITEM(BLEND_MODE_MULTIPLY)
                        SB_META_ENUM_ITEM(BLEND_MODE_SCREEN))

SB_META_DECLARE_KLASS(Sandbox::MaskMode,void);
SB_META_ENUM_BIND(Sandbox::MaskMode,namespace Sandbox,
                  SB_META_ENUM_ITEM(MASK_MODE_NONE)
                  SB_META_ENUM_ITEM(MASK_MODE_ALPHA)
                  SB_META_ENUM_ITEM(MASK_MODE_SCREEN))


SB_META_BEGIN_KLASS_BIND(Sandbox::ContainerBlend)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Mode,GetMode,SetMode)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ContainerClip)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Rect,GetRect,SetRect)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ContainerMask)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Mode,GetMode,SetMode)
SB_META_PROPERTY_RW(Texture,GetTexture,SetTexture)
SB_META_PROPERTY_RW(Transform,GetTransform,SetTransform)
SB_META_END_KLASS_BIND()


SB_META_BEGIN_KLASS_BIND(Sandbox::ContainerVisible)
SB_META_PROPERTY_RW_DEF(Invert)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ContainerShader)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW_DEF(Shader)
SB_META_PROPERTY_RW_DEF(Texture)
SB_META_PROPERTY_RW_DEF(Enabled)
SB_META_PROPERTY_RO(MaskTransform, GetMaskTransform)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ContainerScroll)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(ScrollBegin)
SB_META_METHOD(ScrollMove)
SB_META_METHOD(ScrollEnd)
SB_META_METHOD(Reset)
SB_META_PROPERTY_RW_DEF(ContentSize)
SB_META_PROPERTY_RW_DEF(ViewSize)
SB_META_PROPERTY_RW_DEF(ViewPos)
SB_META_PROPERTY_RW_DEF(Bounds)
SB_META_PROPERTY_RW_DEF(VEnabled)
SB_META_PROPERTY_RW_DEF(HEnabled)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::TransformModificator)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Translate,GetTranslate,SetTranslate)
SB_META_PROPERTY_RW(TranslateX,GetTranslateX,SetTranslateX)
SB_META_PROPERTY_RW(TranslateY,GetTranslateY,SetTranslateY)
SB_META_PROPERTY_RW(Scale,GetScale,SetScale)
SB_META_PROPERTY_RW(ScaleX,GetScaleX,SetScaleX)
SB_META_PROPERTY_RW(ScaleY,GetScaleY,SetScaleY)
SB_META_PROPERTY_RW(Angle,GetAngle,SetAngle)
SB_META_PROPERTY_RW(ScrewX,GetScrewX,SetScrewX)
SB_META_END_KLASS_BIND()


SB_META_BEGIN_KLASS_BIND(Sandbox::ContainerTransform3d)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW_DEF(ProjectionMatrix)
SB_META_PROPERTY_RW_DEF(ViewMatrix)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ColorModificator)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW(Color,GetColor,SetColor)
SB_META_PROPERTY_RW(Alpha,GetAlpha,SetAlpha)
SB_META_END_KLASS_BIND()


SB_META_BEGIN_KLASS_BIND(Sandbox::GeometryBuffer)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW_DEF(Color)
SB_META_PROPERTY_RW_DEF(Image)
SB_META_METHOD(Clear)
SB_META_METHOD(BuildLine)
SB_META_METHOD(BuildContour)
SB_META_METHOD(BuildFill)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::Scene)
SB_META_PROPERTY_RW_DEF(Scale)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::RTScene)
SB_META_CONSTRUCTOR((const Sandbox::RenderTargetPtr&))
SB_META_PROPERTY_RO(Target, GetTarget)
SB_META_END_KLASS_BIND()


namespace Sandbox {
    

    
    void register_scene( lua_State* lua ) {
        luabind::Class<DrawAttributes>(lua);
        luabind::Class<FontDrawAttributes>(lua);
        luabind::Class<TextDrawAttributes>(lua);
        luabind::Class<SceneObject>(lua);
        luabind::Class<SceneObjectWithPosition>(lua);
        luabind::Class<Sprite>(lua);
        luabind::Class<SpriteBox>(lua);
        luabind::Class<SpriteFill>(lua);
        luabind::Class<ColorizedSprite>(lua);
        luabind::Class<ColorizedSpriteBox>(lua);
        luabind::Class<SpriteWithMask>(lua);
        luabind::Class<ClearScene>(lua);
        luabind::Class<FillRect>(lua);
        luabind::Class<Background>(lua);
        luabind::Class<Label>(lua);
        luabind::Class<ColorizedLabel>(lua);
        luabind::Class<TextBox>(lua);
        luabind::Class<CircleObject>(lua);
        luabind::Class<LineObject>(lua);
        luabind::Class<Container>(lua);
        luabind::Enum<BlendMode>(lua);
        luabind::Enum<MaskMode>(lua);
        luabind::Class<ContainerBlend>(lua);
        luabind::Class<ContainerClip>(lua);
        luabind::Class<ContainerMask>(lua);
        luabind::Class<TransformModificator>(lua);
        luabind::Class<ColorModificator>(lua);
        luabind::Class<ContainerVisible>(lua);
        luabind::Class<ContainerTransform3d>(lua);
        luabind::Class<ContainerShader>(lua);
        luabind::Class<ContainerScroll>(lua);
        luabind::Class<GeometryBuffer>(lua);
        luabind::ExternClass<Scene>(lua);
        luabind::Class<RTScene>(lua);
    }

}
