#include "sb_mygui_client_widget.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::ClientWidget, Sandbox::mygui::AnimatedWidget )

namespace Sandbox {
    
    namespace mygui {
        
        ClientWidget::ClientWidget() {
        }
        
        ClientWidget::~ClientWidget() {
            
        }
        
        void ClientWidget::shutdownOverride() {
            Base::shutdownOverride();
            MyGUI::EnumeratorWidgetPtr e = getEnumerator();
            while (e.next()) {
                MyGUI::Widget* w = e.current();
                //w->detachFromWidget();
                m_saved_widgets.push_back(w);
            }
            setWidgetClient(0);
        }
        
        void ClientWidget::initialiseOverride() {
            Base::initialiseOverride();
            MyGUI::Widget* client = 0;
            assignWidget(client, "Client");
            setWidgetClient(client);
            for (MyGUI::VectorWidgetPtr::iterator it = m_saved_widgets.begin();it!=m_saved_widgets.end();++it) {
                (*it)->attachToWidget(this);
            }
            m_saved_widgets.clear();
        }
        
    }
    
}
