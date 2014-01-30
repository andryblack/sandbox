//
//  sb_mygui_widgets.cpp
//  Sandbox
//
//  Created by Andrey Kunitsyn on 30/01/14.
//
//

#include "sb_mygui_widgets.h"

#include "sb_mygui_scroll_list.h"
#include "sb_mygui_cached_widget.h"

#include "MyGUI_WidgetManager.h"
#include "MyGUI_FactoryManager.h"

namespace Sandbox {
    
    namespace mygui {
        
        void register_widgets() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            
            factory.registerFactory<ScrollList>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.registerFactory<CachedWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
        }
        void unregister_widgets() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            
            factory.unregisterFactory<ScrollList>(MyGUI::WidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<CachedWidget>(MyGUI::WidgetManager::getInstance().getCategoryName());
        }

    }
    
}
