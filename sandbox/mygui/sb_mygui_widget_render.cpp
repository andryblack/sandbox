//
//  sb_mygui_cached_widget.cpp
//  Sandbox
//
//  Created by Andrey Kunitsyn on 30/01/14.
//
//

#include "sb_mygui_widget_render.h"
#include "MyGUI_ITexture.h"
#include "MyGUI_RenderManager.h"
#include "sb_math.h"
#include "MyGUI_Gui.h"
#include "MyGUI_SharedLayerNode.h"
#include "MyGUI_SharedLayer.h"
#include "sb_mygui_render.h"
#include <sbstd/sb_platform.h>
#include "sb_graphics.h"


SB_META_DECLARE_OBJECT(Sandbox::mygui::WidgetRender, Sandbox::mygui::CachedWidget)

namespace Sandbox {
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(WidgetRender)
        
        WidgetRender::WidgetRender(MyGUI::IntSize size) : counter(0)  {
            setRenderContent(true);
            mIsMargin = false;
            setSize(size);
            m_image = ImagePtr(new Image());
            MyGUI::Gui::getInstance()._linkChildWidget(this);
        }
        
        WidgetRender::~WidgetRender() {
            
        }
        
        void WidgetRender::remove_ref() const {
            sb_assert(counter!=0);
            --counter;
            if (counter==0) {
                MyGUI::Gui::getInstance().destroyWidget(const_cast<WidgetRender*>(this));
            }
        }
        
        void WidgetRender::initialiseOverride() {
            Base::initialiseOverride();
            
        }
        
        void WidgetRender::shutdownOverride() {
            Base::shutdownOverride();
        }
        
        void WidgetRender::render() {
            _updateChilds();
            RenderTargetImpl* target = renderToTarget(true);
            if (target) {
                m_image->SetTexture( target->getTexture()->GetTexture() );
                m_image->SetTextureRect(0, 0, getWidth(), getHeight());
                m_image->SetSize(getWidth(), getHeight());
            }
        }
    }
    
}
