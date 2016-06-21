
#ifndef SB_MYGUI_FT_FONT_OL_H_INCLUDED
#define SB_MYGUI_FT_FONT_OL_H_INCLUDED

#include "sb_mygui_ft_font.h"


namespace Sandbox
{
    namespace mygui {
        
        
        class ResourceTrueTypeFontOutline :
        public ResourceTrueTypeFont
        {
            MYGUI_RTTI_DERIVED( ResourceTrueTypeFontOutline )
            
        public:
            ResourceTrueTypeFontOutline();
            virtual ~ResourceTrueTypeFontOutline();
            void setOutlineWidth(float _value);
        protected:
            virtual void parseProperty(const std::string& key,const std::string& value);
            virtual int createFaceGlyph(const FT_Library& _ftLibrary,FT_UInt _glyphIndex, MyGUI::Char _codePoint, int _fontAscent, const FT_Face& _ftFace, FT_Int32 _ftLoadFlags, GlyphHeightMap& _glyphHeightMap);
            virtual void renderGlyph(const FT_Library& _ftLibrary,MyGUI::GlyphInfo& _info, int _lineHeight, MyGUI::uint8* _texBuffer, int _texWidth, int _texHeight, int& _texX, int& _texY, FT_UInt _glyphIndex,FT_Bitmap& _ftBitmap);
        private:
            float mOutlineWidth;
            
        private:
            
        };
        
        
        
    }
} // namespace MyGUI

#endif /* SB_MYGUI_FT_FONT_OL_H_INCLUDED */
