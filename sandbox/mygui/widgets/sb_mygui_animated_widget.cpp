

#include "sb_mygui_animated_widget.h"
#include "MyGUI_ITexture.h"
#include "MyGUI_RenderManager.h"
#include "sb_math.h"
#include "MyGUI_Gui.h"
#include "MyGUI_LayerNode.h"
#include "../sb_mygui_render.h"
#include <sbstd/sb_platform.h>
#include "sb_graphics.h"
#include "sb_mygui_animated_layer.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedWidget, MyGUI::Widget)

namespace Sandbox {
    
    namespace mygui {
        
        
        AnimatedWidget::AnimatedWidget()  {
            m_thread.reset(new ThreadsMgr());
        }
        
        AnimatedWidget::~AnimatedWidget() {
        }
        
        void AnimatedWidget::initialiseOverride() {
            Base::initialiseOverride();
            MyGUI::Gui::getInstance().eventFrameStart += MyGUI::newDelegate( this, &AnimatedWidget::frameEntered );
        }
        
        void AnimatedWidget::shutdownOverride() {
            Base::shutdownOverride();
            MyGUI::Gui::getInstance().eventFrameStart -= MyGUI::newDelegate( this, &AnimatedWidget::frameEntered );
        }
        
        void AnimatedWidget::frameEntered(float dt) {
            m_thread->Update(dt);
        }
        
        void AnimatedWidget::attachToLayerItemNode(MyGUI::ILayerNode* _node, bool _deep) {
            Base::attachToLayerItemNode(_node, _deep);
            AnimatedLayerNode* n = _node->castType<AnimatedLayerNode>(false);
            if (n) {
                m_transform = n->GetTransformModificator();
                m_color = n->GetColorModificator();
            }
        }
                
    }
    
}
