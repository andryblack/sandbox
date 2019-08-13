#ifndef SB_TRANSFORM_COMPONENTS_H_INCLUDED
#define SB_TRANSFORM_COMPONENTS_H_INCLUDED

#include "sb_vector2.h"
#include "meta/sb_meta.h"

namespace Sandbox {
    
    
    class TransformComponents : public meta::object {
        SB_META_OBJECT
    protected:
        Vector2f    m_translate;
        Vector2f    m_scale;
        float       m_angle;
    public:
        TransformComponents();
        
        void SetTranslate(const Vector2f& tr) { m_translate = tr; }
        const Vector2f& GetTranslate() const { return m_translate;}
        void SetScaleXY(const Vector2f& v) { m_scale = v;}
        const Vector2f& GetScaleXY() const { return m_scale; }
        
        void SetTranslateX(float x) { m_translate.x = x; }
        float GetTranslateX() const { return m_translate.x;}
        void SetTranslateY(float y) { m_translate.y = y; }
        float GetTranslateY() const { return m_translate.y;}
        void SetScale(float s) { m_scale.x = m_scale.y = s;}
        float GetScale() const { return (m_scale.x + m_scale.y)*0.5f; }
        void SetScaleX(float s) { m_scale.x = s;}
        float GetScaleX() const { return m_scale.x;}
        void SetScaleY(float s) { m_scale.y = s;}
        float GetScaleY() const { return m_scale.y;}
        void SetAngle(float a) { m_angle = a;}
        float GetAngle() const { return m_angle;}
    };
    typedef sb::intrusive_ptr<TransformComponents> TransformComponentsPtr;
    
}

#endif // SB_TRANSFORM_COMPONENTS_H_INCLUDED

