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
#include "MyGUI_MainSkin.h"
#include "MyGUI_CommonStateInfo.h"
#include "sb_image.h"
#include "sb_shader.h"
#include "sb_rect.h"
#include <ghl_render.h>

namespace Sandbox {
    
    namespace mygui {
        
        
        class MaskSubSkin : public MyGUI::SubSkin {
            MYGUI_RTTI_DERIVED( MaskSubSkin )
        public:
            MaskSubSkin();
            virtual void doRender(MyGUI::IRenderTarget* _target);
        private:
            sb::vector<GHL::Vertex2Tex> m_vdata;
        };
        
        class MaskSetSubSkinState : public MyGUI::SubSkinStateInfo {
            MYGUI_RTTI_DERIVED( MaskSetSubSkinState )
        public:
            MaskSetSubSkinState();
            MyGUI::ITexture* get_texture() { return m_texture; }
        protected:
            virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version);
        private:
            MyGUI::ITexture* m_texture;
        };
        
        class MaskSetSubSkin : public MyGUI::MainSkin {
            MYGUI_RTTI_DERIVED( MaskSetSubSkin )
        public:
            MaskSetSubSkin();
            
            virtual void doRender(MyGUI::IRenderTarget* _target);
            virtual void setStateData(MyGUI::IStateInfo* _data);
            MyGUI::ITexture* getTexture() {
                if (m_texture) {
                    return m_texture;
                }
                return MyGUI::MainSkin::getTexture();
            }
        private:
            MyGUI::ITexture* m_texture;
        };
        
        
        class MaskImageWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( MaskImageWidget )
        public:
            MaskImageWidget();
            ~MaskImageWidget();
            
            void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            void setImage(const ImagePtr& img) { m_image = img; update_shader(); }
            const ImagePtr& getImage() const { return m_image; }
            
            void setShader(const ShaderPtr& s);
            const ShaderPtr& getShader() const { return m_shader; }
            
        protected:
            void update_shader();
        private:
            ImagePtr    m_image;
            ShaderPtr   m_shader;
        };
    }
}

#endif /* defined(__Sandbox__sb_mygui_mask_image__) */



