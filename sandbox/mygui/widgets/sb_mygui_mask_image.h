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
        
        
                
        
        class MaskImageWidget : public ImageWidget {
            MYGUI_RTTI_DERIVED( MaskImageWidget )
        public:
            MaskImageWidget();
            ~MaskImageWidget();
            
            void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            void setImage(const ImagePtr& img) { ImageWidget::setImage(img); update_shader(); }
            
            void setShader(const ShaderPtr& s);
            const ShaderPtr& getShader() const { return m_shader; }
            
        protected:
            void update_shader();
        private:
            ShaderPtr   m_shader;
        };
    }
}

#endif /* defined(__Sandbox__sb_mygui_mask_image__) */



