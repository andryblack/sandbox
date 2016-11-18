//
//  sb_mygui_client_widget.h
//  Sandbox
//
//
//

#ifndef _SB_MYGUI_CLIENT_WIDGET_H_INCLUDED_
#define _SB_MYGUI_CLIENT_WIDGET_H_INCLUDED_

#include "sb_mygui_animated_widget.h"
#include "meta/sb_meta.h"

namespace Sandbox {
    
    namespace mygui {
        
        class ClientWidget : public AnimatedWidget {
            MYGUI_RTTI_DERIVED( ClientWidget )
        public:
            ClientWidget();
            ~ClientWidget();
            
            
        protected:
            void initialiseOverride();
            void shutdownOverride();
        private:
            MyGUI::VectorWidgetPtr  m_saved_widgets;
        };
        
    }
    
}

#endif /* _SB_MYGUI_CLIENT_WIDGET_H_INCLUDED_ */
