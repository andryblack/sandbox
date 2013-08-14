//
//  sb_rocket_font.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/12/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_rocket_font__
#define __sr_osx__sb_rocket_font__

#include <Rocket/Core/FontFaceHandle.h>
#include "sb_font.h"

namespace Sandbox {
    
    class RocketFontFaceHandle : public Rocket::Core::FontFaceHandle {
    private:
        FontPtr m_font;
    public:
        explicit RocketFontFaceHandle( const FontPtr& fnt) : m_font(fnt) {}
        const FontPtr& GetFont() const { return m_font; }
        /// Returns the point size of this font face.
        /// @return The face's point size.
        virtual int GetSize() const;
        
        /// Returns the pixel height of a lower-case x in this font face.
        /// @return The height of a lower-case x.
        virtual int GetXHeight() const;
        /// Returns the default height between this font face's baselines.
        /// @return The default line height.
        virtual int GetLineHeight() const;
        
        /// Returns the font's baseline, as a pixel offset from the bottom of the font.
        /// @return The font's baseline.
        virtual int GetBaseline() const;
        
        /// Returns the width a string will take up if rendered with this handle.
        /// @param[in] string The string to measure.
        /// @param[in] prior_character The optionally-specified character that immediately precedes the string. This may have an impact on the string width due to kerning.
        /// @return The width, in pixels, this string will occupy if rendered with this handle.
        virtual int GetStringWidth(const Rocket::Core::WString& string, Rocket::Core::word prior_character = 0) const;
        
        /// Generates the geometry required to render a line above, below or through a line of text.
        /// @param[out] geometry The geometry to append the newly created geometry into.
        /// @param[in] position The position of the baseline of the lined text.
        /// @param[in] width The width of the string to line.
        /// @param[in] height The height to render the line at.
        /// @param[in] colour The colour to draw the line in.
        virtual void GenerateLine(Rocket::Core::Geometry* geometry, const Rocket::Core::Vector2f& position, int width, Rocket::Core::Font::Line height, const Rocket::Core::Colourb& colour) const;
        
        virtual int Draw(Graphics& g, const Rocket::Core::WString& string, const Rocket::Core::Vector2f& position, const Rocket::Core::Colourb& colour) const;
    };
    
}

#endif /* defined(__sr_osx__sb_rocket_font__) */
