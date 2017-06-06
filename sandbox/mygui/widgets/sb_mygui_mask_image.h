//
//  sb_mygui_scene_widget.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 02/02/14.
//
//

#ifndef __Sandbox__sb_mygui_mask_image__
#define __Sandbox__sb_mygui_mask_image__

#include "meta/sb_meta.h"
#include "sb_mygui_image_widget.h"
#include "sb_shader.h"
#include "sb_rect.h"
#include <ghl_render.h>

namespace Sandbox {
    
    namespace mygui {
        
        
                
        
        class MaskImageWidget : public ImageWidgetBase {
            MYGUI_RTTI_DERIVED( MaskImageWidget )
        public:
            MaskImageWidget();
            ~MaskImageWidget();
            
            void setImage(const ImagePtr& img) { ImageWidgetBase::setImage(img); update_shader(); }
            
            void setShader(const ShaderPtr& s);
            
        protected:
            void update_shader();
        };
    }
}

#endif /* defined(__Sandbox__sb_mygui_mask_image__) */



