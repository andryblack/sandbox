//
//  sb_draw_modificator.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/17/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_draw_modificator__
#define __sr_osx__sb_draw_modificator__

#include <sbstd/sb_shared_ptr.h>
#include <meta/sb_meta.h>
#include <sbstd/sb_vector.h>
#include "sb_color.h"
#include "sb_transform2d.h"

namespace Sandbox {
    
    class Graphics;
    
    class DrawModificator : public meta::object {
        SB_META_OBJECT
    protected:
        DrawModificator()  {}
    public:
        static const unsigned int STORE_COLOR = 1;
        static const unsigned int STORE_TRANSFORM = 2;
        virtual unsigned int GetStoreMask() const = 0;
        virtual void    Apply(Graphics& g) const = 0;
    };
    typedef sb::shared_ptr<DrawModificator> DrawModificatorPtr;
    
    class DrawModificatorsStack {
    public:
        DrawModificatorsStack();
        void ProcessModificators( Graphics& g ) const;
    protected:
        void AddModificatorImpl( const DrawModificatorPtr& m );
        void RemoveModificatorImpl( const DrawModificatorPtr& m );
        
        virtual void DrawWithModificators( Graphics& g ) const = 0;
    private:
        sb::vector<DrawModificatorPtr>  m_modificators;
        unsigned int m_store_mask;
        void    UpdateStoreMask();
        void    ProcessWithStoreColor(Graphics& g) const;
        void    ProcessWithStoreTransform(Graphics& g) const;
        void    ProcessStack(Graphics& g) const;
    };
    
    
    class ColorModificator : public DrawModificator {
        SB_META_OBJECT
    private:
        Color   m_color;
    public:
        virtual unsigned int GetStoreMask() const { return STORE_COLOR; }
        
        const Color& GetColor() const { return m_color; }
        void SetColor(const Color& c) { m_color = c; }
        float GetAlpha() const { return m_color.a; }
        void SetAlpha(float a) { m_color.a = a; }
        
        void Apply(Graphics& g) const;
    };
    typedef sb::shared_ptr<ColorModificator> ColorModificatorPtr;
    
    class TransformModificator : public DrawModificator {
        SB_META_OBJECT
    private:
        Vector2f	m_translate;
		float		m_scale_x;
		float		m_scale_y;
		float		m_angle;
    public:
        TransformModificator();
        virtual unsigned int GetStoreMask() const { return STORE_TRANSFORM; }
        
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

        void Apply(Graphics& g) const;
    };
    typedef sb::shared_ptr<TransformModificator> TransformModificatorPtr;
}

SB_META_PRIVATE_CLASS(DrawModificatorsStack)

#endif /* defined(__sr_osx__sb_draw_modificator__) */
