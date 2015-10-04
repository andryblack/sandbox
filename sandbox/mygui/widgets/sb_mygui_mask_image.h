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
#include "sb_rt_scene.h"
#include "MyGUI_Widget.h"
#include "MyGUI_SubSkin.h"
#include "sb_image.h"
#include <ghl_render.h>

namespace Sandbox {
    
    namespace mygui {
        
        class MaskSubSkin : public MyGUI::SubSkin {
            MYGUI_RTTI_DERIVED( MaskSubSkin )
        public:
            MaskSubSkin();
            virtual void doManualRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count);
        private:
            sb::vector<GHL::Vertex2Tex> m_vdata;
        };
        
        class MaskImageWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( MaskImageWidget )
        public:
            MaskImageWidget();
            ~MaskImageWidget();
            
            void initialiseOverride();
            
            void setImage(const ImagePtr& img) { m_image = img; }
            const ImagePtr& getImage() const { return m_image; }
            
        private:
            ImagePtr    m_image;
            sb::string  m_mask_skin;
        };
    }
}

#endif /* defined(__Sandbox__sb_mygui_mask_image__) */



