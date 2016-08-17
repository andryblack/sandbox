//
//  sb_mygui_scene_widget.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 02/02/14.
//
//

#ifndef SB_MYGUI_IMAGE_WIDGET_H_INCLUDED
#define SB_MYGUI_IMAGE_WIDGET_H_INCLUDED

#include "meta/sb_meta.h"
#include "sb_rt_scene.h"
#include "MyGUI_Widget.h"
#include "MyGUI_SubSkin.h"
#include "MyGUI_MainSkin.h"
#include "MyGUI_CommonStateInfo.h"
#include "sb_image.h"
#include "sb_shader.h"
#include "sb_rect.h"
#include <ghl_render.h>

namespace Sandbox {
    
    namespace mygui {
        
        class ImageWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( ImageWidget )
        public:
            ImageWidget();
            ~ImageWidget();
            
            void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            virtual void setImage(const ImagePtr& img) { m_image = img;  }
            const ImagePtr& getImage() const { return m_image; }
            
        private:
            ImagePtr    m_image;
        };
    }
}

#endif /* SB_MYGUI_IMAGE_WIDGET_H_INCLUDED */



