#include "sb_mygui_font.h"
#include "sb_mygui_ft_font.h"
#include "sb_mygui_ft_font_ol.h"
#include "sb_mygui_multipass_font.h"
#include "sb_mygui_combine_font.h"

#include "MyGUI_FactoryManager.h"
#include "MyGUI_ResourceManager.h"

namespace Sandbox {
    namespace mygui {
        
        void register_font() {
            std::string resourceCategory = MyGUI::ResourceManager::getInstance().getCategoryName();
            MyGUI::FactoryManager::getInstance().registerFactory<ResourceTrueTypeFont>(resourceCategory);
            MyGUI::FactoryManager::getInstance().registerFactory<ResourceTrueTypeFontOutline>(resourceCategory);
            MyGUI::FactoryManager::getInstance().registerFactory<ResourceMultipassFont>(resourceCategory);
            MyGUI::FactoryManager::getInstance().registerFactory<ResourceCombineFont>(resourceCategory);
        }
        void unregister_font() {
            std::string resourceCategory = MyGUI::ResourceManager::getInstance().getCategoryName();
            MyGUI::FactoryManager::getInstance().unregisterFactory<ResourceTrueTypeFont>(resourceCategory);
            MyGUI::FactoryManager::getInstance().unregisterFactory<ResourceTrueTypeFontOutline>(resourceCategory);
            MyGUI::FactoryManager::getInstance().unregisterFactory<ResourceMultipassFont>(resourceCategory);
            MyGUI::FactoryManager::getInstance().unregisterFactory<ResourceCombineFont>(resourceCategory);
        }
        
    }
}
