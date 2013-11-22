//
//  sb_rocket_font.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/12/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_rocket_font.h"
#include "sb_graphics.h"

namespace Sandbox {
    
    int RocketFontFaceHandle::GetSize() const {
        return m_font->GetSize();
    }
    
    int RocketFontFaceHandle::GetXHeight() const {
        return m_font->GetXHeight();
    }
    
    int RocketFontFaceHandle::GetLineHeight() const {
        return m_font->GetHeight();
    }
    
    int RocketFontFaceHandle::GetBaseline() const {
        return m_font->GetBaseline();
    }
    
    int RocketFontFaceHandle::GetStringWidth(const Rocket::Core::WString& string, Rocket::Core::word prior_character ) const {
        Rocket::Core::String utf8;
        string.ToUTF8(utf8);
        return m_font->GetTextWidth(utf8.CString());
    }
    
    
    void RocketFontFaceHandle::GenerateLine(Rocket::Core::Geometry* geometry, const Rocket::Core::Vector2f& position, int width, Rocket::Core::Font::Line height, const Rocket::Core::Colourb& colour) const {
        return;
    }
    
    int RocketFontFaceHandle::Draw(Graphics& g, const Rocket::Core::WString& string, const Rocket::Core::Vector2f& position, const Rocket::Core::Colourb& colour) const {
        Rocket::Core::String utf8;
        string.ToUTF8(utf8);
        Color c = g.GetColor();
        g.SetColor(c*Color(colour.red/255.0f,colour.green/255.0f,colour.blue/255.0f,colour.alpha/255.0f));
        int res = m_font->Draw(g, Vector2f(position.x,position.y), utf8.CString(), ALIGN_LEFT);
        g.SetColor(c);
        return res;
    }
}
