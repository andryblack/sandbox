/*
 *  sb_label.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 12.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_label.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::Label, Sandbox::SceneObjectWithPosition)
SB_META_DECLARE_OBJECT(Sandbox::ColorizedLabel, Sandbox::Label)

namespace Sandbox {

	Label::Label() : m_align(ALIGN_LEFT){
	}
	Label::~Label() {
	}

	void Label::Draw(Graphics& g) const {
		if (m_font && !m_text.empty()) {
			m_font->Draw(g,GetDrawAttributes().get(),GetPos(),m_text.c_str(),m_align);
		}
	}
    
    void Label::SetFont(const FontPtr& font) {
        if (m_font!=font) {
            m_font = font;
            if (m_font) {
                m_font->AllocateSymbols(m_text.c_str());
            }
            UpdateText();
        }
    }
    void Label::SetAlign( FontAlign align) {
        if (align != m_align) {
            m_align = align;
            UpdateText();
        }
    }
    void Label::SetText(const char* text) {
        if (strcmp(text, m_text.c_str())!=0) {
            m_text.assign(text);
            if (m_font) {
                m_font->AllocateSymbols(m_text.c_str());
            }
            UpdateText();
        }
    }
    
    void ColorizedLabel::Draw(Graphics& g) const {
        Color prev = g.GetColor();
        g.SetColor(prev*m_color);
        Label::Draw(g);
        g.SetColor(prev);
    }
    
}
