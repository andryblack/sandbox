#include "sb_mygui_mask_text.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "MyGUI_RenderItem.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskTextWidget,Sandbox::mygui::TextWidget)

namespace Sandbox {
    namespace mygui {
        
        
        
        MaskTextWidget::MaskTextWidget() {
            
        }
        
        MaskTextWidget::~MaskTextWidget() {
            
        }
        
        void MaskTextWidget::setShader(const ShaderPtr &s ) {
             m_shader = s;
             update_shader();
             _updateView();
        }
        
        void MaskTextWidget::setPassImage(const sb::string& pass, const ImagePtr& img) {
            m_pass_image[pass] = img;
        }
        static const ImagePtr empty_image;
        const ImagePtr& MaskTextWidget::getPassImage(const sb::string& pass) const {
            sb::map<sb::string,ImagePtr>::const_iterator it = m_pass_image.find(pass);
            if (it!=m_pass_image.end()) {
                return it->second;
            }
            return empty_image;
        }
        
        void MaskTextWidget::update_shader() {
            
        }
        
        void MaskTextWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key.substr(0,7) == "Texture") {
                setPassImage(_key.substr(7),RenderManager::getInstance().resources()->GetImage(_value.c_str(), false));
            } else if (_key == "Shader") {
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