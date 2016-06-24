

#include "sb_mygui_ft_font_ol.h"

#include "MyGUI_DataManager.h"
#include "MyGUI_DataStreamHolder.h"
#include "MyGUI_RenderManager.h"
#include "MyGUI_Bitwise.h"
#include "MyGUI_FactoryManager.h"
#include "MyGUI_ResourceManager.h"

#include "sb_math.h"


#	include FT_GLYPH_H
#	include FT_TRUETYPE_TABLES_H
#	include FT_BITMAP_H
#	include FT_WINFONTS_H
#   include FT_STROKER_H



namespace Sandbox
{
    namespace mygui {
        
        typedef MyGUI::uint8 uint8;
        
        
        ResourceTrueTypeFontOutline::ResourceTrueTypeFontOutline() :
            mOutlineWidth(1.0f)
        {
        }
        
        ResourceTrueTypeFontOutline::~ResourceTrueTypeFontOutline()
        {
            
        }
        
        void ResourceTrueTypeFontOutline::parseProperty(const std::string& key,const std::string& value) {
            if (key == "OutlineWidth")
                setOutlineWidth(MyGUI::utility::parseFloat(value));
            else
                Base::parseProperty(key,value);
        }
        
        
        int ResourceTrueTypeFontOutline::createFaceGlyph(const FT_Library& _ftLibrary,FT_UInt _glyphIndex, MyGUI::Char _codePoint, int _fontAscent, const FT_Face& _ftFace, FT_Int32 _ftLoadFlags, GlyphHeightMap& _glyphHeightMap)
        {
            if (mGlyphMap.find(_glyphIndex) == mGlyphMap.end())
            {
                
                    if (FT_Load_Glyph(_ftFace, _glyphIndex, (_ftLoadFlags & (~FT_LOAD_RENDER)) | FT_LOAD_NO_BITMAP) == 0) {
                        // Set up a stroker.
                        int res = 0;
                        float outlineWidth = mOutlineWidth * mScale;
                        FT_Stroker stroker;
                        FT_Stroker_New(_ftLibrary, &stroker);
                        FT_Stroker_Set(stroker,
                                       (int)(outlineWidth * 64),
                                       FT_STROKER_LINECAP_ROUND,
                                       FT_STROKER_LINEJOIN_ROUND,
                                       0);
                        
                        FT_Glyph glyph;
                        if (FT_Get_Glyph(_ftFace->glyph, &glyph) == 0)
                        {
                            FT_Glyph_StrokeBorder(&glyph, stroker,0, 1);
                            // Render the outline spans to the span list
                            FT_Glyph_To_Bitmap(&glyph,FT_RENDER_MODE_NORMAL,0,1);
                            FT_BitmapGlyph ft_bitmap_glyph = (FT_BitmapGlyph) glyph;
                            
                            
                            float bearingX = ft_bitmap_glyph->left;
                            
                            // The following calculations aren't currently needed but are kept here for future use.
                            // float ascent = _glyph->metrics.horiBearingY / 64.0f;
                            // float descent = (_glyph->metrics.height / 64.0f) - ascent;
                            
                            MyGUI::GlyphInfo info(
                                                  _codePoint,
                                                  ft_bitmap_glyph->bitmap.width,
                                                  ft_bitmap_glyph->bitmap.rows,
                                                  (_ftFace->glyph->advance.x / 64.0f),
                                                  bearingX,
                                                  floor(_fontAscent - ft_bitmap_glyph->top ));
                            
                            res = createGlyph(_glyphIndex, info, _glyphHeightMap);
                            
                            FT_Bitmap new_bitmap;
                            FT_Bitmap_New(&new_bitmap);
                            FT_Bitmap_Copy(_ftLibrary, &ft_bitmap_glyph->bitmap, &new_bitmap);
                            m_bitmaps_map[_glyphIndex] = new_bitmap;

                            FT_Done_Glyph(glyph);
                            
//                            if (FT_Load_Glyph(_ftFace, _glyphIndex, _ftLoadFlags | FT_LOAD_RENDER) == 0) {
//                                FT_Bitmap new_bitmap;
//                                FT_Bitmap_New(&new_bitmap);
//                                FT_Bitmap_Copy(_ftLibrary, &_ftFace->glyph->bitmap, &new_bitmap);
//                                m_bitmaps_map[_glyphIndex] = new_bitmap;
//                                
//                                ft_bitmap_glyph->left -= _ftFace->glyph->bitmap_left;
//                                ft_bitmap_glyph->top -= _ftFace->glyph->bitmap_top;
//                            }
                            
                        }
                        // Clean up afterwards.
                        FT_Stroker_Done(stroker);
                        
                        
                        
                        return res;
                    } else {
                        MYGUI_LOG(Warning, "ResourceTrueTypeFontOutline: Cannot load glyph " << _glyphIndex << " for character " << _codePoint << " in font '" << getResourceName() << "'.");
                    }
                
            }
            else
            {
                mCharMap[_codePoint] = _glyphIndex;
            }
            
            return 0;
        }
    
        void ResourceTrueTypeFontOutline::renderGlyph(const FT_Library& _ftLibrary,MyGUI::GlyphInfo &_info, int _lineHeight, uint8 *_texBuffer, int _texWidth, int _texHeight, int &_texX, int &_texY, FT_UInt _glyphIndex, FT_Bitmap& _ftBitmap) {
            Base::renderGlyph(_ftLibrary,_info,_lineHeight,_texBuffer,_texWidth,_texHeight,_texX,_texY,_glyphIndex,_ftBitmap);
        }
        
    
        void ResourceTrueTypeFontOutline::setOutlineWidth(float _value) {
            mOutlineWidth = _value;
        } 
        
    }
} // namespace Sandbox

