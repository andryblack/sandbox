#ifndef SB_MYGUI_WIDGET_RENDER_H_INCLUDED
#define SB_MYGUI_WIDGET_RENDER_H_INCLUDED

#include "MyGUI_Widget.h"
#include "sb_image.h"
#include "widgets/sb_mygui_cached_widget.h"

namespace MyGUI {
    class SharedLayerNode;
    class SharedLayer;
}

namespace Sandbox {
    
    namespace mygui {
        
        class RenderTargetImpl;
    
        class WidgetRender : public CachedWidget {
            MYGUI_RTTI_DERIVED( WidgetRender )
        private:
            typedef unsigned int counter_t;
            mutable counter_t   counter;
        public:
            WidgetRender(MyGUI::IntSize size);
            ~WidgetRender();
            

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
            ImagePtr    m_image;
        };

        typedef sb::intrusive_ptr<WidgetRender> WidgetRenderPtr;
        
    }
    
}

#endif /*SB_MYGUI_WIDGET_RENDER_H_INCLUDED*/
