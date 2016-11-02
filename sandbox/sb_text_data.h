#ifndef SB_TEXT_DATA_H_INCLUDED
#define SB_TEXT_DATA_H_INCLUDED

#include "sb_vector2.h"
#include "sb_color.h"
#include <sbstd/sb_vector.h>
#include <sbstd/sb_string.h>
#include <ghl_types.h>
#include "sb_font_data.h"
#include "sb_size.h"
#include "sb_utf.h"

namespace Sandbox {
    
    class Font;
    typedef sb::intrusive_ptr<Font> FontPtr;
    
    struct TextData {
        
        enum {
            SYMB_TAB = 0x0009,
            SYMB_LF = 0x000A,
            SYMB_CR = 0x000D,
            SYMB_SPACE = 0x0020,
            SYMB_NBSP = 0x00A0,
            SYMB_NEL = 0x0085,
        };
        
        enum SymbolType {
            SYMBOL_SET_COLOR,
            SYMBOL_TEXT
        };
    
        struct SymbolData {
            SymbolType type;
            union Data {
                GHL::UInt32 color;
                struct {
                    GHL::UInt32 code;
                    float offset;
                } symbol;
            } data;
        };
        
        typedef sb::vector<SymbolData> SymbolsData;
        
        struct Line {
            size_t count;
            float width;
            Vector2f offset;
            SymbolsData data;
            void Clear() { data.clear(); count = 0; width=0.0f;}
        };
        
        typedef sb::vector<Line> LinesData;
        
        LinesData data;
        Sizef   size;
        size_t  length;
        
        void Clear();
        void Fill(const char* text, const FontPtr& font, int max_width, FontAlign align);
    };
    
}

#endif /*SB_TEXT_DATA_H_INCLUDED*/
