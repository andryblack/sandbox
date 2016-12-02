#include "sb_container_mask.h"

SB_META_DECLARE_OBJECT(Sandbox::ContainerMask, Sandbox::Container)

namespace Sandbox {
    
    ContainerMask::ContainerMask() : m_mode( MASK_MODE_NONE ) {
    }
    
    void ContainerMask::Draw(Graphics& g) const {
        if (m_image) {
            MaskMode m = g.GetMaskMode();
            TexturePtr t = g.GetMaskTexture();
            Transform2d tr = g.GetMaskTransform();
            g.SetMask(m_mode, *m_image, m_rect);
            Container::Draw(g);
            g.SetMask(m, t, tr);
        } else {
            Container::Draw(g);
        }
    }
    
    void ContainerMask::CalcRect() {
        if (m_image) {
            m_rect = Rectf(-m_image->GetHotspot().x*m_image->GetWidth()/m_image->GetTextureW(),
                       -m_image->GetHotspot().y*m_image->GetHeight()/m_image->GetTextureH(),
                       m_image->GetWidth(),
                       m_image->GetHeight());
        }
    }
    
    void ContainerMask::SetImage(const ImagePtr &img) {
        m_image = img;
    }
    
    void ContainerMask::SetRect(const Rectf &r) {
        m_rect = r;
    }
    
}
