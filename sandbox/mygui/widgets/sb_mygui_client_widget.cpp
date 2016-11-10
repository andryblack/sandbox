#include "sb_mygui_client_widget.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::ClientWidget, MyGUI::Widget)

namespace Sandbox {
    
    namespace mygui {
        
        ClientWidget::ClientWidget() {
        }
        
        ClientWidget::~ClientWidget() {
            
        }
        
        void ClientWidget::initialiseOverride() {
            Base::initialiseOverride();
            MyGUI::Widget* client;
            assignWidget(client, "Client");
            setWidgetClient(client);
        }
        
    }
    
}
