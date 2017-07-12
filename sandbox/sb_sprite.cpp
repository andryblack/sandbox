/*
 *  sb_sprite.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_sprite.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::Sprite, Sandbox::SceneObjectWithPosition)
SB_META_DECLARE_OBJECT(Sandbox::ColorizedSprite, Sandbox::Sprite)
SB_META_DECLARE_OBJECT(Sandbox::SpriteFill, Sandbox::Sprite)
SB_META_DECLARE_OBJECT(Sandbox::SpriteBox, Sandbox::SceneObjectWithPosition)
SB_META_DECLARE_OBJECT(Sandbox::ColorizedSpriteBox, Sandbox::SpriteBox)
SB_META_DECLARE_OBJECT(Sandbox::SpriteWithMask, Sandbox::Sprite)

namespace Sandbox {
	
	Sprite::Sprite() {
	}
	
	Sprite::~Sprite() {
	}
	
	void Sprite::Draw(Graphics& g) const {
        if (m_image)
            g.DrawImage(*m_image,GetDrawAttributes().get(),GetPos());
	}
    
    bool Sprite::CheckHit(const Vector2f& pos) const {
        if (!m_image)
            return false;
        if (!GetVisible())
            return false;
        Vector2f local = GlobalToLocal(pos);
        Vector2f hs = -m_image->GetHotspotReal();
        if (local.x < hs.x)
            return false;
        local.x -= hs.x;
        if (local.y < hs.y)
            return false;
        local.y -= hs.y;
        if (local.x > m_image->GetWidth())
            return false;
        if (local.y > m_image->GetHeight())
            return false;
        return true;
    }

    void SpriteFill::SetRect(const Rectf& rect) {
        SetPos(Vector2f(rect.x, rect.y));
        m_size.x = rect.w;
        m_size.y = rect.h;
    }
    
    Rectf SpriteFill::GetRect() const {
        return Rectf(GetPos().x,GetPos().y,m_size.x,m_size.y);
    }
    
    void SpriteFill::Draw( Graphics& g ) const {
        if (m_image) {
            float y = GetPos().y;
            float ey = y + m_size.y;
            float ex = GetPos().x + m_size.x;
            for (;y<=ey;y+=m_image->GetHeight()) {
                float ys = std::min(m_image->GetHeight(), ey-y);
                float dys = ys / m_image->GetHeight();
                float x = GetPos().x;
                for (;x<ex;x+=m_image->GetWidth()) {
                    float xs = std::min(m_image->GetWidth(), ex-x);
                    float dxs = xs / m_image->GetWidth();
                    Image img(*m_image);
                    img.SetSize(img.GetWidth()*dxs, img.GetHeight()*dys);
                    img.SetTextureRect(img.GetTextureX(), img.GetTextureY(), img.GetTextureW()*dxs, img.GetTextureH()*dys);
                    g.DrawImage(img, GetDrawAttributes().get(), x, y);
                }
            }
        }
    }
    void ColorizedSprite::Draw(Graphics& g) const {
        if (m_image)
            g.DrawImage(*m_image,GetDrawAttributes().get(),GetPos(),m_color);
    }
    
    
    void SpriteBox::Draw(Graphics& g) const {
        if (m_image)
            g.DrawImageBox(*m_image,GetDrawAttributes().get(),GetPos(),GetSize());
	}
    
    void ColorizedSpriteBox::Draw(Graphics& g) const {
        if (m_image) {
            Color prev = g.GetColor();
            g.SetColor(prev*m_color);
            g.DrawImageBox(*m_image,GetDrawAttributes().get(),GetPos(),GetSize());
            g.SetColor(prev);
        }
    }
    
    void SpriteWithMask::Draw( Graphics& g ) const {
        if (m_image) {
            if (m_mask) {
                MaskMode m = g.GetMaskMode();
                TexturePtr t = g.GetMaskTexture();
                Transform2d tr = g.GetMaskTransform();
                
                Rectf rect(GetPos().x-m_mask->GetHotspot().x*m_mask->GetWidth()/m_mask->GetTextureW(),
                      GetPos().y-m_mask->GetHotspot().y*m_mask->GetHeight()/m_mask->GetTextureH(),
                      m_mask->GetWidth(),
                      m_mask->GetHeight());
                g.SetMask(MASK_MODE_ALPHA, *m_mask, rect);
                
                g.DrawImage(*m_image,GetDrawAttributes().get(),GetPos());
                g.SetMask(m, t, tr);
                
            } else {
                g.DrawImage(*m_image,GetDrawAttributes().get(),GetPos());
            }
        }
    }
}

