//
//  sb_debug_geometry.cpp
//  caleydoscope
//
//  Created by Andrey Kunitsyn on 7/4/13.
//  Copyright (c) 2013 andryblack. All rights reserved.
//

#include "sb_debug_geometry.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::CircleObject, Sandbox::SceneObjectWithPosition)
SB_META_DECLARE_OBJECT(Sandbox::LineObject, Sandbox::SceneObject)

namespace Sandbox {
    
    CircleObject::CircleObject(const Vector2f& pos, float r) : m_r(r) {
        SetPos(pos);
    }
    
    void CircleObject::Draw(Graphics &g) const {
        g.DrawCircle(GetPos(), m_r);
    }
    
    LineObject::LineObject(const Vector2f& a, const Vector2f& b) : m_a(a),m_b(b) {
        
    }
    
    void LineObject::Draw(Graphics& g) const {
        g.DrawLine(m_a, m_b);
    }
}