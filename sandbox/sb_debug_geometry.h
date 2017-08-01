//
//  sb_debug_geometry.h
//  caleydoscope
//
//  Created by Andrey Kunitsyn on 7/4/13.
//  Copyright (c) 2013 andryblack. All rights reserved.
//

#ifndef __caleydoscope__sb_debug_geometry__
#define __caleydoscope__sb_debug_geometry__

#include "sb_scene_object.h"
#include "sb_vector2.h"

namespace Sandbox {
    
    class CircleObject : public SceneObjectWithPosition {
        SB_META_OBJECT
    public:
        explicit CircleObject(const Vector2f& pos, float r);
        
        virtual void Draw(Graphics& g) const SB_OVERRIDE;
        
        float GetR() const { return m_r; }
        void SetR(float r) { m_r = r; }
    private:
        float   m_r;
    };
    
    
    class LineObject : public SceneObject {
        SB_META_OBJECT
    public:
        explicit LineObject(const Vector2f& a, const Vector2f& b);
        
        virtual void Draw(Graphics& g) const SB_OVERRIDE;
        
        const Vector2f& GetA() const {return m_a; }
        void SetA(const Vector2f& v) { m_a = v; }
        const Vector2f& GetB() const {return m_b; }
        void SetB(const Vector2f& v) { m_b = v; }
    private:
        Vector2f   m_a;
        Vector2f   m_b;
    };
}

#endif /* defined(__caleydoscope__sb_debug_geometry__) */
