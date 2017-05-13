#include "sb_text_box.h"
#include "sb_font.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::TextBox, Sandbox::Label)
SB_META_DECLARE_OBJECT(Sandbox::TextDrawAttributes, Sandbox::FontDrawAttributes)


namespace Sandbox {
    
    void TextDrawAttributes::SetPassColor(const sb::string &pass_name, const Sandbox::Color &c) {
        m_pass_colors[pass_name] = c;
    }
    
    bool TextDrawAttributes::BeginPass(Sandbox::Graphics &g, const Sandbox::FontPass &pass) const {
        sb::map<sb::string,Color>::const_iterator it = m_pass_colors.find(pass.GetName());
        if (it!=m_pass_colors.end()) {
            g.SetColor(g.GetColor()*it->second);
        }
        return true;
    }
    
    void TextDrawAttributes::EndPass(Sandbox::Graphics &g, const Sandbox::FontPass &pass) const{
        
    }
    
    TextBox::TextBox() : m_width(-1.0f) {
        m_data.Clear();
    }
    
    TextBox::~TextBox() {
        
    }
    
    void TextBox::Draw(Graphics &g) const {
        if (!m_font)
            return;
        Sandbox::Transform2d tr = g.GetTransform();
        Sandbox::Vector2f offset(0,0);
        if (m_align == ALIGN_RIGHT) {
            offset.x = -m_data.size.w;
        } else if (m_align == ALIGN_CENTER) {
            offset.x = -m_data.size.w / 2;
        }
        g.SetTransform(tr.translated(GetPos()+offset));
        m_font->Draw(g, GetDrawAttributes().get(), m_data);
        g.SetTransform(tr);
    }
    
    
    void TextBox::SetWidth(float w) {
        if (m_width != w) {
            m_width = w;
            UpdateText();
        }
    }
    void TextBox::UpdateText() {
        if (m_font) {
            m_data.Fill(m_text.c_str(), m_font, int(m_width), m_align);
        }
    }
    
}
