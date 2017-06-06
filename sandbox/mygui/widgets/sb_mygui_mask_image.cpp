#include "sb_mygui_mask_image.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "MyGUI_RenderItem.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskImageWidget,Sandbox::mygui::ImageWidgetBase)

namespace Sandbox {
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(MaskImageWidget)
       
        MaskImageWidget::MaskImageWidget() {
            
        }
        
        MaskImageWidget::~MaskImageWidget() {
            
        }
        
        void MaskImageWidget::setShader(const ShaderPtr &s ) {
            Base::setShader(s);
             update_shader();
             _updateView();
        }
        
        void MaskImageWidget::update_shader() {
            if (getShader() && getImage() && getImage()->GetTexture()) {
                TexturePtr texture = getImage()->GetTexture();
                ShaderVec2UniformPtr u = getShader()->GetVec2Uniform("texture_size");
                if (u) {
                    u->SetValue(Vector2f(texture->GetWidth(),texture->GetHeight()));
                }
                u = getShader()->GetVec2Uniform("orig_texture_size");
                if (u) {
                    u->SetValue(Vector2f(texture->GetOriginalWidth(),texture->GetOriginalHeight()));
                }
            }
        }        
        
        
    }
}
