#ifndef SB_MYGUI_MULTIPASS_FONT_H_INCLUDED
#define SB_MYGUI_MULTIPASS_FONT_H_INCLUDED

#include "MyGUI_Prerequest.h"
#include "MyGUI_ITexture.h"
#include "MyGUI_IFont.h"
#include "MyGUI_Colour.h"

namespace Sandbox {
    namespace mygui {
        class ResourceMultipassFont : public MyGUI::IFont {
            MYGUI_RTTI_DERIVED( ResourceMultipassFont )
        public:
            
            ResourceMultipassFont();
            
            virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version);
            
            virtual size_t getNumPasses() const;
            virtual MyGUI::GlyphInfo* getGlyphInfo( int pass, MyGUI::Char _id);
            
            virtual int getDefaultHeight();
            virtual bool getColour( size_t pass, MyGUI::Colour& clr );
            virtual MyGUI::FloatSize getOffset( size_t pass );
        private:
            struct Pass {
                MyGUI::IFont*   mFont;
                bool    mUseColour;
                MyGUI::Colour   mColour;
                MyGUI::FloatSize  mOffset;
            };
            std::vector<Pass> mPasses;
            MyGUI::IFont*   mMainFont;
        };
    }
}

#endif /*SB_MYGUI_MULTIPASS_FONT_H_INCLUDED*/