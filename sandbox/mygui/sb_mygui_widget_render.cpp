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
#include "sb_mygui_render.h"
#include <sbstd/sb_platform.h>
#include "sb_graphics.h"


SB_META_DECLARE_OBJECT(Sandbox::mygui::WidgetRender, MyGUI::Widget)

namespace Sandbox {
    
    namespace mygui {
        
        
        WidgetRender::WidgetRender(MyGUI::IntSize size) : counter(0)  {
            m_replaced_layer = new MyGUI::SharedLayerNode(0,0);
            m_texture_name = get_type_info()->name;
            char buf[128];
            sb::snprintf(buf, 128, "_%p", this);
            m_texture_name += buf;
            mIsMargin = false;
            setSize(size);
            m_image = ImagePtr(new Image());
            
            MyGUI::Gui::getInstance()._linkChildWidget(this);
            m_target = static_cast<RenderManager*>(MyGUI::RenderManager::getInstancePtr())->createTarget(size);
        }
        
        WidgetRender::~WidgetRender() {
            delete m_target;
            delete m_replaced_layer;
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
        
        void WidgetRender::onWidgetCreated(MyGUI::Widget* _widget) {
            Base::onWidgetCreated(_widget);
        }
        
        void WidgetRender::addChildItem(MyGUI::LayerItem* _item) {
            _item->attachItemToNode(0, m_replaced_layer);
        }
        
        void WidgetRender::removeChildItem(MyGUI::LayerItem* _item) {
            
        }
        
        void WidgetRender::render() {
            _updateChilds();
            
            MyGUI::IntSize size = getSize();
            size.width = next_pot(size.width);
            size.height = next_pot(size.height);
            bool need_recreate = !m_target;
            if (!need_recreate) {
                if (size.width > m_target->getWidth())
                    need_recreate = true;
                if (size.height > m_target->getHeight())
                    need_recreate = true;
            }
            if (need_recreate) {
                if (!m_target) {
                    m_target = static_cast<RenderManager*>(MyGUI::RenderManager::getInstancePtr())->createTarget(size);
                } else {
                    m_target->resize(size);
                }
            }
           
            m_image->SetTexture( m_target->getTexture()->GetTexture() );
            m_image->SetTextureRect(0, 0, getWidth(), getHeight());
            m_image->SetSize(getWidth(), getHeight());
            
            
            if (m_target) {
                m_target->begin();
                Sandbox::Transform2d tr = m_target->graphics()->GetTransform();
                m_target->graphics()->SetTransform(tr.translated(-getAbsoluteLeft(),-getAbsoluteTop()));
                m_replaced_layer->renderToTarget(m_target, true);
                m_target->graphics()->SetTransform(tr);
                m_target->end();
            }
        }
    }
    
}
