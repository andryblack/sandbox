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
SB_META_DECLARE_OBJECT(Sandbox::TransformModificator, Sandbox::TransformComponents)

namespace Sandbox {
    
    
    
    void ColorModificator::Apply(Sandbox::Graphics &g) const {
        g.SetColor(g.GetColor()*m_color);
    }
   
    TransformModificator::TransformModificator() : m_screw_x(0.0f) {
    }
    
    void TransformModificator::Apply(Graphics &g) const {
        Transform2d tr = g.GetTransform();
        Apply(tr);
        g.SetTransform(tr);
    }
    void TransformModificator::Apply(Transform2d& tr) const {
        tr.translate(m_origin + m_translate);
        if (m_screw_x != 0.0f)
            tr.screw_x(m_screw_x);
        tr.rotate_scale(m_angle,m_scale.x,m_scale.y);
        tr.translate(-m_origin);
    }
    Transform2d TransformModificator::GetTransform() const {
        Transform2d res;
        Apply(res);
        return res;
    }
    void TransformModificator::UnTransform(Vector2f& v) const {
        Transform2d tr;
        Apply(tr);
        tr.inverse();
        v = tr.transform(v);
    }
    void TransformModificator::Transform(Vector2f& v) const {
        Transform2d tr;
        Apply(tr);
        v = tr.transform(v);
    }
}
