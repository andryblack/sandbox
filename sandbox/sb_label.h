/*
 *  sb_label.h
 *  SR
 *
 *  Created by Андрей Куницын on 12.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_LABEL_H
#define SB_LABEL_H

#include "sb_scene_object.h"
#include "sb_font.h"
#include "sb_vector2.h"
#include <sbstd/sb_string.h>
#include "sb_color.h"

namespace Sandbox {
	
	class Label : public SceneObjectWithPosition {
        SB_META_OBJECT
    public:
		Label();
		~Label();
		void Draw(Graphics& g) const;
		void SetFont(const FontPtr& font) { m_font = font; }
		const FontPtr& GetFont() const { return m_font;}
		void SetAlign( FontAlign align) { m_align = align; }
		FontAlign GetAlign() const { return m_align;}
		void SetText(const char* text) { m_text = text; UpdateText();}
		const char* GetText() const { return m_text.c_str(); }
    protected:
        virtual void UpdateText() {}
		FontPtr		m_font;
		FontAlign	m_align;
		sb::string	m_text;
	};
	typedef sb::intrusive_ptr<Label> LabelPtr;
    
    class ColorizedLabel : public Label {
        SB_META_OBJECT
    public:
        void Draw(Graphics& g) const;
        void SetColor(const Color& c) { m_color = c; }
        const Color& GetColor() const { return m_color; }
    private:
        Color   m_color;
    };
    
    class MultilineLabel : public Label {
        SB_META_OBJECT
    public:
        MultilineLabel();
        ~MultilineLabel();
        void Draw(Graphics& g) const;
        void SetWidth(float w) { m_width = w; UpdateText(); }
        float GetWidth() const { return m_width; }
        size_t GetLines() const { return m_lines.size(); }
        float GetHeight() const;
    protected:
        virtual void UpdateText();
        struct Line {
            Vector2f pos;
            sb::string text;
        };
        struct MultilineContext;
		friend struct MultilineContext;
        sb::vector<Line> m_lines;
        float   m_width;
    };
}

#endif /*SB_LABEL_H*/
