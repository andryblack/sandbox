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
#include "sb_object.h"
#include "sb_color.h"

namespace Sandbox {
	
	class Sprite : public Object {
	public:
		Sprite();
		~Sprite();
		void SetImage(const ImagePtr& img) { m_image = img;}
		const ImagePtr& GetImage() const { return m_image;}
		void SetPos(const Vector2f& pos) { m_pos=pos;}
		const Vector2f& GetPos() const { return m_pos;}
		virtual void Draw( Graphics& g ) const;
	protected:
		ImagePtr m_image;
		Vector2f m_pos;
	};
	typedef shared_ptr<Sprite> SpritePtr;
    
    class ColorizedSprite : public Sprite {
    public:
        virtual void Draw( Graphics& g ) const;
        void SetColor( const Color& color ) { m_color = color; }
        const Color& GetColor() const { return m_color; }
    private:
        Color   m_color;
    };
    
}
#endif /*SB_SPRITE_H*/
