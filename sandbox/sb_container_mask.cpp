#include "sb_container_mask.h"

SB_META_DECLARE_OBJECT(Sandbox::ContainerMask, Sandbox::Container)

namespace Sandbox {
    
    ContainerMask::ContainerMask() : m_mode( MASK_MODE_NONE ) {
    }
    
    void ContainerMask::Draw(Graphics& g) const {
        MaskMode m = g.GetMaskMode();
        TexturePtr t = g.GetMaskTexture();
        Transform2d tr = g.GetMaskTransform();
        g.SetMask(m_mode, m_texture, m_transform);
        Container::Draw(g);
        g.SetMask(m, t, tr);
    }
    
    void ContainerMask::SetImage(const ImagePtr &img) {
        if (img) {
            m_texture = img->GetTexture();
            m_transform = Transform2d();
            if (m_texture) {
                m_transform.scale(1.0f/m_texture->GetWidth(), 1.0f/m_texture->GetHeight());
            }
            m_transform.translate(-img->GetHotspot());
        }
    }
    
}
