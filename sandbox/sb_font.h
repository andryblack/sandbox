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
#include <sbstd/sb_intrusive_ptr.h>
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
        
        /// font size (font units)
        float   GetSize() const { return m_size; }
        /// font 'x' height
        float   GetXHeight() const { return m_xheight; }
        /// font height (px)
        float   GetHeight() const { return m_height; }
        /// font baseline (px) 
        float   GetBaseline() const { return m_baseline; }
    protected:
        void    set_height(float height) { m_height = height; }
        void    set_size(float size) { m_size = size; }
        void    set_baseline(float baseline) { m_baseline = baseline; }
        void    set_x_height(float h) {m_xheight = h;}
	private:
        float   m_size;
        float   m_height;
        float   m_xheight;
        float   m_baseline;
	};
	
	typedef sb::intrusive_ptr<Font> FontPtr;

}

#endif /*SB_FONT_H*/
