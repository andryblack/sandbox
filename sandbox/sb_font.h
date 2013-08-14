/*
 *  sb_font.h
 *  SR
 *
 *  Created by Андрей Куницын on 12.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_FONT_H
#define SB_FONT_H

#include "sb_notcopyable.h"
#include <sbstd/sb_shared_ptr.h>
#include "meta/sb_meta.h"


namespace Sandbox {

    struct Vector2f;
    
	enum FontAlign {
		ALIGN_LEFT = 0,
		ALIGN_RIGHT = 1,
		ALIGN_CENTER = 3,
	};
	
	class Graphics;
	
	class Font : public meta::object {
        SB_META_OBJECT
	public:
		Font();
		virtual ~Font();
		virtual float Draw(Graphics& g,const Vector2f& pos,const char* text,FontAlign align) const = 0;
		virtual float GetTextWidth(const char* text) const = 0;
        virtual float GetHeight() const { return m_height; }
    protected:
        void    set_height(float height) { m_height = height; }
	private:
        float   m_height;
	};
	
	typedef sb::shared_ptr<Font> FontPtr;

}

#endif /*SB_FONT_H*/
