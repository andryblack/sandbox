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
SB_META_DECLARE_OBJECT(Sandbox::MultilineLabel, Sandbox::Label)
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
    
    void ColorizedLabel::Draw(Graphics& g) const {
        Color prev = g.GetColor();
        g.SetColor(prev*m_color);
        Label::Draw(g);
        g.SetColor(prev);
    }
    
    MultilineLabel::MultilineLabel() : m_width(100.0f){
        
    }
    
    MultilineLabel::~MultilineLabel() {
        
    }
    
    void MultilineLabel::Draw(Graphics& g) const {
        if (m_font && !m_text.empty()) {
            for (sb::vector<Line>::const_iterator it = m_lines.begin();it!=m_lines.end();++it) {
                m_font->Draw(g, GetDrawAttributes().get(), GetPos()+it->pos, it->text.c_str(), m_align);
            }
		}
    }
    
    float MultilineLabel::GetHeight() const {
        if (!m_font) return 0.0f;
        return GetLines()*m_font->GetHeight();
    }
    
    struct MultilineLabel::MultilineContext {
        const char* text;
        const char* text_start;
        Vector2f pos;
        MultilineLabel& label;
        
        MultilineContext(MultilineLabel& label,const char* text) :
        label(label){
            this->text = text_start = text;
        }
        
        void addLine(const char* begin,const char* end) {
            Line l;
            l.text.assign(begin,end);
            l.pos.y = 0.0f;
            if (!label.m_lines.empty()) {
                l.pos.y = label.m_lines.back().pos.y + label.m_font->GetHeight();
            }
            label.m_lines.push_back(l);
            text = text_start = end;
            pos.x = 0.0f;
        }
        
        void find_space() {
            const char* text1 = text_start;
            const char* last_space = text;
            while (text1!=text) {
                UTF32Char ch = 0;
                text1 = get_char(text1, ch);
                if (ch == ' ' || ch == '-' || ch == '=' || ch == ',' || ch == '.') {
                    last_space = text1;
                }
            }
            if (last_space!=text) {
                text = last_space;
            }
        }
        
        void process() {
            
            UTF32Char prev = 0;
            while (*text) {
                UTF32Char ch = 0;
                text = get_char(text, ch);
                if (ch == '\n') {
                    addLine(text_start, text-1);
                    text = text_start = text+1;
                    ch = 0;
                } else {
                    label.m_font->MovePosition(pos, prev, ch);
                    if (pos.x  > label.m_width) {
                        find_space();
                        addLine(text_start, text);
                        ch = 0;
                    }
                }
                prev = ch;
            }
            if (label.m_lines.empty()) {
                Line l;
                l.text = label.m_text;
                label.m_lines.push_back(l);
            } else {
                if (text_start!=text) {
                    addLine(text_start, text);
                }
            }
        }
        
    };
    void MultilineLabel::UpdateText() {
        m_lines.clear();
        if (m_font) {
            MultilineContext ctx(*this,m_text.c_str());
            ctx.process();
        }
    }
}
