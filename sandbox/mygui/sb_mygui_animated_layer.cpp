#include "sb_mygui_animated_layer.h"
#include "sb_mygui_render.h"
#include "sb_graphics.h"
#include "widgets/sb_mygui_animated_widget.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedLayer, MyGUI::OverlappedLayer)
SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedLayerNode, MyGUI::LayerNode)


namespace Sandbox {
    
    namespace mygui {
    
        AnimatedLayerNode::AnimatedLayerNode( ILayerNode* _parent , MyGUI::Widget* widget ) : MyGUI::LayerNode(_parent->getLayer(),_parent), m_widget(widget) {
        }
        
        AnimatedLayerNode::AnimatedLayerNode(MyGUI::ILayer* layer , MyGUI::Widget* widget) : MyGUI::LayerNode(layer,0),m_widget(0){
            if (widget && MyGUI::utility::parseBool(widget->getUserString("animated"))) {
                m_widget = widget;
            }
        }
        
        AnimatedLayerNode::~AnimatedLayerNode() {
           
        }
        
        void AnimatedLayerNode::renderToTarget(MyGUI::IRenderTarget* _target, bool _update) {
            if (m_widget) {
                m_widget->renderNodeToTarget(_target, this, _update);
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
