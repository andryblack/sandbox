#ifndef _SB_MYGUI_ANIMATED_LAYER_H_INCLUDED_
#define _SB_MYGUI_ANIMATED_LAYER_H_INCLUDED_

#include "MyGUI_OverlappedLayer.h"
#include "MyGUI_LayerNode.h"
#include "sb_container.h"

namespace Sandbox {
    
    namespace mygui {
    
    class AnimatedLayerNode : public MyGUI::LayerNode {
        MYGUI_RTTI_DERIVED( AnimatedLayerNode )
    public:
        explicit AnimatedLayerNode(MyGUI::ILayer* _layer, MyGUI::ILayerNode* _parent = nullptr);
        virtual void renderToTarget(MyGUI::IRenderTarget* _target, bool _update);
        
        void SetTransformModificator(const TransformModificatorPtr& ptr);
        TransformModificatorPtr GetTransformModificator();
        
        void SetColorModificator(const ColorModificatorPtr& ptr);
        ColorModificatorPtr GetColorModificator();
        
    protected:
        TransformModificatorPtr     m_transform;
        ColorModificatorPtr         m_color;
    };
    
    class AnimatedLayer : public MyGUI::OverlappedLayer {
        MYGUI_RTTI_DERIVED( AnimatedLayer )
    
    public:
        AnimatedLayer();
        virtual ~AnimatedLayer();
    
        virtual MyGUI::ILayerNode* createChildItemNode();
        virtual void destroyChildItemNode(MyGUI::ILayerNode* _node);
        
    };
    }
}

#endif /*_SB_MYGUI_ANIMATED_LAYER_H_INCLUDED_*/
