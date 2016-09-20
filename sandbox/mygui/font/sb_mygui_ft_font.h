
#ifndef SB_MYGUI_FT_FONT_H_INCLUDED
#define SB_MYGUI_FT_FONT_H_INCLUDED

#include "MyGUI_Prerequest.h"
#include "MyGUI_ITexture.h"
#include "MyGUI_IFont.h"
#include "MyGUI_Colour.h"

#include "sb_image.h"

#	include <ft2build.h>
#	include FT_FREETYPE_H
#   include FT_GLYPH_H

namespace Sandbox
{
    namespace mygui {
        
    
    class ResourceTrueTypeFont :
    public MyGUI::IFont
    {
        MYGUI_RTTI_DERIVED( ResourceTrueTypeFont )
        
    public:
        ResourceTrueTypeFont();
        virtual ~ResourceTrueTypeFont();
        
        virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version);
        
        // Returns the glyph info for the specified code point, or the glyph info for a substitute glyph if the code point does not
        // exist in this font. Returns nullptr if there is a problem with the font.
        virtual MyGUI::GlyphInfo* getGlyphInfo(int pass, MyGUI::Char _id);
        
        // получившаяся высота при генерации в пикселях
        virtual int getDefaultHeight();
        
        // создаение ресурса по текущим значениям
        void initialise();
        
        void setSource(const std::string& _value);
        void setSize(float _value);
        void setScale(float _value);
        void setResolution(MyGUI::uint _value);
        void setHinting(const std::string& _value);
        void setTabWidth(float _value);
       
        void setSubstituteCode(int _value);
        void setDistance(int _value);
        
        void addCodePointRange(MyGUI::Char _first, MyGUI::Char _second);
        void removeCodePointRange(MyGUI::Char _first, MyGUI::Char _second);
        
        void setCharImage(MyGUI::Char char_code,
                          const sb::string& texture,
                          const MyGUI::IntCoord& uv,
                          const MyGUI::IntPoint& bearing,
                          float advance);
        void setCharImage(MyGUI::Char char_code,
                          const Sandbox::ImagePtr& image,
                          float advance);
    protected:
        enum Hinting
        {
            HintingUseNative,
            HintingForceAuto,
            HintingDisableAuto,
            HintingDisableAll
        };
        
        void addCodePoint(MyGUI::Char _codePoint);
        void removeCodePoint(MyGUI::Char _codePoint);
        
        void clearCodePoints();
        
        // The following variables are set directly from values specified by the user.
        std::string mSource; // Source (filename) of the font.
        float mSize; // Size of the font, in points (there are 72 points per inch).
        float mScale; // Scale font rendered to texture for hdpi displays
        MyGUI::uint mResolution; // Resolution of the font, in pixels per inch.
        Hinting mHinting; // What type of hinting to use when rendering the font.
        float mSpaceWidth; // The width of a "Space" character, in pixels. If zero, the default width is used.
        int mGlyphSpacing; // How far apart the glyphs are placed from each other in the font texture, in pixels.
        float mTabWidth; // The width of the "Tab" special character, in pixels.
        MyGUI::Char mSubstituteCodePoint; // The code point to use as a substitute for code points that don't exist in the font.
        
        // The following variables are calculated automatically.
        int mDefaultHeight; // The nominal height of the font in pixels.
        MyGUI::GlyphInfo* mSubstituteGlyphInfo; // The glyph info to use as a substitute for code points that don't exist in the font.
        sb::vector<MyGUI::ITexture*> mTextures; // The texture that contains all of the rendered glyphs in the font.
        
        // The following constants used to be mutable, but they no longer need to be. Do not modify their values!
        static const int mDefaultGlyphSpacing; // How far apart the glyphs are placed from each other in the font texture, in pixels.
        static const float mDefaultTabWidth; // Default "Tab" width, used only when tab width is no specified.
        static const float mSelectedWidth; // The width of the "Selected" and "SelectedBack" special characters, in pixels.
        static const float mCursorWidth; // The width of the "Cursor" special character, in pixels.
        
    protected:
        virtual void parseProperty(const std::string& key,const std::string& value);
        
    protected:
        // A map of code points to glyph indices.
        typedef std::map<MyGUI::Char, FT_UInt> CharMap;
        
        // A map of glyph indices to glyph info objects.
        typedef std::map<FT_UInt, MyGUI::GlyphInfo> GlyphMap;
        
