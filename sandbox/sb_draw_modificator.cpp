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
   
    TransformModificator::TransformModificator() : m_scale_x(1.0f),m_scale_y(1.0f),m_angle(0.0f),m_screw_x(0.0f) {
    }
    
    void TransformModificator::Apply(Graphics &g) const {
        Transform2d tr = g.GetTransform();
        Apply(tr);
        g.SetTransform(tr);
    }
    void TransformModificator::Apply(Transform2d& tr) const {
        tr.translate(m_origin);
        tr.translate(m_translate);
        if (m_screw_x != 0.0f)
            tr.screw_x(m_screw_x);
        tr.rotate(m_angle).scale(m_scale_x,m_scale_y);
        tr.translate(-m_origin);
    }
    void TransformModificator::Transform(Vector2f& v) const {
        Transform2d tr;
        Apply(tr);
        tr.inverse();
        v = tr.transform(v);
    }
}
