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
#include "sb_shared_ptr.h"
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
		virtual void Draw(Graphics& g,const Vector2f& pos,const char* text,FontAlign align) const = 0;
		virtual float GetTextWidth(const char* text) const = 0;
	private:
	};
	
	typedef sb::shared_ptr<Font> FontPtr;

}

#endif /*SB_FONT_H*/
