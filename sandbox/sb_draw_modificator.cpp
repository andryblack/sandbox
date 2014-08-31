//
//  sb_draw_modificator.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/17/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_draw_modificator.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::ColorModificator, meta::object)
SB_META_DECLARE_OBJECT(Sandbox::TransformModificator, meta::object)

namespace Sandbox {
    
    
    
    void ColorModificator::Apply(Sandbox::Graphics &g) const {
        g.SetColor(g.GetColor()*m_color);
    }
   
    TransformModificator::TransformModificator() : m_scale_x(1.0f),m_scale_y(1.0f),m_angle(0.0f) {
    }
    
    void TransformModificator::Apply(Graphics &g) const {
        Transform2d tr = g.GetTransform();
        tr.translate(m_translate).rotate(m_angle).scale(m_scale_x,m_scale_y);
        g.SetTransform(tr);
    }
    void TransformModificator::Transform(Vector2f& v) const {
        v = (v - m_translate).rotate(-m_angle);
        v.x /= m_scale_x;
        v.y /= m_scale_y;
    }
}
