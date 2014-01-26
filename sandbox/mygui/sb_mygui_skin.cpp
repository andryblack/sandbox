#include "sb_mygui_skin.h"
#include "MyGUI_FactoryManager.h"
#include "MyGUI_SubWidgetManager.h"
#include "meta/sb_meta.h"


SB_META_DECLARE_OBJECT(Sandbox::mygui::ColorizedSubSkinStateInfo,MyGUI::SubSkinStateInfo)

SB_META_DECLARE_OBJECT(Sandbox::mygui::ColorizedSubSkin,MyGUI::SubSkin)

namespace Sandbox {

    namespace mygui {
        
     
        void ColorizedSubSkin::applyColor() {
            MyGUI::Colour val(m_color.red*m_state_color.red,
                              m_color.green*m_state_color.green,
                              m_color.blue*m_state_color.blue,
                              m_color.alpha*m_state_color.alpha);
            MyGUI::uint32 colour = MyGUI::texture_utility::toColourARGB(val);
            mCurrentColour = (colour & 0x00FFFFFF) | (mCurrentColour & 0xFF000000);
            MyGUI::texture_utility::convertColour(mCurrentColour, mVertexFormat);
        }
        void ColorizedSubSkin::_setColour(const MyGUI::Colour& _value) {
            m_color = _value;
            applyColor();
            if (nullptr != mNode)
                mNode->outOfDate(mRenderItem);
        }
        void ColorizedSubSkin::setStateData(MyGUI::IStateInfo* _data) {
            MyGUI::SubSkin::setStateData(_data);
            m_state_color = _data->castType<ColorizedSubSkinStateInfo>()->getColor();
            
            applyColor();
            
            if (nullptr != mNode)
                mNode->outOfDate(mRenderItem);
        }
        
        void register_skin() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            
            factory.registerFactory<ColorizedSubSkinStateInfo>(MyGUI::SubWidgetManager::getInstance().getStateCategoryName(), "ColorizedSubSkin");
                        
            factory.registerFactory<ColorizedSubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
        }
        
        void unregister_skin() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            factory.unregisterFactory(MyGUI::SubWidgetManager::getInstance().getStateCategoryName(), "ColorizedSubSkin");
            factory.unregisterFactory<ColorizedSubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
        }
        
    }
    
}
