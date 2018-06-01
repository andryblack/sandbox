#include "sb_font_chain.h"

SB_META_DECLARE_OBJECT(Sandbox::FontChain,Sandbox::OutlineFontDataProvider)

namespace Sandbox {
    
    
    FontChain::FontChain(const FontDataProviderPtr& first) : OutlineFontDataProvider() {
        OutlineFontDataProvider* p = meta::sb_dynamic_cast<OutlineFontDataProvider>(first.get());
        if (p) {
            set_outline_data(p->GetOutlineData());
        }
        set_data(first->GetMainData());
        set_size(first->GetSize());
        set_height(first->GetHeight());
        set_baseline(first->GetBaseline());
        set_x_height(first->GetXHeight());
        Entry e = { first };
        m_chain.push_back(e);
    }
    
    void FontChain::Add(const Sandbox::FontDataProviderPtr &next) {
        if (!next) {
            return;
        }
        next->set_data(GetMainData());
        OutlineFontDataProvider* p = meta::sb_dynamic_cast<OutlineFontDataProvider>(next.get());
        if (p) {
            p->set_outline_data(m_outline_data);
        }
        Entry e = { next };
        m_chain.push_back(e);
    }
    
    bool FontChain::preallocate_symb(UTF32Char ch)  {
        for (sb::vector<Entry>::const_iterator it = m_chain.begin();
             it!=m_chain.end();
             ++it) {
            const Entry& e(*it);
            if (e.font->preallocate_symb(ch)) {
                return true;
            }
        }
        return false;
    }
}
