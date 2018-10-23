//
//  sb_mygui_animated_widget.h
//  Sandbox
//
//
//

#ifndef _SB_MYGUI_ANIMATED_WIDGET_H_INCLUDED_
#define _SB_MYGUI_ANIMATED_WIDGET_H_INCLUDED_

#include "MyGUI_Widget.h"
#include "meta/sb_meta.h"
#include "sb_vector2.h"
#include "sb_container.h"
#include "MyGUI_SharedLayerNode.h"
#include "sb_threads_mgr.h"
#include "../sb_mygui_animated_layer.h"

namespace Sandbox {
    
    namespace mygui {
        class RenderTargetImpl;
        class AnimatedLayerNode;
        
        class AnimatedWidget : public MyGUI::Widget, public AnimatedLayerProxy {
            MYGUI_RTTI_DERIVED( AnimatedWidget )
        public:
            AnimatedWidget();
            ~AnimatedWidget();
            
            virtual MyGUI::ILayerNode* createChildItemNode(MyGUI::ILayerNode* _node);
            
            const TransformModificatorPtr& GetTransform() const;
            const ColorModificatorPtr& GetColor() const;
            const ThreadsMgrPtr& GetThread() const;
            
            void SetOrigin(const Vector2f& o);
            const Vector2f& GetOrigin() const { return m_origin; }
            
            virtual bool renderNodeToTarget(MyGUI::IRenderTarget* rt,MyGUI::LayerNode* node,bool update);
            virtual void DrawContent(MyGUI::IRenderTarget* rt,MyGUI::LayerNode* node,bool update);
            
            bool getNeedUpdate() const { return m_thread; }
            void setNeedUpdate(bool need);
        protected:
            void initialiseOverride();
            void shutdownOverride();
            void detachFromLayer();
            
            virtual void setPropertyOverride(const std::string& _key, const std::string& _value);
            virtual void Update(float dt);
            
            mutable TransformModificatorPtr     m_transform;
            mutable ColorModificatorPtr         m_color;
            mutable ThreadsMgrPtr               m_thread;
            Vector2f            m_origin;
        };
        
    }
    
}

#endif /* _SB_MYGUI_ANIMATED_WIDGET_H_INCLUDED_ */
