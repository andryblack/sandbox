#include "sb_mygui_image_widget.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "MyGUI_RenderItem.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::ImageWidget,MyGUI::Widget)

namespace Sandbox {
    namespace mygui {
        
        
        
        ImageWidget::ImageWidget() {
            
        }
        
        ImageWidget::~ImageWidget() {
            
        }
        
        void ImageWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "Image") {
                setImage(RenderManager::getInstance().getImage(_value.c_str()));
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