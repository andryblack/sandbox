/*
 *  sb_container.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 11.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container.h"
#include "sb_graphics.h"
#include <algorithm>

SB_META_DECLARE_OBJECT(Sandbox::Container, Sandbox::ContainerBase)

namespace Sandbox {

	Container::Container() {
		
	}
	
	Container::~Container() {
	}
	
    void Container::DrawImpl(Graphics& g) const {
        
        
        Transform2d tr = g.GetTransform();
        Color clr = g.GetColor();
        if (m_transform) {
            m_transform->Apply(g);
        }
        if (m_color) {
            m_color->Apply(g);
        }
        
        DrawChilds(g);

        if (m_color) {
            g.SetColor(clr);
        }
        if (m_transform) {
            g.SetTransform(tr);
        }
    }
    
    void Container::Draw(Sandbox::Graphics &g) const {
        if (m_objects.empty()) return;
        DrawImpl(g);
    }
    
    
    void Container::SetTransformModificator(const TransformModificatorPtr& ptr) {
        m_transform = ptr;
    }
    TransformModificatorPtr Container::GetTransformModificator() {
        if (!m_transform) {
            m_transform.reset(new TransformModificator());
        }
        return m_transform;
    }
    
    void Container::SetColorModificator(const ColorModificatorPtr& ptr) {
        m_color = ptr;
    }
    ColorModificatorPtr Container::GetColorModificator() {
        if (!m_color) {
            m_color.reset(new ColorModificator());
        }
        return m_color;
    }
    
    void Container::SetTranslate(const Vector2f& tr) {
        GetTransformModificator()->SetTranslate(tr);
    }
    Vector2f Container::GetTranslate() const {
        if (m_transform) return m_transform->GetTranslate();
        return Vector2f();
    }
    
    void Container::SetScale(float s) {
        GetTransformModificator()->SetScale(s);
    }
    float Container::GetScale() const {
        if (m_transform) return m_transform->GetScale();
        return 1.0f;
    }
    
    void Container::SetAngle(float a) {
        GetTransformModificator()->SetAngle(a);
    }
    float Container::GetAngle() const {
        if (m_transform) return m_transform->GetAngle();
        return 0.0f;
    }
    
    void Container::SetAlpha(float a) {
        GetColorModificator()->SetAlpha(a);
    }
    float Container::GetAlpha() const {
        if (m_color) return m_color->GetAlpha();
        return 1.0f;
    }
    
    void Container::SetColor(const Color& c) {
        GetColorModificator()->SetColor(c);
    }
    const Color& Container::GetColor() const {
        if (m_color) return m_color->GetColor();
        return Color::white();
    }

    
    void Container::GlobalToLocalImpl(Vector2f& v) const {
        SceneObject::GlobalToLocalImpl(v);
        if (m_transform) {
            m_transform->UnTransform(v);
        }
    }
    
    void Container::GetTransformImpl(Transform2d& tr) const {
        SceneObject::GetTransformImpl(tr);
        if (m_transform) {
            m_transform->Apply(tr);
        }
    }
    void Container::GetTransformToImpl(const SceneObject* root, Transform2d& tr) const {
        SceneObject::GetTransformToImpl(root,tr);
        if (m_transform && (root!=this)) {
            m_transform->Apply(tr);
        }
    }
}
