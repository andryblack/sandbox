

#include "sb_mygui_ft_font.h"
#include <algorithm>

#include "MyGUI_DataManager.h"
#include "MyGUI_DataStreamHolder.h"
#include "MyGUI_RenderManager.h"
#include "MyGUI_Bitwise.h"
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
    
    namespace
    {
        
        template<typename T>
        void setMax(T& _var, const T& _newValue)
        {
            if (_var < _newValue)
                _var = _newValue;
        }
        
        std::pair<const MyGUI::Char, const uint8> charMaskData[] =
        {
            std::make_pair(MyGUI::FontCodeType::Selected, (const uint8)'\x88'),
            std::make_pair(MyGUI::FontCodeType::SelectedBack, (const uint8)'\x60'),
            std::make_pair(MyGUI::FontCodeType::Cursor, (const uint8)'\xFF'),
            std::make_pair(MyGUI::FontCodeType::Tab, (const uint8)'\x00')
        };
        
        const std::map<const MyGUI::Char, const uint8> charMask(charMaskData, charMaskData + sizeof charMaskData / sizeof(*charMaskData));
        
        const uint8 charMaskBlack = (const uint8)'\x00';
        const uint8 charMaskWhite = (const uint8)'\xFF';
        
        
        struct PixelBase
        {
            static size_t getNumBytes()
            {
                return 4;
            }
            
            static MyGUI::PixelFormat::Enum getFormat()
            {
                return MyGUI::PixelFormat::R8G8B8A8;
            }
            
            static void set(uint8*& _dest, uint8 _luminance, uint8 _alpha)
            {
#ifdef MYGUI_USE_PREMULTIPLIED_ALPHA
                _luminance = (MyGUI::uint16(_luminance) * _alpha) >> 8;
#endif
                *_dest++ = _luminance;
                *_dest++ = _luminance;
                *_dest++ = _luminance;
                *_dest++ = _alpha;
            }
            
            static void set(uint8*& _dest, const MyGUI::Colour& _luminance, uint8 _alpha)
            {
#ifdef MYGUI_USE_PREMULTIPLIED_ALPHA
                *_dest++ = _luminance.red * _alpha ;
                *_dest++ = _luminance.green * _alpha ;
                *_dest++ = _luminance.blue * _alpha;
                *_dest++ = _alpha;
#else
                *_dest++ = _luminance.red * 255;
                *_dest++ = _luminance.green * 255;
                *_dest++ = _luminance.blue * 255;
                *_dest++ = _alpha;
#endif
            }
            
            static void blend_c(uint8& _dest, float _src, uint8 _a) {
#ifdef MYGUI_USE_PREMULTIPLIED_ALPHA
                _dest = (MyGUI::uint16(_dest) * (255-_a))/255 + (_src * _a);
#endif
            }
            
            static void blend(uint8*& _dest, const MyGUI::Colour& _clr, uint8 _alpha)
            {
                blend_c(*_dest++ , _clr.red , _alpha);
                blend_c(*_dest++ , _clr.green , _alpha);
                blend_c(*_dest++ , _clr.blue , _alpha);
                blend_c(*_dest++ , _alpha / 255.0f , _alpha);
                
            }
        };
        
        
    }
    
    const int ResourceTrueTypeFont::mDefaultGlyphSpacing = 1;
    const float ResourceTrueTypeFont::mDefaultTabWidth = 8.0f;
    const float ResourceTrueTypeFont::mSelectedWidth = 1.0f;
    const float ResourceTrueTypeFont::mCursorWidth = 2.0f;
    
    ResourceTrueTypeFont::ResourceTrueTypeFont() :
    mSize(0),
    mScale(1.0f),
    mResolution(96),
    mHinting(HintingUseNative),
    mSpaceWidth(0.0f),
    mGlyphSpacing(-1),
    mTabWidth(0.0f),
    mSubstituteCodePoint(static_cast<MyGUI::Char>(MyGUI::FontCodeType::NotDefined)),
    mDefaultHeight(0),
    mSubstituteGlyphInfo(nullptr)
    {
    }
    
    ResourceTrueTypeFont::~ResourceTrueTypeFont()
    {
        for (sb::vector<MyGUI::ITexture*>::iterator it = mTextures.begin();it!=mTextures.end();++it) {
            MyGUI::RenderManager::getInstance().destroyTexture(*it);
        }
    }
        
        void ResourceTrueTypeFont::parseProperty(const std::string& key,const std::string& value) {
            if (key == "Source")
                setSource(value);
            else if (key == "Size")
                setSize(MyGUI::utility::parseFloat(value));
            else if (key == "Scale")
                setScale(MyGUI::utility::parseFloat(value));
            else if (key == "Resolution")
                setResolution(MyGUI::utility::parseUInt(value));
            else if (key == "TabWidth")
                setTabWidth(MyGUI::utility::parseFloat(value));
            else if (key == "SubstituteCode")
                setSubstituteCode(MyGUI::utility::parseInt(value));
            else if (key == "Distance")
                setDistance(MyGUI::utility::parseInt(value));
            else if (key == "Hinting")
                setHinting(value);
        }
        
        void ResourceTrueTypeFont::setCharImage(MyGUI::Char char_code,
                                                     const sb::string& texture_name,
                                                     const MyGUI::IntCoord& uv,
                                                     const MyGUI::IntPoint& bearing,
                                                     float advance) {
            MyGUI::ITexture* texture = MyGUI::RenderManager::getInstance().getTexture(texture_name);
            if (!texture) {
                texture = MyGUI::RenderManager::getInstance().createTexture(texture_name);
                if (texture) {
                    texture->loadFromFile(texture_name);
                    mTextures.push_back(texture);
                }
            }
            if (texture) {
                MyGUI::GlyphInfo gi;
                gi.texture = texture;
                gi.advance = advance;
                gi.bearingX = bearing.left;
                gi.bearingY = bearing.top;
                gi.width = uv.width;
                gi.height = uv.height;
                MyGUI::IntSize tsize(texture->getWidth(),texture->getHeight());
                gi.uvRect.left = float(uv.left) / tsize.width;
                gi.uvRect.top = float(uv.top) / tsize.height;
                gi.uvRect.right = float(uv.right()) / tsize.width;
                gi.uvRect.bottom = float(uv.bottom()) / tsize.height;
                gi.codePoint = char_code;
                ++mMaxCharIndex;
                mCharMap[char_code] = mMaxCharIndex;
                mGlyphMap[mMaxCharIndex] = gi;
            }
        }
        
        void ResourceTrueTypeFont::setCharImage(MyGUI::Char char_code,
                          const Sandbox::ImagePtr& image,
                          float advance) {
            if (!image) return;
            Sandbox::TexturePtr texture = image->GetTexture();
            if (!texture) return;
            MyGUI::IntCoord coord(image->GetTextureX(),image->GetTextureY(),image->GetTextureW(),image->GetTextureH());
            MyGUI::IntPoint bearing(image->GetHotspot().x,image->GetHotspot().y);
            setCharImage(char_code, texture->GetName(), coord, bearing, advance);
        }
    
    void ResourceTrueTypeFont::deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version)
    {
        Base::deserialization(_node, _version);
        
        MyGUI::xml::ElementEnumerator node = _node->getElementEnumerator();
        while (node.next())
        {
            if (node->getName() == "Property")
            {
                const std::string& key = node->findAttribute("key");
                const std::string& value = node->findAttribute("value");
                parseProperty(key,value);
                
            }
            else if (node->getName() == "Codes")
            {
                // Range of inclusions.
                MyGUI::xml::ElementEnumerator range = node->getElementEnumerator();
                while (range.next("Code"))
                {
                    std::string range_value;
                    if (range->findAttribute("range", range_value))
                    {
                        std::vector<std::string> parse_range = MyGUI::utility::split(range_value);
                        if (!parse_range.empty())
                        {
                            MyGUI::Char first = MyGUI::utility::parseUInt(parse_range[0]);
                            MyGUI::Char last = parse_range.size() > 1 ? MyGUI::utility::parseUInt(parse_range[1]) : first;
                            addCodePointRange(first, last);
                        }
                    }
                }
                
                // If no code points have been included, include the Unicode Basic Multilingual Plane by default before processing
                //	any exclusions.
                if (mCharMap.empty())
                    addCodePointRange(0, 0xFFFF);
                
                // Range of exclusions.
                range = node->getElementEnumerator();
                while (range.next("Code"))
                {
                    std::string range_value;
                    if (range->findAttribute("hide", range_value))
                    {
                        std::vector<std::string> parse_range = MyGUI::utility::split(range_value);
                        if (!parse_range.empty())
                        {
                            MyGUI::Char first = MyGUI::utility::parseUInt(parse_range[0]);
                            MyGUI::Char last = parse_range.size() > 1 ? MyGUI::utility::parseUInt(parse_range[1]) : first;
                            removeCodePointRange(first, last);
                        }
                    }
                }
            }
        }
        
        initialise();
    }
    
    MyGUI::GlyphInfo* ResourceTrueTypeFont::getGlyphInfo(int pass,MyGUI::Char _id)
    {
        CharMap::const_iterator charIter = mCharMap.find(_id);
        
        if (charIter != mCharMap.end())
        {
            GlyphMap::iterator glyphIter = mGlyphMap.find(charIter->second);
            
            if (glyphIter != mGlyphMap.end())
                return &glyphIter->second;
        }
        
        return mSubstituteGlyphInfo;
    }
    
    int ResourceTrueTypeFont::getDefaultHeight()
    {
        return mDefaultHeight;
    }
            
    void ResourceTrueTypeFont::addCodePoint(MyGUI::Char _codePoint)
    {
        mCharMap.insert(CharMap::value_type(_codePoint, 0));
    }
    
    void ResourceTrueTypeFont::removeCodePoint(MyGUI::Char _codePoint)
    {
        mCharMap.erase(_codePoint);
    }
    
    void ResourceTrueTypeFont::addCodePointRange(MyGUI::Char _first, MyGUI::Char _second)
    {
        CharMap::iterator positionHint = mCharMap.lower_bound(_first);
        
        if (positionHint != mCharMap.begin())
            --positionHint;
        
        for (MyGUI::Char i = _first; i <= _second; ++i)
            positionHint = mCharMap.insert(positionHint, CharMap::value_type(i, 0));
    }
    
    void ResourceTrueTypeFont::removeCodePointRange(MyGUI::Char _first, MyGUI::Char _second)
    {
        mCharMap.erase(mCharMap.lower_bound(_first), mCharMap.upper_bound(_second));
    }
    
    void ResourceTrueTypeFont::clearCodePoints()
    {
        mCharMap.clear();
    }
    
    void ResourceTrueTypeFont::initialise()
    {
        if (mGlyphSpacing == -1)
            mGlyphSpacing = mDefaultGlyphSpacing;
        
        mScale = MyGUI::RenderManager::getInstance().getDisplayScale();
        
        
        initialiseFreeType();
    }
    
    void ResourceTrueTypeFont::initialiseFreeType()
    {
        //-------------------------------------------------------------------//
        // Initialise FreeType and load the font.
        //-------------------------------------------------------------------//
        
        FT_Library ftLibrary;
        
        if (FT_Init_FreeType(&ftLibrary) != 0)
            MYGUI_EXCEPT("ResourceTrueTypeFont: Could not init the FreeType library!");
        
        uint8* fontBuffer = nullptr;
        
        FT_Face ftFace = loadFace(ftLibrary, fontBuffer);
        
        if (ftFace == nullptr)
        {
            MYGUI_LOG(Error, "ResourceTrueTypeFont: Could not load the font '" << getResourceName() << "'!");
            return;
        }
        
        //-------------------------------------------------------------------//
        // Calculate the font metrics.
        //-------------------------------------------------------------------//
        
        // The font's overall ascent and descent are defined in three different places in a TrueType font, and with different
        // values in each place. The most reliable source for these metrics is usually the "usWinAscent" and "usWinDescent" pair of
        // values in the OS/2 header; however, some fonts contain inaccurate data there. To be safe, we use the highest of the set
        // of values contained in the face metrics and the two sets of values contained in the OS/2 header.
        int fontAscent = ftFace->size->metrics.ascender >> 6;
        int fontDescent = -ftFace->size->metrics.descender >> 6;
        
        TT_OS2* os2 = (TT_OS2*)FT_Get_Sfnt_Table(ftFace, ft_sfnt_os2);
        
        if (os2 != nullptr)
        {
            setMax(fontAscent, os2->usWinAscent * ftFace->size->metrics.y_ppem / ftFace->units_per_EM);
            setMax(fontDescent, os2->usWinDescent * ftFace->size->metrics.y_ppem / ftFace->units_per_EM);
            
            setMax(fontAscent, os2->sTypoAscender * ftFace->size->metrics.y_ppem / ftFace->units_per_EM);
            setMax(fontDescent, -os2->sTypoDescender * ftFace->size->metrics.y_ppem / ftFace->units_per_EM);
        }
        
        // The nominal font height is calculated as the sum of its ascent and descent as specified by the font designer. Previously
        // it was defined by MyGUI in terms of the maximum ascent and descent of the glyphs currently in use, but this caused the
        // font's line spacing to change whenever glyphs were added to or removed from the font definition. Doing it this way
        // instead prevents a lot of layout problems, and it is also more typographically correct and more aesthetically pleasing.
        mDefaultHeight = fontAscent + fontDescent;
        
        // Set the load flags based on the specified type of hinting.
        FT_Int32 ftLoadFlags;
        
        switch (mHinting)
        {
            case HintingForceAuto:
                ftLoadFlags = FT_LOAD_FORCE_AUTOHINT;
                break;
            case HintingDisableAuto:
                ftLoadFlags = FT_LOAD_NO_AUTOHINT;
                break;
            case HintingDisableAll:
                // When hinting is completely disabled, glyphs must always be rendered -- even during layout calculations -- due to
                // discrepancies between the glyph metrics and the actual rendered bitmap metrics.
                ftLoadFlags = FT_LOAD_NO_HINTING | FT_LOAD_RENDER;
                break;
                //case HintingUseNative:
            default:
                ftLoadFlags = FT_LOAD_DEFAULT;
                break;
        }
        
        //-------------------------------------------------------------------//
        // Create the glyphs and calculate their metrics.
        //-------------------------------------------------------------------//
        
        GlyphHeightMap glyphHeightMap;
        int texWidth = 0;
        
        // Before creating the glyphs, add the "Space" code point to force that glyph to be created. For historical reasons, MyGUI
        // users are accustomed to omitting this code point in their font definitions.
        addCodePoint(MyGUI::FontCodeType::Space);
        
        // Create the standard glyphs.
        for (CharMap::iterator iter = mCharMap.begin(); iter != mCharMap.end(); )
        {
            const MyGUI::Char& codePoint = iter->first;
            FT_UInt glyphIndex = FT_Get_Char_Index(ftFace, codePoint);
            if (glyphIndex) {
                texWidth += createFaceGlyph(ftLibrary,glyphIndex, codePoint, fontAscent, ftFace, ftLoadFlags, glyphHeightMap);
            }
            // If the newly created glyph is the "Not Defined" glyph, it means that the code point is not supported by the font.
            // Remove it from the character map so that we can provide our own substitute instead of letting FreeType do it.
            if (iter->second != 0) {
                ++iter;
            } else
                mCharMap.erase(iter++);
        }
        
        
        // Do some special handling for the "Space" and "Tab" glyphs.
        MyGUI::GlyphInfo* spaceGlyphInfo = getGlyphInfo(-1,MyGUI::FontCodeType::Space);
        
        if (spaceGlyphInfo != nullptr && spaceGlyphInfo->codePoint == MyGUI::FontCodeType::Space)
        {
            FT_UInt glyphIndex = FT_Get_Char_Index(ftFace, spaceGlyphInfo->codePoint);
            if (glyphIndex && (!getGlyphInfo(-1,MyGUI::FontCodeType::NBSP) ||
                               getGlyphInfo(-1,MyGUI::FontCodeType::NBSP) == mSubstituteGlyphInfo)) {
                mCharMap[MyGUI::FontCodeType::NBSP] = glyphIndex;
            }
            // Adjust the width of the "Space" glyph if it has been customized.
            if (mSpaceWidth != 0.0f)
            {
                texWidth += (int)ceil(mSpaceWidth) - (int)ceil(spaceGlyphInfo->width);
                spaceGlyphInfo->width = mSpaceWidth;
                spaceGlyphInfo->advance = mSpaceWidth;
            }
            
            // If the width of the "Tab" glyph hasn't been customized, make it eight spaces wide.
            if (mTabWidth == 0.0f)
                mTabWidth = mDefaultTabWidth * spaceGlyphInfo->advance;
        }
        
        
        // Create the special glyphs. They must be created after the standard glyphs so that they take precedence in case of a
        // collision. To make sure that the indices of the special glyphs don't collide with any glyph indices in the font, we must
        // use glyph indices higher than the highest glyph index in the font.
        FT_UInt nextGlyphIndex = (FT_UInt)ftFace->num_glyphs;
        
        float height = (float)mDefaultHeight;
        
        texWidth += createGlyph(nextGlyphIndex++, MyGUI::GlyphInfo(static_cast<MyGUI::Char>(MyGUI::FontCodeType::Tab), 0.0f, 0.0f, mTabWidth, 0.0f, 0.0f), glyphHeightMap);
        texWidth += createGlyph(nextGlyphIndex++, MyGUI::GlyphInfo(static_cast<MyGUI::Char>(MyGUI::FontCodeType::Selected), mSelectedWidth, height, 0.0f, 0.0f, 0.0f), glyphHeightMap);
        texWidth += createGlyph(nextGlyphIndex++, MyGUI::GlyphInfo(static_cast<MyGUI::Char>(MyGUI::FontCodeType::SelectedBack), mSelectedWidth, height, 0.0f, 0.0f, 0.0f), glyphHeightMap);
        texWidth += createGlyph(nextGlyphIndex++, MyGUI::GlyphInfo(static_cast<MyGUI::Char>(MyGUI::FontCodeType::Cursor), mCursorWidth, height, 0.0f, 0.0f, 0.0f), glyphHeightMap);
        
        // If a substitute code point has been specified, check to make sure that it exists in the character map. If it doesn't,
        // revert to the default "Not Defined" code point. This is not a real code point but rather an invalid Unicode value that
        // is guaranteed to cause the "Not Defined" special glyph to be created.
        if (mSubstituteCodePoint != MyGUI::FontCodeType::NotDefined && mCharMap.find(mSubstituteCodePoint) == mCharMap.end())
            mSubstituteCodePoint = static_cast<MyGUI::Char>(MyGUI::FontCodeType::NotDefined);
        
        // Create the "Not Defined" code point (and its corresponding glyph) if it's in use as the substitute code point.
        if (mSubstituteCodePoint == MyGUI::FontCodeType::NotDefined)
            texWidth += createFaceGlyph(ftLibrary, 0, mSubstituteCodePoint, fontAscent, ftFace, ftLoadFlags, glyphHeightMap);
        
        // Cache a pointer to the substitute glyph info for fast lookup.
        mSubstituteGlyphInfo = &mGlyphMap.find(mCharMap.find(mSubstituteCodePoint)->second)->second;
        
        // Calculate the average height of all of the glyphs that are in use. This value will be used for estimating how large the
        // texture needs to be.
        double averageGlyphHeight = 0.0;
        
        for (GlyphHeightMap::const_iterator j = glyphHeightMap.begin(); j != glyphHeightMap.end(); ++j)
            averageGlyphHeight += j->first * j->second.size();
        
        averageGlyphHeight /= mGlyphMap.size();
        
        //-------------------------------------------------------------------//
        // Calculate the final texture size.
        //-------------------------------------------------------------------//
        
        // Round the current texture width and height up to the nearest powers of two.
        texWidth = MyGUI::Bitwise::firstPO2From(texWidth);
        int texHeight = MyGUI::Bitwise::firstPO2From((int)ceil(averageGlyphHeight) + mGlyphSpacing);
        
        // At this point, the texture is only one glyph high and is probably very wide. For efficiency reasons, we need to make the
        // texture as square as possible. If the texture cannot be made perfectly square, make it taller than it is wide, because
        // the height may decrease in the final layout due to height packing.
        while (texWidth > texHeight)
        {
            texWidth /= 2;
            texHeight *= 2;
        }
        
        // Calculate the final layout of all of the glyphs in the texture, packing them tightly by first arranging them by height.
        // We assume that the texture width is fixed but that the texture height can be adjusted up or down depending on how much
        // space is actually needed.
        // In most cases, the final texture will end up square or almost square. In some rare cases, however, we can end up with a
        // texture that's more than twice as high as it is wide; when this happens, we double the width and try again.
        do
        {
            if (texHeight > texWidth * 2)
                texWidth *= 2;
            
            int texX = 0, texY = 0;
            
            for (GlyphHeightMap::const_iterator j = glyphHeightMap.begin(); j != glyphHeightMap.end(); ++j)
            {
                for (GlyphHeightMap::mapped_type::const_iterator i = j->second.begin(); i != j->second.end(); ++i)
                {
                    MyGUI::GlyphInfo& info = *i->second;
                    
                    int glyphWidth = (int)ceil(info.width);
                    int glyphHeight = (int)ceil(info.height);
                    
                    autoWrapGlyphPos(glyphWidth, texWidth, glyphHeight, texX, texY);
                    
                    if (glyphWidth > 0)
                        texX += mGlyphSpacing + glyphWidth;
                }
            }
            
            texHeight = MyGUI::Bitwise::firstPO2From(texY + glyphHeightMap.rbegin()->first);
        }
        while (texHeight > texWidth * 2);
        
        //-------------------------------------------------------------------//
        // Create the texture and render the glyphs onto it.
        //-------------------------------------------------------------------//
        
        MyGUI::ITexture* texture =  MyGUI::RenderManager::getInstance().createTexture(MyGUI::utility::toString((size_t)this, "_TrueTypeFont"));
        mTextures.push_back(texture);
        
        texture->createManual(texWidth, texHeight, MyGUI::TextureUsage::Static | MyGUI::TextureUsage::Write, PixelBase::getFormat());
        
        uint8* texBuffer = static_cast<uint8*>(texture->lock(MyGUI::TextureUsage::Write));
        
        if (texBuffer != nullptr)
        {
            // Make the texture background transparent white.
            ::memset(texBuffer, 0, texWidth*texHeight*PixelBase::getNumBytes());
            
            renderGlyphs(glyphHeightMap, ftLibrary, ftFace, ftLoadFlags, texBuffer, texWidth, texHeight);
            
            texture->unlock();
            
            MYGUI_LOG(Info, "ResourceTrueTypeFont: Font '" << getResourceName() << "' using texture size " << texWidth << " x " << texHeight << ".");
            MYGUI_LOG(Info, "ResourceTrueTypeFont: Font '" << getResourceName() << "' using real height " << mDefaultHeight << " pixels.");
        }
        else
        {
            MYGUI_LOG(Error, "ResourceTrueTypeFont: Error locking texture; pointer is nullptr.");
        }
        
        for (std::map<FT_UInt, FT_Bitmap>::iterator it = m_bitmaps_map.begin();it!=m_bitmaps_map.end();++it) {
            FT_Bitmap_Done(ftLibrary, &it->second);
        }
        
        FT_Done_Face(ftFace);
        FT_Done_FreeType(ftLibrary);
        
        if (mScale != 1.0f) {
            float iscale = 1.0f / mScale;
            for (GlyphMap::iterator iter = mGlyphMap.begin(); iter != mGlyphMap.end(); ++iter)
            {
                iter->second.width *= iscale;
                iter->second.height *= iscale;
                iter->second.advance *= iscale;
                iter->second.bearingX *= iscale;
                iter->second.bearingY *= iscale;
                iter->second.texture = texture;
            }
            mSpaceWidth *= iscale;
            mGlyphSpacing *= iscale;
            mDefaultHeight *= iscale;
        } else {
            for (GlyphMap::iterator iter = mGlyphMap.begin(); iter != mGlyphMap.end(); ++iter)
            {
                iter->second.texture = texture;
            }
        }
        
        delete [] fontBuffer;
    }
    
    FT_Face ResourceTrueTypeFont::loadFace(const FT_Library& _ftLibrary, uint8*& _fontBuffer)
    {
        FT_Face result = nullptr;
        
        // Load the font file.
        MyGUI::IDataStream* datastream = MyGUI::DataManager::getInstance().getData(mSource);
        
        if (datastream == nullptr)
            return result;
        
        size_t fontBufferSize = datastream->size();
        _fontBuffer = new uint8[fontBufferSize];
        datastream->read(_fontBuffer, fontBufferSize);
        
        MyGUI::DataManager::getInstance().freeData(datastream);
        datastream = nullptr;
        
        // Determine how many faces the font contains.
        if (FT_New_Memory_Face(_ftLibrary, _fontBuffer, (FT_Long)fontBufferSize, -1, &result) != 0)
            MYGUI_EXCEPT("ResourceTrueTypeFont: Could not load the font '" << getResourceName() << "'!");
        
        FT_Long faceIndex = 0;
        
        // Load the first face.
        if (FT_New_Memory_Face(_ftLibrary, _fontBuffer, (FT_Long)fontBufferSize, faceIndex, &result) != 0)
            MYGUI_EXCEPT("ResourceTrueTypeFont: Could not load the font '" << getResourceName() << "'!");
        
        if (result->face_flags & FT_FACE_FLAG_SCALABLE)
        {
            // The font is scalable, so set the font size by first converting the requested size to FreeType's 26.6 fixed-point
            // format.
            FT_F26Dot6 ftSize = (FT_F26Dot6)((mSize*mScale) * (1 << 6));
            
            if (FT_Set_Char_Size(result, ftSize, 0, mResolution, mResolution) != 0)
                MYGUI_EXCEPT("ResourceTrueTypeFont: Could not set the font size for '" << getResourceName() << "'!");
            
            // If no code points have been specified, use the Unicode Basic Multilingual Plane by default.
            if (mCharMap.empty())
                addCodePointRange(0, 0xFFFF);
        }
        else
        {
            MYGUI_EXCEPT("ResourceTrueTypeFont: Could not load the font '" << getResourceName() << "'!");
        }
        
        return result;
    }
    
    void ResourceTrueTypeFont::autoWrapGlyphPos(int _glyphWidth, int _texWidth, int _lineHeight, int& _texX, int& _texY)
    {
        if (_glyphWidth > 0 && _texX + mGlyphSpacing + _glyphWidth > _texWidth)
        {
            _texX = 0;
            _texY += mGlyphSpacing + _lineHeight;
        }
    }
    
    MyGUI::GlyphInfo ResourceTrueTypeFont::createFaceGlyphInfo(MyGUI::Char _codePoint, int _fontAscent, FT_GlyphSlot _glyph)
    {
        float bearingX = _glyph->metrics.horiBearingX / 64.0f;
        
        // The following calculations aren't currently needed but are kept here for future use.
        // float ascent = _glyph->metrics.horiBearingY / 64.0f;
        // float descent = (_glyph->metrics.height / 64.0f) - ascent;
        
        return MyGUI::GlyphInfo(
                         _codePoint,
                         (float)_glyph->bitmap.width,
                         (float)_glyph->bitmap.rows,
                         (_glyph->advance.x / 64.0f),
                         bearingX ,
                         floor(_fontAscent - (_glyph->metrics.horiBearingY / 64.0f)));
    }
    
    int ResourceTrueTypeFont::createGlyph(FT_UInt _glyphIndex, const MyGUI::GlyphInfo& _glyphInfo, GlyphHeightMap& _glyphHeightMap)
    {
        int width = (int)ceil(_glyphInfo.width);
        int height = (int)ceil(_glyphInfo.height);
        
        mCharMap[_glyphInfo.codePoint] = _glyphIndex;
        mMaxCharIndex = std::max(mMaxCharIndex, _glyphIndex);
        MyGUI::GlyphInfo& info = mGlyphMap.insert(GlyphMap::value_type(_glyphIndex, _glyphInfo)).first->second;
        _glyphHeightMap[(FT_Pos)height].insert(std::make_pair(_glyphIndex, &info));
        
        return (width > 0) ? mGlyphSpacing + width : 0;
    }
    
    int ResourceTrueTypeFont::createFaceGlyph(const FT_Library& _ftLibrary,FT_UInt _glyphIndex, MyGUI::Char _codePoint, int _fontAscent, const FT_Face& _ftFace, FT_Int32 _ftLoadFlags, GlyphHeightMap& _glyphHeightMap)
    {
        if (mGlyphMap.find(_glyphIndex) == mGlyphMap.end())
        {
            
            if (FT_Load_Glyph(_ftFace, _glyphIndex, (_ftLoadFlags & (~FT_LOAD_RENDER)) | FT_LOAD_NO_BITMAP) == 0) {
                // Set up a stroker.
                int res = 0;
                
                FT_Glyph glyph;
                if (FT_Get_Glyph(_ftFace->glyph, &glyph) == 0)
                {
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
                    
                } else {
                     MYGUI_LOG(Warning, "ResourceTrueTypeFontOutline: Cannot get glyph " << _glyphIndex << " for character " << _codePoint << " in font '" << getResourceName() << "'.");
                }
                
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
    
    void ResourceTrueTypeFont::renderGlyphs(const GlyphHeightMap& _glyphHeightMap, const FT_Library& _ftLibrary, const FT_Face& _ftFace, FT_Int32 _ftLoadFlags, uint8* _texBuffer, int _texWidth, int _texHeight)
    {
        FT_Bitmap ftBitmap;
        FT_Bitmap_New(&ftBitmap);
        
        int texX = 0, texY = 0;
        
        for (GlyphHeightMap::const_iterator j = _glyphHeightMap.begin(); j != _glyphHeightMap.end(); ++j)
        {
            for (GlyphHeightMap::mapped_type::const_iterator i = j->second.begin(); i != j->second.end(); ++i)
            {
                MyGUI::GlyphInfo& info = *i->second;
                
                switch (info.codePoint)
                {
                    case MyGUI::FontCodeType::Selected:
                    case MyGUI::FontCodeType::SelectedBack:
                    {
                        fillGlyph(info, charMaskWhite, charMask.find(info.codePoint)->second, j->first, _texBuffer, _texWidth, _texHeight, texX, texY);
                        
                        // Manually adjust the glyph's width to zero. This prevents artifacts from appearing at the seams when
                        // rendering multi-character selections.
                        MyGUI::GlyphInfo* glyphInfo = getGlyphInfo(0,info.codePoint);
                        glyphInfo->width = 0.0f;
                        glyphInfo->uvRect.right = glyphInfo->uvRect.left;
                    }
                        break;
                        
                    case MyGUI::FontCodeType::Cursor:
                    case MyGUI::FontCodeType::Tab:
                        fillGlyph(info, charMaskWhite, charMask.find(info.codePoint)->second, j->first, _texBuffer, _texWidth, _texHeight, texX, texY);
                        break;
                        
                    default:
                    {
                        renderGlyph(_ftLibrary,info,j->first,_texBuffer,_texWidth,_texHeight,texX,texY,i->first,ftBitmap);
                        
                    }
                        break;
                }
            }
        }
        
        FT_Bitmap_Done(_ftLibrary, &ftBitmap);
    }
    
    void ResourceTrueTypeFont::fillGlyph(MyGUI::GlyphInfo& _info, uint8 _luminance, uint8 _alpha, int _lineHeight, uint8* _texBuffer, int _texWidth, int _texHeight, int& _texX, int& _texY)
    {
        int width = (int)ceil(_info.width);
        int height = (int)ceil(_info.height);
        
        autoWrapGlyphPos(width, _texWidth, _lineHeight, _texX, _texY);
        
        uint8* dest = _texBuffer + (_texY * _texWidth + _texX) * PixelBase::getNumBytes();
        
        // Calculate how much to advance the destination pointer after each row to get to the start of the next row.
        ptrdiff_t destNextRow = (_texWidth - width) * PixelBase::getNumBytes();
        
        
        
        for (int j = height; j > 0; --j)
        {
            for (int i=0;i<width;++i) {
                PixelBase::set(dest, _luminance, _alpha);
            }
            dest += destNextRow;
        }
        
        // Calculate and store the glyph's UV coordinates within the texture.
        _info.uvRect.left = (float)_texX / _texWidth; // u1
        _info.uvRect.top = (float)_texY / _texHeight; // v1
        _info.uvRect.right = (float)(_texX + width) / _texWidth; // u2
        _info.uvRect.bottom = (float)(_texY + height) / _texHeight; // v2
        
        if (width > 0)
            _texX += mGlyphSpacing + width;
    }
    
    void ResourceTrueTypeFont::putGlyph(MyGUI::GlyphInfo& _info,  int _lineHeight, uint8* _texBuffer, int _texWidth, int _texHeight, int& _texX, int& _texY,const FT_Bitmap* _bitmap,const MyGUI::Colour& _clr) {
        int width = (int)ceil(_info.width);
        int height = (int)ceil(_info.height);
        
        autoWrapGlyphPos(width, _texWidth, _lineHeight, _texX, _texY);
        
        uint8* dest = _texBuffer + (_texY * _texWidth + _texX ) * PixelBase::getNumBytes();
        
        // Calculate how much to advance the destination pointer after each row to get to the start of the next row.
        ptrdiff_t destNextRow = (_texWidth - _bitmap->width) * PixelBase::getNumBytes();
        
        const unsigned char* src = _bitmap->buffer;
        
        for (int j = _bitmap->rows; j > 0; --j)
        {
            for (int i=0;i<_bitmap->width;++i) {
                PixelBase::set(dest, _clr, *src++);
            }
            dest += destNextRow;
        }
        
        // Calculate and store the glyph's UV coordinates within the texture.
        _info.uvRect.left = (float)_texX / _texWidth; // u1
        _info.uvRect.top = (float)_texY / _texHeight; // v1
        _info.uvRect.right = (float)(_texX + _bitmap->width) / _texWidth; // u2
        _info.uvRect.bottom = (float)(_texY + _bitmap->rows) / _texHeight; // v2
        
        if (width > 0)
            _texX += mGlyphSpacing + width;
    }
    
    void ResourceTrueTypeFont::blendGlyph(MyGUI::GlyphInfo& _info, int _dx,int _dy, int _lineHeight, uint8* _texBuffer, int _texWidth, int _texHeight, int& _texX, int& _texY,const FT_Bitmap* _bitmap,const MyGUI::Colour& _clr) {
        int width = (int)ceil(_info.width);
        int height = (int)ceil(_info.height);
        
        autoWrapGlyphPos(width, _texWidth, _lineHeight, _texX, _texY);
        
        uint8* dest = _texBuffer + ((_texY+_dy) * _texWidth + _texX + _dx) * PixelBase::getNumBytes();
        
        // Calculate how much to advance the destination pointer after each row to get to the start of the next row.
        ptrdiff_t destNextRow = (_texWidth - _bitmap->width) * PixelBase::getNumBytes();
        
        const unsigned char* src = _bitmap->buffer;
        
        for (int j = _bitmap->rows; j > 0; --j)
        {
            for (int i=0;i<_bitmap->width;++i) {
                PixelBase::blend(dest, _clr, *src++);
            }
            dest += destNextRow;
        }
    }
    
    void ResourceTrueTypeFont::renderGlyph(const FT_Library& _ftLibrary,MyGUI::GlyphInfo &_info, int _lineHeight, uint8 *_texBuffer, int _texWidth, int _texHeight, int &_texX, int &_texY, FT_UInt _glyphIndex, FT_Bitmap& _ftBitmap) {
        std::map<FT_UInt,FT_Bitmap>::const_iterator it = m_bitmaps_map.find(_glyphIndex);
        if (it != m_bitmaps_map.end()) {
            const FT_Bitmap& bitmap(it->second);
            if (bitmap.buffer != nullptr)
            {
                const FT_Bitmap* bitmap_buffer = 0;
                
                switch (bitmap.pixel_mode)
                {
                    case FT_PIXEL_MODE_GRAY:
                        bitmap_buffer = &bitmap;
                        break;
                        
                    case FT_PIXEL_MODE_MONO:
                        // Convert the monochrome bitmap to 8-bit before rendering it.
                        if (FT_Bitmap_Convert(_ftLibrary, &bitmap, &_ftBitmap, 1) == 0)
                        {
                            // Go through the bitmap and convert all of the nonzero values to 0xFF (white).
                            for (uint8* p = _ftBitmap.buffer, * endP = p + _ftBitmap.width * _ftBitmap.rows; p != endP; ++p)
                                *p ^= -*p ^ *p;
                            
                            bitmap_buffer = &_ftBitmap;
                        }
                        break;
                }
                if (bitmap_buffer) {
                    putGlyph(_info,_lineHeight,_texBuffer,_texWidth,_texHeight,_texX,_texY,bitmap_buffer,MyGUI::Colour::White);
                }
            }
            
        } else {
            MYGUI_LOG(Warning, "ResourceTrueTypeFont: Cannot rendered glyph " << _glyphIndex << " for character " << _info.codePoint << " in font '" << getResourceName() << "'.");
        }
    }
        
    
    void ResourceTrueTypeFont::setSource(const std::string& _value)
    {
        mSource = _value;
    }
    
    void ResourceTrueTypeFont::setSize(float _value)
    {
        mSize = _value;
    }
    
    void ResourceTrueTypeFont::setScale(float _value) {
        mScale = _value;
    }
    
    void ResourceTrueTypeFont::setResolution(MyGUI::uint _value)
    {
        mResolution = _value;
    }
    
    void ResourceTrueTypeFont::setHinting(const std::string& _value)
    {
        if (_value == "use_native")
            mHinting = HintingUseNative;
        else if (_value == "force_auto")
            mHinting = HintingForceAuto;
        else if (_value == "disable_auto")
            mHinting = HintingDisableAuto;
        else if (_value == "disable_all")
            mHinting = HintingDisableAll;
        else
            mHinting = HintingUseNative;
    }
        
    void ResourceTrueTypeFont::setTabWidth(float _value)
    {
        mTabWidth = _value;
    }
        
    void ResourceTrueTypeFont::setSubstituteCode(int _value)
    {
        mSubstituteCodePoint = _value;
    }
    
    void ResourceTrueTypeFont::setDistance(int _value)
    {
        mGlyphSpacing = _value;
    }
            
    }
} // namespace Sandbox

