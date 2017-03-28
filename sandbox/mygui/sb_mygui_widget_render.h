#ifndef SB_MYGUI_WIDGET_RENDER_H_INCLUDED
#define SB_MYGUI_WIDGET_RENDER_H_INCLUDED

#include "MyGUI_Widget.h"
#include "sb_image.h"

namespace MyGUI {
    class SharedLayerNode;
}

namespace Sandbox {
    
    namespace mygui {
        
        class RenderTargetImpl;
    
        class WidgetRender : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( WidgetRender )
        private:
            typedef unsigned int counter_t;
            mutable counter_t   counter;
        public:
            WidgetRender(MyGUI::IntSize size);
            ~WidgetRender();
                        
            virtual void onWidgetCreated(MyGUI::Widget* _widget);
            
            virtual void addChildItem(LayerItem* _item);
            virtual void removeChildItem(LayerItem* _item);
            virtual void addChildNode(LayerItem* _item);
            virtual void removeChildNode(LayerItem* _item);

            ImagePtr getImage() const { return m_image; }
            void render();
            
            void add_ref() const {
                ++counter;
            }
            void remove_ref() const;
        protected:
            void initialiseOverride();
            void shutdownOverride();
            
        private:
            RenderTargetImpl*        m_target;
            
            MyGUI::SharedLayerNode*  m_replaced_layer;
            std::string m_texture_name;
            ImagePtr    m_image;
        };

        typedef sb::intrusive_ptr<WidgetRender> WidgetRenderPtr;
        
    }
    
}

#endif /*SB_MYGUI_WIDGET_RENDER_H_INCLUDED*/
