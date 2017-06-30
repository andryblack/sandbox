#ifndef _SB_MYGUI_BACKGROUND_WIDGET_H_INCLUDED_
#define _SB_MYGUI_BACKGROUND_WIDGET_H_INCLUDED_

#include "sb_mygui_scene_object.h"
#include "sb_background.h"

namespace Sandbox {
    namespace mygui {
    
        class BackgroundWidget : public SceneObjectWidget {
            MYGUI_RTTI_DERIVED( BackgroundWidget )
        public:
            BackgroundWidget();
            void setBackground(const char* mame);
        protected:
            void setPropertyOverride(const std::string& _key, const std::string& _value);
            void initialiseOverride();
            void shutdownOverride();
        private:
            sb::intrusive_ptr<Background> m_background;
        };
    }
}

#endif /*_SB_MYGUI_BACKGROUND_WIDGET_H_INCLUDED_*/