        // A map of glyph heights to the set of paired glyph indices and glyph info objects that are of that height.
        typedef std::map<FT_Pos, std::map<FT_UInt, MyGUI::GlyphInfo*> > GlyphHeightMap;
        
        void initialiseFreeType();
        
        // Loads the font face as specified by mSource, mSize, and mResolution. Automatically adjusts code-point ranges according
        // to the capabilities of the font face.
        // Returns a handle to the FreeType face object for the face, or nullptr if the face could not be loaded.
        // Keeps the font file loaded in memory and stores its location in _fontBuffer. The caller is responsible for freeing this
        // buffer when it is done using the face by calling delete[] on the buffer after calling FT_Done_Face() on the face itself.
        FT_Face loadFace(const FT_Library& _ftLibrary, MyGUI::uint8*& _fontBuffer);
        
        // Wraps the current texture coordinates _texX and _texY to the beginning of the next line if the specified glyph width
        // doesn't fit at the end of the current line. Automatically takes the glyph spacing into account.
        void autoWrapGlyphPos(int _glyphWidth, int _texWidth, int _lineHeight, int& _texX, int& _texY);
        
        // Creates a GlyphInfo object using the specified information.
        MyGUI::GlyphInfo createFaceGlyphInfo(MyGUI::Char _codePoint, int _fontAscent, FT_GlyphSlot _glyph);
        
        // Creates a glyph with the specified glyph index and assigns it to the specified code point.
        // Automatically updates _glyphHeightMap, mCharMap, and mGlyphMap with data from the new glyph..
        int createGlyph(FT_UInt _glyphIndex, const MyGUI::GlyphInfo& _glyphInfo, GlyphHeightMap& _glyphHeightMap);
        
        // Creates a glyph with the specified index from the specified font face and assigns it to the specified code point.
        // Automatically updates _glyphHeightMap with data from the newly created glyph.
        virtual int createFaceGlyph(const FT_Library& _ftLibrary,FT_UInt _glyphIndex, MyGUI::Char _codePoint, int _fontAscent, const FT_Face& _ftFace, FT_Int32 _ftLoadFlags, GlyphHeightMap& _glyphHeightMap);
        
        // Renders all of the glyphs in _glyphHeightMap into the specified texture buffer using data from the specified font face.
        void renderGlyphs(const GlyphHeightMap& _glyphHeightMap, const FT_Library& _ftLibrary, const FT_Face& _ftFace, FT_Int32 _ftLoadFlags, MyGUI::uint8* _texBuffer, int _texWidth, int _texHeight);
        
        // Renders the glyph described by the specified glyph info according to the specified parameters.
        // Supports two types of rendering, depending on the value of UseBuffer: Texture block transfer and rectangular color fill.
        // The _luminance0 value is used for even-numbered columns (from zero), while _luminance1 is used for odd-numbered ones.
        void fillGlyph(MyGUI::GlyphInfo& _info, MyGUI::uint8 _luminance, MyGUI::uint8 _alpha, int _lineHeight, MyGUI::uint8* _texBuffer, int _texWidth, int _texHeight, int& _texX, int& _texY);
        void putGlyph(MyGUI::GlyphInfo& _info, int _lineHeight, MyGUI::uint8* _texBuffer, int _texWidth, int _texHeight, int& _texX, int& _texY,const FT_Bitmap* _bitmap,const MyGUI::Colour& _clr);
        void blendGlyph(MyGUI::GlyphInfo& _info, int _dx,int _dy, int _lineHeight, MyGUI::uint8* _texBuffer, int _texWidth, int _texHeight, int& _texX, int& _texY,const FT_Bitmap* _bitmap,const MyGUI::Colour& _clr);
        virtual void renderGlyph(const FT_Library& _ftLibrary,MyGUI::GlyphInfo& _info, int _lineHeight, MyGUI::uint8* _texBuffer, int _texWidth, int _texHeight, int& _texX, int& _texY, FT_UInt _glyphIndex,FT_Bitmap& _ftBitmap);
        
        
        CharMap mCharMap; // A map of code points to glyph indices.
        GlyphMap mGlyphMap; // A map of glyph indices to glyph info objects.
        FT_UInt mMaxCharIndex;
        
        std::map<FT_UInt,FT_Bitmap> m_bitmaps_map;
    };
        
        
        
    }
} // namespace MyGUI

#endif /* SB_MYGUI_FT_FONT_H_INCLUDED */
