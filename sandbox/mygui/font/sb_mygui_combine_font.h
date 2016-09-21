#ifndef SB_MYGUI_COMBINE_FONT_H_INCLUDED
#define SB_MYGUI_COMBINE_FONT_H_INCLUDED

#include "MyGUI_Prerequest.h"
#include "MyGUI_ITexture.h"
#include "MyGUI_IFont.h"
#include "MyGUI_Colour.h"

namespace Sandbox {
    namespace mygui {
        class ResourceCombineFont : public MyGUI::IFont {
            MYGUI_RTTI_DERIVED( ResourceCombineFont )
        public:
            
            ResourceCombineFont();
            
            virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version);
            
            virtual size_t getNumPasses() const;
            virtual MyGUI::GlyphInfo* getGlyphInfo( int pass, MyGUI::Char _id);
            virtual const MyGUI::GlyphInfo* getSubstituteGlyphInfo() const;
            
            virtual int getDefaultHeight();
            virtual std::string getPassName( size_t pass );
            virtual MyGUI::FloatSize getOffset( size_t pass );
        private:
            std::vector<MyGUI::IFont*> mFonts;
        };
    }
}

#endif /*SB_MYGUI_COMBINE_FONT_H_INCLUDED*/
