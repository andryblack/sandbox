//
//  sb_draw_modificator.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/17/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_draw_modificator.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::DrawModificator, Sandbox::meta::object)
SB_META_DECLARE_OBJECT(Sandbox::ColorModificator, Sandbox::DrawModificator)
SB_META_DECLARE_OBJECT(Sandbox::TransformModificator, Sandbox::DrawModificator)

namespace Sandbox {
    
    DrawModificatorsStack::DrawModificatorsStack() : m_store_mask(0) {
        
    }
    
    void DrawModificatorsStack::AddModificatorImpl( const DrawModificatorPtr& m ) {
        m_modificators.push_back(m);
        UpdateStoreMask();
    }
    void DrawModificatorsStack::RemoveModificatorImpl( const DrawModificatorPtr& m ) {
        for (sb::vector<DrawModificatorPtr>::iterator it = m_modificators.begin();it!=m_modificators.end();++it) {
            if ((*it) == m) {
                m_modificators.erase(it);
                UpdateStoreMask();
                return;
            }
        }
    }
    
    void DrawModificatorsStack::UpdateStoreMask() {
        m_store_mask = 0;
        for (sb::vector<DrawModificatorPtr>::const_iterator it = m_modificators.begin();it!=m_modificators.end();++it) {
            m_store_mask = m_store_mask | (*it)->GetStoreMask();
        }
    }
    
    void DrawModificatorsStack::ProcessModificators(Graphics &g) const {
        if (m_store_mask & DrawModificator::STORE_COLOR) {
            ProcessWithStoreColor(g);
        } else if (m_store_mask & DrawModificator::STORE_TRANSFORM) {
            ProcessWithStoreTransform(g);
        } else {
            ProcessStack(g);
        }
    }
    
    void    DrawModificatorsStack::ProcessWithStoreColor(Graphics& g) const {
        Color c = g.GetColor();
        if (m_store_mask & DrawModificator::STORE_TRANSFORM) {
            ProcessWithStoreTransform(g);
        } else {
            ProcessStack(g);
        }
        g.SetColor(c);
    }
    void    DrawModificatorsStack::ProcessWithStoreTransform(Graphics& g) const {
        Transform2d tr = g.GetTransform();
        ProcessStack(g);
        g.SetTransform(tr);
    }
    void    DrawModificatorsStack::ProcessStack(Graphics& g) const {
        for (sb::vector<DrawModificatorPtr>::const_iterator it = m_modificators.begin();it!=m_modificators.end();++it) {
            (*it)->Apply(g);
        }
        DrawWithModificators(g);
    }

    
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
}
