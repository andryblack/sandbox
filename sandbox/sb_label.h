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
#include "sb_string.h"

namespace Sandbox {
	
	class Label : public SceneObject {
        SB_META_OBJECT
    public:
		Label();
		~Label();
		void Draw(Graphics& g) const;
		void SetPos(const Vector2f& pos) { m_pos = pos; }
		const Vector2f& GetPos() const { return m_pos;}
		void SetFont(const FontPtr& font) { m_font = font; }
		const FontPtr& GetFont() const { return m_font;}
		void SetAlign( FontAlign align) { m_align = align; }
		FontAlign GetAlign() const { return m_align;}
		void SetText(const char* text) { m_text = text; }
		const char* GetText() const { return m_text.c_str(); }
	private:
		Vector2f	m_pos;
		FontPtr		m_font;
		FontAlign	m_align;
		sb::string	m_text;
	};
	typedef sb::shared_ptr<Label> LabelPtr;
}

#endif /*SB_LABEL_H*/
