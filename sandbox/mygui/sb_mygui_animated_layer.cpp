#include "sb_mygui_animated_layer.h"
#include "sb_mygui_render.h"
#include "sb_graphics.h"


SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedLayer, MyGUI::OverlappedLayer)
SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedLayerNode, MyGUI::LayerNode)


namespace Sandbox {
    
    namespace mygui {
    
    AnimatedLayerNode::AnimatedLayerNode(MyGUI::ILayer* _layer, MyGUI::ILayerNode* _parent) : MyGUI::LayerNode(_layer,_parent) {
    }
        
    
    void AnimatedLayerNode::renderToTarget(MyGUI::IRenderTarget* _target, bool _update) {
        Sandbox::Graphics& g(*Sandbox::mygui::RenderManager::getInstance().graphics());
        
        Transform2d tr = g.GetTransform();
        Color clr = g.GetColor();
        if (m_transform) {
            m_transform->Apply(g);
        }
        if (m_color) {
            m_color->Apply(g);
        }
        
       Base::renderToTarget(_target, _update);
        
        if (m_color) {
            g.SetColor(clr);
        }
        if (m_transform) {
            g.SetTransform(tr);
        }

        
    }
        
        MyGUI::ILayerNode* AnimatedLayerNode::createChildItemNode()
        {
            LayerNode* layer = new AnimatedLayerNode(mLayer, this);
            mChildItems.push_back(layer);
            
            mOutOfDate = true;
            
            return layer;
        }
    
    
        void AnimatedLayerNode::SetTransformModificator(const TransformModificatorPtr& ptr) {
            m_transform = ptr;
        }
        TransformModificatorPtr AnimatedLayerNode::GetTransformModificator() {
            if (!m_transform) {
                m_transform.reset(new TransformModificator());
            }
            return m_transform;
        }
        
        void AnimatedLayerNode::SetColorModificator(const ColorModificatorPtr& ptr) {
            m_color = ptr;
        }
        ColorModificatorPtr AnimatedLayerNode::GetColorModificator() {
            if (!m_color) {
                m_color.reset(new ColorModificator());
            }
            return m_color;
        }

        
        
    AnimatedLayer::AnimatedLayer() {
        
    }
    
    AnimatedLayer::~AnimatedLayer() {
        
    }
    
    MyGUI::ILayerNode* AnimatedLayer::createChildItemNode() {
        // создаем рутовый айтем
        AnimatedLayerNode* node = new AnimatedLayerNode(this);
        mChildItems.push_back(node);
        
        mOutOfDate = true;
        
        return node;
    }
        
    
    void AnimatedLayer::destroyChildItemNode(MyGUI::ILayerNode* _node) {
        Base::destroyChildItemNode(_node);
    }
        
}
}
