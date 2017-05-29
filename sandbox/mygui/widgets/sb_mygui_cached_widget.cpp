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
        
        
        ReplacedLayer::ReplacedLayer() : m_root(this,0) {
            
        }
        
        MyGUI::ILayerNode* ReplacedLayer::createRootItemNode(MyGUI::Widget* _item) {
            return &m_root;
        }
        void ReplacedLayer::destroyRootItemNode(MyGUI::ILayerNode* _item) {
            MyGUI::ILayerNode* parent = _item->getParent();
            if (parent)
            {
                parent->destroyChildItemNode(_item);
                return;
            }
        }
        
        // up child item (make it draw and pick above other)
        void ReplacedLayer::upRootItemNode(MyGUI::ILayerNode* _node) {
            // none
        }
        
        // child items list
        
        size_t ReplacedLayer::getLayerNodeCount() const {
            return 1;
        }
        
        MyGUI::ILayerNode* ReplacedLayer::getLayerNodeAt(size_t _index) const {
            return const_cast<MyGUI::LayerNode*>(&m_root);
        }
        
        // return widget at position
        MyGUI::ILayerItem* ReplacedLayer::getLayerItemByPoint(int _left, int _top) const {
            return m_root.getLayerItemByPoint(_left, _top);
        }
        MyGUI::ILayerItem* ReplacedLayer::checkLayerItemByPoint(const MyGUI::ILayerItem* _target, int _left, int _top) const {
            return m_root.checkLayerItemByPoint(_target, _left, _top);
        }
        
        
        // return position in layer coordinates
        MyGUI::FloatPoint ReplacedLayer::getPosition(float _left, float _top) const {
            return MyGUI::FloatPoint(_left,_top);
        }
        
        // return layer size
        const MyGUI::IntSize& ReplacedLayer::getSize() const {
            return m_view_size;
        }
        
        // render layer
        void ReplacedLayer::renderToTarget(MyGUI::IRenderTarget* _target, bool _update) {
            m_root.renderToTarget(_target, _update);
        }
        
        void ReplacedLayer::resizeView(const MyGUI::IntSize& _viewSize) {
            m_view_size = _viewSize;
            m_root.resizeView(_viewSize);
        }

        MYGUI_IMPL_TYPE_NAME(CachedWidget)
        
        
        CachedWidget::CachedWidget() : m_render_content(false) {
            m_target = 0;
            m_replaced_layer = new ReplacedLayer();
            m_suboffset = Sandbox::Vector2f(0,0);
        }
        
        CachedWidget::~CachedWidget() {
            delete m_replaced_layer;
        }
        
        void CachedWidget::initialiseOverride() {
            Base::initialiseOverride();
            MyGUI::Gui::getInstance().eventFrameStart += MyGUI::newDelegate( this, &CachedWidget::frameEntered );
            m_target = static_cast<mygui::RenderManager*>(MyGUI::RenderManager::getInstancePtr())->createTarget(getSize());
            if (m_target)
                setRenderItemTexture(m_target->getTexture());
        }
        
        void CachedWidget::shutdownOverride() {
            Base::shutdownOverride();
            MyGUI::Gui::getInstance().eventFrameStart -= MyGUI::newDelegate( this, &CachedWidget::frameEntered );
            if (m_target) {
                delete m_target;
                m_target = 0;
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
            _item->_attachToLayerItemNode(m_replaced_layer->getRoot(), false);
        }
        
        void CachedWidget::removeChildItem(LayerItem* _item) {
            //_item->detachFromLayer();
        }
        
        void CachedWidget::addChildNode(LayerItem* _item) {
            //mLayerNodes.push_back(_item);
            // создаем оверлаппеду новый айтем
            MyGUI::ILayerNode* child_node = _item->createChildItemNode(m_replaced_layer->getRoot());
            _item->_attachToLayerItemNode(child_node, true);
        }
        
        void CachedWidget::removeChildNode(LayerItem* _item) {
//            MyGUI::ILayerNode* node = _item->getLayerNode();
//            if (node) {
//                node->detachLayerItem(_item);
//                m_replaced_layer->getRoot()->destroyChildItemNode(node);
//            }
        }
        
        void CachedWidget::frameEntered(float dt) {
            renderToTarget();
        }
        RenderTargetImpl* CachedWidget::renderToTarget() {
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
                if (m_target) {
                    m_target->resize(size);
                } else {
                    m_target = static_cast<mygui::RenderManager*>(MyGUI::RenderManager::getInstancePtr())->createTarget(size);
                }
                setRenderItemTexture(m_target->getTexture());
                m_replaced_layer->resizeView(size);
                _updateView();
            } else {
                if (!m_replaced_layer->getRoot()->isOutOfDate() && !m_render_content)
                    return 0;
            }

            if (m_target) {
                m_target->begin();
                Sandbox::Transform2d tr = m_target->graphics()->GetTransform();
                m_target->graphics()->SetTransform(tr.translated(-getAbsoluteLeft(),-getAbsoluteTop()).translated(m_suboffset));
                doRenderToTarget(m_target);
                m_target->graphics()->SetTransform(tr);
                m_target->end();
            }
            return m_target;
        }
        
        void CachedWidget::doRenderToTarget(MyGUI::IRenderTarget* rt) {
            m_replaced_layer->renderToTarget(rt, true);
        }

    }
    
}
