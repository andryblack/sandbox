//
//  sb_mygui_cached_widget.cpp
//  Sandbox
//
//  Created by Andrey Kunitsyn on 30/01/14.
//
//

#include "sb_mygui_cached_widget.h"
#include "MyGUI_ITexture.h"
#include "MyGUI_RenderManager.h"
#include "sb_math.h"
#include "MyGUI_Gui.h"
#include "MyGUI_LayerNode.h"
#include "../sb_mygui_render.h"
#include <sbstd/sb_platform.h>
#include "sb_graphics.h"


SB_META_DECLARE_OBJECT(Sandbox::mygui::CachedWidget, MyGUI::Widget)

namespace Sandbox {
    
    namespace mygui {
        
        
        CachedWidget::CachedWidget() : m_render_content(false) {
            m_texture = 0;
            m_replaced_layer = new MyGUI::SharedLayerNode(0,0);
            m_texture_name = get_type_info()->name;
            char buf[128];
			sb::snprintf(buf, 128, "_%p", this);
            m_texture_name += buf;
           
        }
        
        CachedWidget::~CachedWidget() {
            delete m_replaced_layer;
        }
        
        void CachedWidget::initialiseOverride() {
            Base::initialiseOverride();
            MyGUI::Gui::getInstance().eventFrameStart += MyGUI::newDelegate( this, &CachedWidget::frameEntered );
            m_texture = MyGUI::RenderManager::getInstance().createTexture(m_texture_name);
            setRenderItemTexture(m_texture);
        }
        
        void CachedWidget::shutdownOverride() {
            Base::shutdownOverride();
            MyGUI::Gui::getInstance().eventFrameStart -= MyGUI::newDelegate( this, &CachedWidget::frameEntered );
            if (m_texture) {
                MyGUI::RenderManager::getInstance().destroyTexture(m_texture);
                m_texture = 0;
            }
        }
        
        void CachedWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "ForceRender") {
                m_render_content = MyGUI::utility::parseBool(_value);
            } else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        void CachedWidget::onWidgetCreated(MyGUI::Widget* _widget) {
            Base::onWidgetCreated(_widget);
            _widget->setInheritsAlpha(false);
        }
        
        void CachedWidget::addChildItem(LayerItem* _item) {
            _item->attachItemToNode(0, m_replaced_layer);
        }
        
        void CachedWidget::removeChildItem(LayerItem* _item) {
          
        }
        
        void CachedWidget::addChildNode(LayerItem* _item) {
            MyGUI::ILayerNode* child_node = m_replaced_layer->createChildItemNode();
            _item->attachItemToNode(0, child_node);
        }
        void CachedWidget::removeChildNode(LayerItem* _item) {
            
        }
        
        void CachedWidget::frameEntered(float dt) {
            MyGUI::IntSize size = getSize();
            size.width = next_pot(size.width);
            size.height = next_pot(size.height);
            bool need_recreate = !m_texture;
            if (!need_recreate) {
                if (size.width > m_texture->getWidth())
                    need_recreate = true;
                if (size.height > m_texture->getHeight())
                    need_recreate = true;
            }
            if (need_recreate) {
                m_texture->createManual(size.width, size.height, MyGUI::TextureUsage::RenderTarget, MyGUI::PixelFormat::R8G8B8A8);
                _updateView();
            } else {
                if (!m_replaced_layer->isOutOfDate() && !m_render_content)
                    return;
            }

            if (m_texture) {
                MyGUI::IRenderTarget* rt = m_texture->getRenderTarget();
                if (rt) {
                    rt->begin();
                    RenderTargetImpl* impl = static_cast<RenderTargetImpl*>(rt);
                    Sandbox::Transform2d tr = impl->graphics()->GetTransform();
                    impl->graphics()->SetTransform(tr.translated(-getAbsoluteLeft(),-getAbsoluteTop()));
                    doRenderToTarget(rt);
                    impl->graphics()->SetTransform(tr);
                    rt->end();
                }
            }
        }
        
        void CachedWidget::doRenderToTarget(MyGUI::IRenderTarget* rt) {
            m_replaced_layer->renderToTarget(rt, true);
        }

    }
    
}
