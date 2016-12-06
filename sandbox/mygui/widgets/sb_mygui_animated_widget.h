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

namespace Sandbox {
    
    namespace mygui {
        class RenderTargetImpl;
        
        class AnimatedWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( AnimatedWidget )
        public:
            AnimatedWidget();
            ~AnimatedWidget();
            
            virtual void attachToLayerItemNode(MyGUI::ILayerNode* _node, bool _deep);
            
            const TransformModificatorPtr& GetTransform() const { return m_transform; }
            const ColorModificatorPtr& GetColor() const { return m_color; }
            const ThreadsMgrPtr& GetThread() const { return m_thread; }
            
            void SetOrigin(const Vector2f& o) { m_origin = o; }
            const Vector2f& GetOrigin() const { return m_origin; }
        protected:
            void initialiseOverride();
            void shutdownOverride();
            
            virtual void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            TransformModificatorPtr     m_transform;
            ColorModificatorPtr         m_color;
            ThreadsMgrPtr               m_thread;
            Vector2f            m_origin;
         private:
            void frameEntered(float dt);
        };
        
    }
    
}

#endif /* _SB_MYGUI_ANIMATED_WIDGET_H_INCLUDED_ */
