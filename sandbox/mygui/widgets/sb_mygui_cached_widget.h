//
//  sb_mygui_cached_widget.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 30/01/14.
//
//

#ifndef __Sandbox__sb_mygui_cached_widget__
#define __Sandbox__sb_mygui_cached_widget__

#include "MyGUI_Widget.h"
#include "meta/sb_meta.h"
#include "sb_vector2.h"
#include "../sb_mygui_animated_layer.h"

namespace Sandbox {
    
    namespace mygui {
        class RenderTargetImpl;
        
        class CachedWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( CachedWidget )
        public:
            CachedWidget();
            ~CachedWidget();
            
            virtual void onWidgetCreated(MyGUI::Widget* _widget);
            
            virtual void addChildItem(LayerItem* _item);
            virtual void removeChildItem(LayerItem* _item);
            virtual void addChildNode(LayerItem* _item);
            virtual void removeChildNode(LayerItem* _item);

            void setSubOffset(const Sandbox::Vector2f& sub) {
                m_suboffset = sub;
            }
            
        protected:
            void initialiseOverride();
            void shutdownOverride();
            virtual void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            virtual void doRenderToTarget(MyGUI::IRenderTarget* rt);
        private:
            RenderTargetImpl*        m_target;
            
            AnimatedLayerNode*  m_replaced_layer;
            void frameEntered(float dt);
            std::string m_texture_name;
            
            bool    m_render_content;
            Sandbox::Vector2f m_suboffset;
        };
        
    }
    
}

#endif /* defined(__Sandbox__sb_mygui_cached_widget__) */
