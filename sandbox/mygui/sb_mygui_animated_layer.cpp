#include "sb_mygui_animated_layer.h"
#include "sb_mygui_render.h"
#include "sb_graphics.h"
#include "sb_mygui_animated_widget.h"
#include "widgets/sb_mygui_animated_widget.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedLayer, MyGUI::OverlappedLayer)
SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedLayerNode, MyGUI::LayerNode)


namespace Sandbox {
    
    namespace mygui {
    
        AnimatedLayerNode::AnimatedLayerNode( ILayerNode* _parent , AnimatedWidget* widget ) : MyGUI::LayerNode(_parent->getLayer(),_parent), m_widget(widget) {
        }
        
        AnimatedLayerNode::AnimatedLayerNode(MyGUI::ILayer* layer , MyGUI::Widget* widget) : MyGUI::LayerNode(layer,0),m_widget(0){
            m_widget = widget->castType<AnimatedWidget>(false);
        }
        
        AnimatedLayerNode::~AnimatedLayerNode() {
           
        }
        
        void AnimatedLayerNode::renderToTarget(MyGUI::IRenderTarget* _target, bool _update) {
            if (m_widget) {
                m_widget->renderToTarget(_target, this, _update);
            } else {
                Base::renderToTarget(_target, _update);
            }
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
