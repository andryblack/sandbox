#ifndef _SB_MYGUI_ANIMATED_LAYER_H_INCLUDED_
#define _SB_MYGUI_ANIMATED_LAYER_H_INCLUDED_

#include "MyGUI_OverlappedLayer.h"
#include "MyGUI_LayerNode.h"
#include "MyGUI_ILayer.h"
#include "sb_container.h"

namespace Sandbox {
    
    namespace mygui {
    
        class AnimatedWidget;
    class AnimatedLayerNode : public MyGUI::LayerNode {
        MYGUI_RTTI_DERIVED( AnimatedLayerNode )
    public:
        explicit AnimatedLayerNode(MyGUI::ILayerNode* _parent , AnimatedWidget* widget);
        explicit AnimatedLayerNode(MyGUI::ILayer* layer , MyGUI::Widget* widget);
        ~AnimatedLayerNode();
        
        virtual void renderToTarget(MyGUI::IRenderTarget* _target, bool _update);
        
    protected:
        AnimatedWidget* m_widget;
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
