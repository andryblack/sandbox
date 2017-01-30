#include "sb_mygui_animated_layer.h"
#include "sb_mygui_render.h"
#include "sb_graphics.h"
#include "widgets/sb_mygui_animated_widget.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedLayer, MyGUI::OverlappedLayer)
SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedLayerNode, MyGUI::LayerNode)


namespace Sandbox {
    
    namespace mygui {
        
        AnimatedLayerProxy::AnimatedLayerProxy() : m_node(0) {
            
        }
        
        AnimatedLayerProxy::~AnimatedLayerProxy() {
            if (m_node) {
                m_node->resetProxy();
            }
        }
        
        void AnimatedLayerProxy::resetLayerNode(AnimatedLayerNode *node) {
            if (node == m_node) {
                m_node->resetProxy();
                m_node = 0;
            }
        }
        
        void AnimatedLayerProxy::attachLayerNode(AnimatedLayerNode *node) {
            if (m_node == node)
                return;
            if (m_node) {
                m_node->resetProxy();
            }
            m_node = node;
        }
    
        AnimatedLayerNode::AnimatedLayerNode( ILayerNode* _parent , AnimatedLayerProxy* proxy ) : MyGUI::LayerNode(_parent->getLayer(),_parent), m_proxy(proxy) {
            sb_assert(getLayer());
            if (m_proxy) {
                m_proxy->attachLayerNode(this);
            }
        }
        
        AnimatedLayerNode::AnimatedLayerNode( AnimatedLayer* _layer , MyGUI::Widget* _widget) : MyGUI::LayerNode(_layer,0), m_proxy(0) {
            sb_assert(getLayer());
            if (_widget) {
                AnimatedWidget* aw = _widget->castType<AnimatedWidget>(false);
                if (aw) {
                    m_proxy = aw;
                    aw->attachLayerNode(this);
                }
            }
          }
        
        AnimatedLayerNode::~AnimatedLayerNode() {
            if (m_proxy) {
                m_proxy->resetLayerNode(this);
            }
        }
        
        void AnimatedLayerNode::resetProxy() {
            m_proxy = 0;
        }
        
        void AnimatedLayerNode::detachLayerItem(MyGUI::ILayerItem* _root) {
            if (std::find(mLayerItems.begin(), mLayerItems.end(), _root)!=mLayerItems.end()) {
                Base::detachLayerItem(_root);
            }
        }
        
        void AnimatedLayerNode::renderToTarget(MyGUI::IRenderTarget* _target, bool _update) {
            if (m_proxy) {
                if (m_proxy->renderNodeToTarget(_target, this, _update)) {
                    return;
                }
            }
            Base::renderToTarget(_target, _update);
        }
        
        
    AnimatedLayer::AnimatedLayer() {
        
    }
    
    AnimatedLayer::~AnimatedLayer() {
        
    }
    
        MyGUI::ILayerNode* AnimatedLayer::createRootItemNode(MyGUI::Widget* widget) {
            // создаем рутовый айтем
            AnimatedLayerNode* node = new AnimatedLayerNode(this,widget);
            mChildItems.push_back(node);
            
            mOutOfDate = true;
            
            return node;
        }
        
    
    void AnimatedLayer::destroyRootItemNode(MyGUI::ILayerNode* _node) {
        Base::destroyRootItemNode(_node);
    }
}
}
