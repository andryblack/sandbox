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
#include "sb_transform_components.h"
#include "sb_transform2d.h"

namespace Sandbox {
    
    class Graphics;
    
    
    
    class ColorModificator : public meta::object {
        SB_META_OBJECT
    protected:
        Color   m_color;
    public:
        
        const Color& GetColor() const { return m_color; }
        void SetColor(const Color& c) { m_color = c; }
        float GetAlpha() const { return m_color.a; }
        void SetAlpha(float a) { m_color.a = a; }
        
        void Apply(Graphics& g) const;
    };
    typedef sb::intrusive_ptr<ColorModificator> ColorModificatorPtr;
    
    class TransformModificator : public TransformComponents {
        SB_META_OBJECT
    protected:
        float       m_screw_x;
        Vector2f    m_origin;
    public:
        TransformModificator();
        
        void SetOrigin(const Vector2f& tr) { m_origin = tr; }
        const Vector2f& GetOrigin() const { return m_origin;}
        
        void SetScrewX(float a) { m_screw_x = a;}
        float GetScrewX() const { return m_screw_x;}
        

        void UnTransform(Vector2f& v) const;
        void Transform(Vector2f& v) const;
        void Apply(Graphics& g) const;
        void Apply(Transform2d& tr) const;
        
        Transform2d GetTransform() const;
    };
    typedef sb::intrusive_ptr<TransformModificator> TransformModificatorPtr;
}

#endif /* defined(__sr_osx__sb_draw_modificator__) */
