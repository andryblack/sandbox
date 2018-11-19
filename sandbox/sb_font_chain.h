#ifndef SB_FONT_CHAIN_H_INCLUDED
#define SB_FONT_CHAIN_H_INCLUDED

#include "sb_font_data.h"
#include <sbstd/sb_vector.h>

namespace Sandbox {
    
    class FontChain : public OutlineFontDataProvider {
        SB_META_OBJECT
    private:
        struct Entry {
            FontDataProviderPtr font;
        };
        sb::vector<Entry> m_chain;
    protected:
         bool preallocate_symb(UTF32Char ch) SB_OVERRIDE;
    public:
        FontChain( const FontDataProviderPtr& first );
        void Add( const FontDataProviderPtr& next );
    };
    
}

#endif /*SB_FONT_CHAIN_H_INCLUDED*/
