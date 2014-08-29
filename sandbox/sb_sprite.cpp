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

SB_META_DECLARE_OBJECT(Sandbox::Sprite, Sandbox::SceneObject)
SB_META_DECLARE_OBJECT(Sandbox::ColorizedSprite, Sandbox::Sprite)
SB_META_DECLARE_OBJECT(Sandbox::SpriteFill, Sandbox::Sprite)

namespace Sandbox {
	
	Sprite::Sprite() {
	}
	
	Sprite::~Sprite() {
	}
	
	void Sprite::Draw(Graphics& g) const {
        if (m_image)
            g.DrawImage(*m_image,m_pos);
	}

    void SpriteFill::SetRect(const Rectf& rect) {
        SetPos(Vector2f(rect.x, rect.y));
        m_size.x = rect.x;
        m_size.y = rect.y;
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
                }
            }
        }
    }
    void ColorizedSprite::Draw(Graphics& g) const {
        if (m_image)
            g.DrawImage(*m_image,m_pos,m_color);
    }
    
    
    
}

