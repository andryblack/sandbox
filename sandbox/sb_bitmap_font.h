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


#include <sbstd/sb_vector.h>

namespace Sandbox {
    
    
    
    class BitmapFont : public Font {
        SB_META_OBJECT
    public:
        BitmapFont();
        ~BitmapFont();
        
        
		void AddGlypth(const ImagePtr& img,const char* code,float asc);
		void AddKerningPair(const char* from,const char* to,float offset);
        void SetHeight(float h) { set_height(h);set_x_height(h);}
        void SetXHeight(float h) { set_x_height(h);}
        void SetSize(float s) { set_size(s); }
        void SetBaseline(float bl) {set_baseline(bl);}
        
        void FixupChars(const char* from, const char* to);
    private:
        
		
    };
    
}

#endif /* defined(__YinYang__sb_bitmap_font__) */
