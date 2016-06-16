#ifndef SB_MYGUI_SKIN_H_INCLUDED
#define SB_MYGUI_SKIN_H_INCLUDED

#include "MyGUI_Prerequest.h"
#include "MyGUI_CommonStateInfo.h"
#include "MyGUI_SubSkin.h"


namespace Sandbox {
    
    namespace mygui {
        
        class ColorizedSubSkinStateInfo : public MyGUI::SubSkinStateInfo {
            MYGUI_RTTI_DERIVED( ColorizedSubSkinStateInfo )
        public:
            virtual ~ColorizedSubSkinStateInfo() { }
            
            const MyGUI::Colour& getColor() const
            {
                return m_color;
            }
        private:
            virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version)
            {
                MyGUI::SubSkinStateInfo::deserialization(_node,_version);
                std::string text = _node->findAttribute("color");
                if (!text.empty())
                    m_color = MyGUI::Colour::parse(text);
            }
            MyGUI::Colour    m_color;
        };
        
        class ColorizedSubSkin : public MyGUI::SubSkin {
            MYGUI_RTTI_DERIVED( ColorizedSubSkin )
        public:
            virtual void _setColour(const MyGUI::Colour& _value);
            virtual void setStateData(MyGUI::IStateInfo* _data);
        private:
            MyGUI::Colour   m_color;
            MyGUI::Colour   m_state_color;
            void applyColor();
        };
        
        class CopySubSkin : public MyGUI::SubSkin {
            MYGUI_RTTI_DERIVED(CopySubSkin)
        public:
            virtual void _setUVSet(const MyGUI::FloatRect& _rect);
            virtual void _updateView();
            virtual void createDrawItem(MyGUI::ITexture* _texture,  MyGUI::ILayerNode* _node);
            virtual void doRender(MyGUI::IRenderTarget* _target);
        private:
        };
        
        void register_skin();
        void unregister_skin();
        
    }
    
}


#endif