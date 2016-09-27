#ifndef SB_TEXT_DATA_H_INCLUDED
#define SB_TEXT_DATA_H_INCLUDED

#include "sb_vector2.h"
#include "sb_color.h"
#include <sbstd/sb_vector.h>
#include <sbstd/sb_string.h>
#include <ghl_types.h>

namespace Sandbox {
    
    
    struct TextData {
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
        
        void Clear() { data.clear(); }
    };
    
}

#endif /*SB_TEXT_DATA_H_INCLUDED*/
