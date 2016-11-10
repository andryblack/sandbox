//
//  sb_mygui_client_widget.h
//  Sandbox
//
//
//

#ifndef _SB_MYGUI_CLIENT_WIDGET_H_INCLUDED_
#define _SB_MYGUI_CLIENT_WIDGET_H_INCLUDED_

#include "MyGUI_Widget.h"
#include "meta/sb_meta.h"

namespace Sandbox {
    
    namespace mygui {
        
        class ClientWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( ClientWidget )
        public:
            ClientWidget();
            ~ClientWidget();
            
            
        protected:
            void initialiseOverride();
        private:
        };
        
    }
    
}

#endif /* _SB_MYGUI_CLIENT_WIDGET_H_INCLUDED_ */
