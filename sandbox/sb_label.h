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
		void Draw(Graphics& g) const SB_OVERRIDE;
        void SetFont(const FontPtr& font);
		const FontPtr& GetFont() const { return m_font;}
        void SetAlign( FontAlign align);
		FontAlign GetAlign() const { return m_align;}
        void SetText(const char* text);
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
        void Draw(Graphics& g) const SB_OVERRIDE;
        void SetColor(const Color& c) { m_color = c; }
        const Color& GetColor() const { return m_color; }
    private:
        Color   m_color;
    };
    
}

#endif /*SB_LABEL_H*/
