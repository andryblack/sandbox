/*
 *  sb_text_box.h
 *
 *  Copyright 2016 andryblack. All rights reserved.
 *
 */

#ifndef SB_TEXT_BOX_H_INCLUDED
#define SB_TEXT_BOX_H_INCLUDED

#include "sb_label.h"
#include "sb_font.h"
#include "sb_vector2.h"
#include <sbstd/sb_string.h>
#include "sb_color.h"
#include "sb_text_data.h"
#include <sbstd/sb_map.h>

namespace Sandbox {
    
    class TextDrawAttributes : public FontDrawAttributes {
        SB_META_OBJECT
    public:
        void SetPassColor(const sb::string& pass_name,const Color& c);
    protected:
        virtual bool BeginPass(Graphics& g, const FontPass& pass) const;
        virtual void EndPass(Graphics& g, const FontPass& pass) const;
    private:
        sb::map<sb::string,Color> m_pass_colors;
    };
    
    class TextBox : public Label  {
        SB_META_OBJECT
    public:
        TextBox();
        ~TextBox();
        void Draw(Graphics& g) const;
        void SetWidth(float w);
        float GetWidth() const { return m_width; }
        const Sizef& GetSize() const {
            return m_data.size;
        }
    protected:
        virtual void UpdateText();
        TextData    m_data;
        float       m_width;
    };
    typedef sb::intrusive_ptr<TextBox> TextBoxPtr;
    
}

#endif /*SB_TEXT_BOX_H_INCLUDED*/
