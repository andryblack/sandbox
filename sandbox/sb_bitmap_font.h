//
//  sb_bitmap_font.h
//  YinYang
//
//  Created by Andrey Kunitsyn on 10/23/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef __YinYang__sb_bitmap_font__
#define __YinYang__sb_bitmap_font__

#include "sb_font.h"
#include "sb_utf.h"
#include "sb_image.h"
#include "sb_vector.h"

namespace Sandbox {
    
    class BitmapFont : public Font {
        SB_META_OBJECT
    public:
        BitmapFont();
        ~BitmapFont();
        
        virtual void Draw(Graphics& g,const Vector2f& pos,const char* text,FontAlign align) const;
		virtual float GetTextWidth(const char* text) const ;

        void Reserve(size_t size);
		void AddGlypth(const ImagePtr& img,const char* code,float asc);
		void AddKerningPair(const char* from,const char* to,float offset);
    private:
        struct Kerning {
			GHL::UInt32 code;
			float	offset;
		};
		struct Glypth {
			GHL::UInt32 code;
			ImagePtr img;
			float	asc;
			std::vector<Kerning> kerning;
		};
		sb::vector<Glypth> m_glypths;
		const Glypth* get_glypth(GHL::UInt32 code) const;
		float getKerning(const Glypth* g,GHL::UInt32 to) const;
    };
    
}

#endif /* defined(__YinYang__sb_bitmap_font__) */
