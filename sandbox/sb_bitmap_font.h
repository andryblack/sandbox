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

#include "sb_image.h"
#include <sbstd/sb_vector.h>

namespace Sandbox {
    
    class BitmapFont : public Font {
        SB_META_OBJECT
    public:
        BitmapFont();
        ~BitmapFont();
        
        virtual float Draw(Graphics& g,const Vector2f& pos,const char* text,FontAlign align) const;
		virtual float GetTextWidth(const char* text) const ;
        virtual bool MovePosition(Vector2f& pos,UTF32Char prev,UTF32Char next) const;

        void Reserve(size_t size);
		void AddGlypth(const ImagePtr& img,const char* code,float asc);
		void AddKerningPair(const char* from,const char* to,float offset);
        void SetHeight(float h) { set_height(h);set_x_height(h);}
        void SetXHeight(float h) { set_x_height(h);}
        void SetSize(float s) { set_size(s); }
        void SetBaseline(float bl) {set_baseline(bl);}
        
        void FixupChars(const char* from, const char* to);
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
        Glypth* get_glypth_int(GHL::UInt32 code);
		float getKerning(const Glypth* g,GHL::UInt32 to) const;
    };
    
}

#endif /* defined(__YinYang__sb_bitmap_font__) */
