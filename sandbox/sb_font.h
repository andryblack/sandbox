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

#include "sb_image.h"
#include <ghl_types.h>
#include "sb_vector2.h"
#include <vector>


namespace Sandbox {

	enum FontAlign {
		ALIGN_LEFT = 0,
		ALIGN_RIGHT = 1,
		ALIGN_CENTER = 3,
	};
	
	class Graphics;
	
	class Font {
	public:
		Font();
		~Font();
		void Reserve(size_t size);
		void AddGlypth(const ImagePtr& img,const char* code,float asc);
		void AddKerningPair(const char* from,const char* to,float offset);
		void Draw(Graphics& g,const Vector2f& pos,const char* text,FontAlign align) const;
		float GetTextWidth(const char* text) const;
	private:
		struct Kerning {
			GHL::UInt16 code;
			float	offset;
		};
		struct Glypth {
			GHL::UInt16 code;
			ImagePtr img;
			float	asc;
			std::vector<Kerning> kerning;
		};
		std::vector<Glypth> m_glypths;
		const Glypth* get_glypth(GHL::UInt16 code) const;
		float getKerning(const Glypth* g,GHL::UInt16 to) const;
	};
	
	typedef shared_ptr<Font> FontPtr;

}

#endif /*SB_FONT_H*/