#include "sb_text_data.h"
#include "sb_utf.h"
#include "sb_font.h"

namespace Sandbox {
    static const char convert_colour[64] =
    {
        0,  1,  2,  3,  4,  5,  6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
        0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    struct RollBackPoint {
        size_t position;
        const char* text;
        size_t count;
        float width;
        RollBackPoint() : text(0) {}
        void clear() {
            text = 0;
        }
        void set(size_t position,const char* text,size_t count, float width) {
            this->position = position;
            this->text = text;
            this->count = count;
            this->width = width;
        }
        
        bool empty() const {
            return !text;
        }
    };
    
    template<typename T>
    static inline void set_min(T& _var, const T& _newValue)
    {
        if (_newValue < _var)
            _var = _newValue;
    }
    
    template<typename T>
    static inline void set_max(T& _var, const T& _newValue)
    {
        if (_var < _newValue)
            _var = _newValue;
    }
    
    void TextData::Clear() {
        length = 0;
        data.clear();
        size = Sizef(0,0);
    }
    
    void TextData::Fill(const char* text, const FontPtr& font, int max_width, FontAlign align) {
        

        
        RollBackPoint roll_back;
        
        float width = 0.0f;
        size_t count = 0;
        
        Clear();
        
        
        TextData::Line line_info;
        float _height = font->GetHeight();
        
        line_info.offset.y = font->GetHeight() - font->GetBaseline();
        
        size.h += _height;
        
        while (*text)
        {
            UTF32Char character = 0;
            text = get_char(text,character);
            
            // new line
            if (character == SYMB_CR
                || character == SYMB_NEL
                || character == SYMB_LF)
            {
                if (character == SYMB_CR && *text)
                {
                    UTF32Char next_char = 0;
                    const char* next_it = get_char(text, next_char);
                    if (next_char == SYMB_LF)
                        text = next_it; // skip both as one newline
                }
                
                line_info.width = width;
                line_info.count = count;
                length += line_info.count + 1;
                
                size.h += _height;
                
                width = 0;
                count = 0;
                
                data.push_back(line_info);
                line_info.Clear();
                line_info.offset.y += _height;
                
                // cancel rollback
                roll_back.clear();
                
                continue;
            }
            // tag
            else if (character == L'#')
            {
                if (*text == 0)
                    break;
                // next symbol
                text = get_char(text, character);
                
                if (character != L'#')
                {
                    // парсим первый символ
                    GHL::UInt32 colour = convert_colour[(character - 48) & 0x3F];
                    
                    for (char i = 0; i < 5; i++)
                    {
                        if (*text == 0)
                            break;
                        text = get_char(text, character);
                        colour <<= 4;
                        colour += convert_colour[ ((character) - 48) & 0x3F ];
                    }
                    colour = ((colour & 0x00ff0000) >> 16) |
                    ((colour & 0x000000ff) << 16) |
                    ((colour & 0xff00ff00)) | 0xff000000;
                    TextData::SymbolData symbol;
                    symbol.type = TextData::SYMBOL_SET_COLOR;
                    symbol.data.color = colour;
                    line_info.data.push_back(symbol);
                    continue;
                }
            }
            
            const FontData::Glypth* info = font->GetGlyph(character);
            
            if (!info)
                continue;
            
            if (SYMB_SPACE == character || SYMB_TAB == character || SYMB_ZWSP == character)
            {
                roll_back.set(line_info.data.size(), text, count, width);
            }
            
            float char_advance = info->asc;
            
            // перенос слов
            if (max_width > 0
                && (width + char_advance) > max_width
                && !roll_back.empty())
            {
                // откатываем до последнего пробела
                width = roll_back.width;
                count = roll_back.count;
                text = roll_back.text;
                line_info.data.erase(line_info.data.begin() + roll_back.position, line_info.data.end());
                
                // запоминаем место отката, как полную строку
                line_info.width = width;
                line_info.count = count;
                length += line_info.count + 1;
                
                size.h += _height;
                
                width = 0;
                count = 0;
                
                data.push_back(line_info);
                line_info.Clear();
                line_info.offset.y += _height;
                
                // отменяем откат
                roll_back.clear();
                
                continue;
            }
            TextData::SymbolData symbol;
            symbol.type = TextData::SYMBOL_TEXT;
            symbol.data.symbol.code = character;
            symbol.data.symbol.offset = char_advance;
            line_info.data.push_back(symbol);
            width += char_advance;
            count ++;
        }
        
        line_info.width = width;
        line_info.count = count;
        length += line_info.count;
        
        data.push_back(line_info);
        size.w = 0.0f;
        float offset = 0.0f;
        float move_down = 0;
        for (TextData::LinesData::iterator line = data.begin(); line != data.end(); ++line)
        {
            line->offset.x = 0;
            line->offset.y += move_down;
            width = line->width;
            if (!line->data.empty()) {
                for (TextData::SymbolsData::const_iterator simbol = line->data.begin();simbol!=line->data.end();++simbol) {
                    if (simbol->type == TextData::SYMBOL_TEXT) {
                        const FontData::Glypth* info = font->GetGlyph(simbol->data.symbol.code);
                        if (info) {
                            if (info->bearingX() < 0) {
                                float firstOffset = - info->bearingX();
                                set_max(offset, firstOffset);
                                width += firstOffset;
                            }
                        }
                        break;
                    }
                }
                for (TextData::SymbolsData::const_reverse_iterator simbol = line->data.rbegin();simbol!=line->data.rend();++simbol) {
                    if (simbol->type == TextData::SYMBOL_TEXT) {
                        const FontData::Glypth* info = font->GetGlyph(simbol->data.symbol.code);
                        if (info) {
                            float rightOffset = ((info->bearingX() + info->width()) - info->asc);
                            if (rightOffset > 0) {
                                width += rightOffset;
                            }
                        }
                        break;
                    }
                }
                int max_asc = int(font->GetHeight()-font->GetBaseline());
                int max_desc = int(font->GetBaseline());
                for (TextData::SymbolsData::const_iterator simbol = line->data.begin();simbol!=line->data.end();++simbol) {
                    if (simbol->type == TextData::SYMBOL_TEXT) {
                        const FontData::Glypth* info = font->GetGlyph(simbol->data.symbol.code);
                        if (info) {
                            if (-info->bearingY() > max_asc) {
                                max_asc = -int(info->bearingY());
                            }
                            if ((info->height()+info->bearingY()) > max_desc) {
                                max_desc = int(info->height()+info->bearingY());
                            }
                        }
                    }
                }
                int add_asc = max_asc-int(font->GetHeight()-font->GetBaseline());
                int add_desc = max_desc - font->GetBaseline();
                line->offset.y += add_asc;
                move_down += add_asc + add_desc;
                size.h += add_asc + add_desc;
            }
            line->width = width;
            set_max(size.w, float(int(ceil(line->width))));
        }
        
        if (!data.empty()) {
            int addHeight = 0;
            for (TextData::SymbolsData::const_reverse_iterator simbol = data.back().data.rbegin();simbol!=data.back().data.rend();++simbol) {
                if (simbol->type == TextData::SYMBOL_TEXT) {
                    const FontData::Glypth* info = font->GetGlyph(simbol->data.symbol.code);
                    if (info) {
                        int bottomOffset = int(ceil(((info->bearingY() + info->height()) - _height)));
                        if (bottomOffset > 0) {
                            set_max(addHeight, bottomOffset);
                        }
                    }
                    break;
                }
            }
            
            size.h += addHeight;
        }
        
        
        // align lines
        for (TextData::LinesData::iterator line = data.begin(); line != data.end(); ++line)
        {
            line->offset.x = offset;
            if (align == ALIGN_RIGHT)
                line->offset.x += size.w - line->width;
            else if (align == ALIGN_CENTER)
                line->offset.x += (size.w - line->width) / 2;
        }
        
        
    }
}
