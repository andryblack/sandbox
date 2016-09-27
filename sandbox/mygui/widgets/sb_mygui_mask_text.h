//
//  sb_mygui_scene_widget.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 02/02/14.
//
//

#ifndef __Sandbox__sb_mygui_mask_text__
#define __Sandbox__sb_mygui_mask_text__

#include "meta/sb_meta.h"
#include "sb_rt_scene.h"
#include "sb_mygui_text_widget.h"
#include "MyGUI_SubSkin.h"
#include "MyGUI_MainSkin.h"
#include "MyGUI_CommonStateInfo.h"
#include "sb_image.h"
#include "sb_shader.h"
#include "sb_rect.h"
#include <ghl_render.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_string.h>

namespace Sandbox {
    
    namespace mygui {
                   
        
        
        class MaskTextWidget : public TextWidget {
            MYGUI_RTTI_DERIVED( MaskTextWidget )
        public:
            MaskTextWidget();
            ~MaskTextWidget();
            
            void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            void setPassImage(const sb::string& pass, const ImagePtr& img);
            const ImagePtr& getPassImage(const sb::string& pass) const;
            
            void setShader(const ShaderPtr& s);
            const ShaderPtr& getShader() const { return m_shader; }
            
        protected:
            void updateData();
        private:
            sb::map<sb::string,ImagePtr>    m_pass_image;
            ShaderPtr   m_shader;
        };
    }
}

#endif /* defined(__Sandbox__sb_mygui_mask_text__) */



