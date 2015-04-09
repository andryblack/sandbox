/*
 *  sb_sprite.h
 *  SR
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_SPRITE_H
#define SB_SPRITE_H

#include "sb_vector2.h"
#include "sb_image.h"
#include "sb_scene_object.h"
#include "sb_color.h"
#include "sb_rect.h"

namespace Sandbox {
    
    
	class Sprite : public SceneObjectWithPosition {
        SB_META_OBJECT
    public:
		Sprite();
		~Sprite();
		void SetImage(const ImagePtr& img) { m_image = img;}
		const ImagePtr& GetImage() const { return m_image;}
		virtual void Draw( Graphics& g ) const;
	protected:
		ImagePtr m_image;
	};
	typedef sb::intrusive_ptr<Sprite> SpritePtr;
    
    class SpriteFill : public Sprite {
        SB_META_OBJECT
    public:
		SpriteFill() {}
		~SpriteFill() {}
		void SetSize(const Vector2f& s) { m_size = s; }
        const Vector2f& GetSize() const { return m_size; }
        void SetRect(const Rectf& rect);
        Rectf GetRect() const;
		virtual void Draw( Graphics& g ) const;
	protected:
		Vector2f m_size;
	};
	typedef sb::intrusive_ptr<Sprite> SpritePtr;
    
    class ColorizedSprite : public Sprite {
        SB_META_OBJECT
    public:
        virtual void Draw( Graphics& g ) const;
        void SetColor( const Color& color ) { m_color = color; }
        const Color& GetColor() const { return m_color; }
        void SetAlpha( float a ) { m_color.a = a; }
        float GetAlpha() const { return m_color.a; }
    private:
        Color   m_color;
    };
    typedef sb::intrusive_ptr<ColorizedSprite> ColorizedSpritePtr;
    
    class SpriteBox : public SceneObjectWithPosition {
        SB_META_OBJECT
    public:
        void SetImage(const ImageBoxPtr& img) { m_image = img; }
        const ImageBoxPtr& GetImage() const { return m_image; }
        virtual void Draw( Graphics& g ) const;
        void SetSize(const Vector2f& size) { m_size = size; }
        const Vector2f& GetSize() const { return m_size; }
    protected:
        ImageBoxPtr m_image;
        Vector2f    m_size;
    };
    
    class ColorizedSpriteBox : public SpriteBox {
        SB_META_OBJECT
    public:
        virtual void Draw( Graphics& g ) const;
        void SetColor( const Color& color ) { m_color = color; }
        const Color& GetColor() const { return m_color; }
        void SetAlpha( float a ) { m_color.a = a; }
        float GetAlpha() const { return m_color.a; }
    private:
        Color   m_color;
    };
    
    class SpriteWithMask : public Sprite {
        SB_META_OBJECT
    public:
        virtual void Draw( Graphics& g ) const;
        const ImagePtr& GetMask() const { return m_mask; }
        void SetMask(const ImagePtr& m) { m_mask = m; }
    private:
        ImagePtr    m_mask;
    };
    
}
#endif /*SB_SPRITE_H*/
