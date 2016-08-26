//
//  sb_draw_modificator.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/17/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_draw_modificator__
#define __sr_osx__sb_draw_modificator__

#include "meta/sb_meta.h"
#include <sbstd/sb_vector.h>
#include "sb_color.h"
#include "sb_transform2d.h"

namespace Sandbox {
    
    class Graphics;
    
    
    
    class ColorModificator : public meta::object {
        SB_META_OBJECT
    private:
        Color   m_color;
    public:
        
        const Color& GetColor() const { return m_color; }
        void SetColor(const Color& c) { m_color = c; }
        float GetAlpha() const { return m_color.a; }
        void SetAlpha(float a) { m_color.a = a; }
        
        void Apply(Graphics& g) const;
    };
    typedef sb::intrusive_ptr<ColorModificator> ColorModificatorPtr;
    
    class TransformModificator : public meta::object {
        SB_META_OBJECT
    private:
        Vector2f	m_translate;
		float		m_scale_x;
		float		m_scale_y;
		float		m_angle;
        float       m_screw_x;
    public:
        TransformModificator();
        
        void SetTranslate(const Vector2f& tr) { m_translate = tr; }
		const Vector2f& GetTranslate() const { return m_translate;}
		void SetScale(float s) { m_scale_x = m_scale_y = s;}
        float GetScale() const { return (m_scale_x + m_scale_y)*0.5f; }
		void SetScaleX(float s) { m_scale_x = s;}
		float GetScaleX() const { return m_scale_x;}
		void SetScaleY(float s) { m_scale_y = s;}
		float GetScaleY() const { return m_scale_y;}
		void SetAngle(float a) { m_angle = a;}
		float GetAngle() const { return m_angle;}
        void SetScrewX(float a) { m_screw_x = a;}
        float GetScrewX() const { return m_screw_x;}

        void Transform(Vector2f& v) const;
        void Apply(Graphics& g) const;
        void Apply(Transform2d& tr) const;
    };
    typedef sb::intrusive_ptr<TransformModificator> TransformModificatorPtr;
}

#endif /* defined(__sr_osx__sb_draw_modificator__) */
