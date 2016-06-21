#include "sb_mygui_multipass_font.h"
#include "MyGUI_FontManager.h"


namespace Sandbox {
    
    namespace mygui {
        
        ResourceMultipassFont::ResourceMultipassFont() : mMainFont(0) {
            
        }
        
        
        void ResourceMultipassFont::deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version) {
            Base::deserialization(_node, _version);
            
            MyGUI::xml::ElementEnumerator node = _node->getElementEnumerator();
            while (node.next())
            {
                if (node->getName() == "Pass")
                {
                    std::string fontName = node->findAttribute("font");
                    if (!fontName.empty()) {
                        MyGUI::IFont* font = MyGUI::FontManager::getInstance().getByName(fontName);
                        if (font) {
                            Pass pass;
                            pass.mFont = font;
                            if (!mMainFont || MyGUI::utility::parseBool(node->findAttribute("main"))) {
                                mMainFont = font;
                            }
                            pass.mName = node->findAttribute("name");
                            std::string value;
                            if (node->findAttribute("offset",value)) {
                                pass.mOffset = MyGUI::utility::parseValue<MyGUI::FloatSize>(value);
                            }
                            mPasses.push_back(pass);
                        } else {
                            MYGUI_EXCEPT("ResourceMultipassFont: Could not load the font pass '" <<fontName << "' on '" << getResourceName() << "'!");
                        }
                    }
                }
            }
        }

        
        size_t ResourceMultipassFont::getNumPasses() const {
            return mPasses.size();
        }
        MyGUI::GlyphInfo* ResourceMultipassFont::getGlyphInfo( int pass, MyGUI::Char _id) {
            if (!mMainFont)
                return 0;
            MyGUI::GlyphInfo* res = 0;
            if (pass < 0) {
                return mMainFont->getGlyphInfo(0,_id);
            }
            if (pass < mPasses.size()) {
                res = mPasses[pass].mFont->getGlyphInfo(0,_id);
            }
            return res;
        }
        
        std::string ResourceMultipassFont::getPassName( size_t pass ) {
            if (pass < mPasses.size()) {
                return mPasses[pass].mName;
            }
            return "";
        }
        
        MyGUI::FloatSize ResourceMultipassFont::getOffset( size_t pass ) {
            if (pass < mPasses.size()) {
                return mPasses[pass].mOffset;
            }
            return MyGUI::FloatSize(0,0);
        }
        
        int ResourceMultipassFont::getDefaultHeight() {
            if (!mPasses.empty()) {
                return mPasses.front().mFont->getDefaultHeight();
            }
            return 0;
        }
        
    }
    
}