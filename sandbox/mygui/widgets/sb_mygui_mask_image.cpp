#include "sb_mygui_mask_image.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "MyGUI_RenderItem.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskImageWidget,Sandbox::mygui::ImageWidgetBase)

namespace Sandbox {
    namespace mygui {
        
        
       
        MaskImageWidget::MaskImageWidget() {
            
        }
        
        MaskImageWidget::~MaskImageWidget() {
            
        }
        
        void MaskImageWidget::setShader(const ShaderPtr &s ) {
             m_shader = s;
             update_shader();
             _updateView();
        }
        
        void MaskImageWidget::update_shader() {
            if (m_shader && getImage() && getImage()->GetTexture()) {
                TexturePtr texture = getImage()->GetTexture();
                ShaderVec2UniformPtr u = m_shader->GetVec2Uniform("texture_size");
                if (u) {
                    u->SetValue(Vector2f(texture->GetWidth(),texture->GetHeight()));
                }
                u = m_shader->GetVec2Uniform("orig_texture_size");
                if (u) {
                    u->SetValue(Vector2f(texture->GetOriginalWidth(),texture->GetOriginalHeight()));
                }
            }
        }
        
        void MaskImageWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "Shader") {
                size_t delim_pos = _value.find(';');
                if (delim_pos!=_value.npos) {
                    sb::string vert = _value.substr(0,delim_pos);
                    sb::string frag = _value.substr(delim_pos+1);
                    setShader(RenderManager::getInstance().resources()->GetShader(vert.c_str(), frag.c_str()));
                }
            }
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        
        
    }
}