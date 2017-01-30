#ifndef _SB_MYGUI_ANIMATED_LAYER_H_INCLUDED_
#define _SB_MYGUI_ANIMATED_LAYER_H_INCLUDED_

#include "MyGUI_OverlappedLayer.h"
#include "MyGUI_LayerNode.h"
#include "MyGUI_ILayer.h"
#include "sb_container.h"

namespace Sandbox {
    
    namespace mygui {
        
        class AnimatedLayerNode;
        
        class AnimatedLayerProxy {
        private:
            AnimatedLayerNode*  m_node;
        public:
            AnimatedLayerProxy();
            virtual ~AnimatedLayerProxy();
            virtual bool renderNodeToTarget(MyGUI::IRenderTarget* rt,MyGUI::LayerNode* node,bool update) = 0;
            void attachLayerNode(AnimatedLayerNode* node);
            void resetLayerNode(AnimatedLayerNode* node);
        };
        
        class AnimatedLayer;
    
        class AnimatedLayerNode : public MyGUI::LayerNode {
            MYGUI_RTTI_DERIVED( AnimatedLayerNode )
        public:
            explicit AnimatedLayerNode(ILayerNode* _parent , AnimatedLayerProxy* proxy);
            explicit AnimatedLayerNode(AnimatedLayer* _layer, MyGUI::Widget* _widget);
            ~AnimatedLayerNode();
            
            virtual void renderToTarget(MyGUI::IRenderTarget* _target, bool _update);
            
            virtual void detachLayerItem(MyGUI::ILayerItem* _root);
            
            void resetProxy();
        protected:
            AnimatedLayerProxy* m_proxy;
        };
        
        class AnimatedLayer : public MyGUI::OverlappedLayer {
            MYGUI_RTTI_DERIVED( AnimatedLayer )
        
        public:
            AnimatedLayer();
            virtual ~AnimatedLayer();
        
            virtual MyGUI::ILayerNode* createRootItemNode(MyGUI::Widget* widget);
            virtual void destroyRootItemNode(MyGUI::ILayerNode* _node);
            
        };
    }
}

#endif /*_SB_MYGUI_ANIMATED_LAYER_H_INCLUDED_*/
