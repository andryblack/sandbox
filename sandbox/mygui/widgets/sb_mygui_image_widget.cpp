#include "sb_mygui_image_widget.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "MyGUI_RenderItem.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::ImageWidgetBase,MyGUI::Widget)
SB_META_DECLARE_OBJECT(Sandbox::mygui::ImageWidget,Sandbox::mygui::ImageWidgetBase)

namespace Sandbox {
    namespace mygui {
        
        
        
        ImageWidgetBase::ImageWidgetBase() {
            
        }
        
        ImageWidgetBase::~ImageWidgetBase() {
            
        }
        
        void ImageWidgetBase::setImage(const ImagePtr& img) {
            m_image = img;
            if (img && img->GetTexture()) {
                img->GetTexture()->SetFiltered(true);
            }
        }
        void ImageWidgetBase::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "Image") {
                setImage(RenderManager::getInstance().getImage(_value.c_str()));
            } else if (_key == "Texture") {
                setTexture(_value);
            }
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        void ImageWidgetBase::setTexture(const std::string& filename ) {
            setImage(RenderManager::getInstance().resources()->GetImage(filename.c_str(), false));
        }
        
        void ImageWidget::setImage(const ImagePtr &img) {
            ImageWidgetBase::setImage(img);
            if (img && img->GetTexture()) {
                Sandbox::TexturePtr texture = img->GetTexture();
                MyGUI::ITexture* tex = RenderManager::getInstance().getTexture(texture->GetName());
                if (!tex) {
                    RenderManager::getInstance().wrapTexture(texture);
                }
                _setTextureName(texture->GetName());
                float iw = 1.0f / float(texture->GetRealWidth());
                float ih = 1.0f / float(texture->GetRealHeight());
                if (nullptr != getSubWidgetMain())
                    getSubWidgetMain()->_setUVSet(MyGUI::FloatRect(img->GetTextureX()*iw,
                                                                   img->GetTextureY()*ih,
                                                                   (img->GetTextureX()+img->GetTextureW())*iw,
                                                                   (img->GetTextureY()+img->GetTextureH())*ih));
            } else {
                _setTextureName("");
            }
            
        }
        
        
        
        
    }
}