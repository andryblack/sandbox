#include "sb_mygui_combine_font.h"
#include "MyGUI_FontManager.h"


namespace Sandbox {
    
    namespace mygui {
        
        ResourceCombineFont::ResourceCombineFont() {
            
        }
        
        
        void ResourceCombineFont::deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version) {
            Base::deserialization(_node, _version);
            
            MyGUI::xml::ElementEnumerator node = _node->getElementEnumerator();
            while (node.next())
            {
                if (node->getName() == "Font")
                {
                    std::string fontName = node->findAttribute("name");
                    if (!fontName.empty()) {
                        MyGUI::IFont* font = MyGUI::FontManager::getInstance().getByName(fontName);
                        if (font) {
                            mFonts.push_back(font);
                        } else {
                            MYGUI_EXCEPT("ResourceCombineFont: Could not load the font '" <<fontName << "' on '" << getResourceName() << "'!");
                        }
                    }
                }
            }
        }
        
        
        size_t ResourceCombineFont::getNumPasses() const {
            if (mFonts.empty()) return 0;
            return mFonts.front()->getNumPasses();
        }
        MyGUI::GlyphInfo* ResourceCombineFont::getGlyphInfo( int pass, MyGUI::Char _id) {
            if (mFonts.empty())
                return 0;
            MyGUI::GlyphInfo* res = 0;
            for (std::vector<MyGUI::IFont*>::const_iterator it = mFonts.begin();it!=mFonts.end();++it) {
                MyGUI::GlyphInfo* crnt = (*it)->getGlyphInfo(pass, _id);
                if (crnt != (*it)->getSubstituteGlyphInfo())
                    return crnt;
                if (!res) res = crnt;
            }
            return res;
        }
        const MyGUI::GlyphInfo* ResourceCombineFont::getSubstituteGlyphInfo() const {
            if (!mFonts.empty())
                return mFonts.front()->getSubstituteGlyphInfo();
            return 0;
        }
        
        std::string ResourceCombineFont::getPassName( size_t pass ) {
            if (!mFonts.empty()){
                return mFonts.front()->getPassName(pass);
            }
            return "";
        }
        
        MyGUI::FloatSize ResourceCombineFont::getOffset( size_t pass ) {
            if (!mFonts.empty()){
                return mFonts.front()->getOffset(pass);
            }
            return MyGUI::FloatSize(0,0);
        }
        
        int ResourceCombineFont::getDefaultHeight() {
            if (!mFonts.empty()){
                return mFonts.front()->getDefaultHeight();
            }
            return 0;
        }
        
    }
    
}
