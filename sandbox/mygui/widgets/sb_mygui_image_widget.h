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
#include "sb_mygui_shader_widget.h"
#include "MyGUI_SubSkin.h"
#include "MyGUI_MainSkin.h"
#include "MyGUI_CommonStateInfo.h"
#include "sb_image.h"
#include "sb_shader.h"
#include "sb_rect.h"
#include <ghl_render.h>

namespace Sandbox {
    
    namespace mygui {
        
        class ImageWidgetBase : public ShaderWidget {
            MYGUI_RTTI_DERIVED( ImageWidgetBase )
        public:
            ImageWidgetBase();
            ~ImageWidgetBase();
            
            void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            virtual void setImage(const ImagePtr& img);
            const ImagePtr& getImage() const { return m_image; }
            
            
            void setTexture(const std::string& filename );
            
            
        private:
            ImagePtr    m_image;
        };
        
        class ImageWidget : public ImageWidgetBase {
            MYGUI_RTTI_DERIVED( ImageWidget )
        protected:
            TexturePtr  m_mask;
        public:
            virtual void setImage(const ImagePtr& img);
            void setMaskTexture( const TexturePtr& tex );
            
            virtual void DrawContent(MyGUI::IRenderTarget* rt,MyGUI::LayerNode* node,bool update);
            
        };
    }
}

#endif /* SB_MYGUI_IMAGE_WIDGET_H_INCLUDED */



