#include "sb_image.h"
#include "sb_bitmask.h"

namespace Sandbox {
    
    bool Image::CheckBit(int x,int y,Resources* resources) {
        float src_w = m_rotated ? m_src_h : m_src_w;
        float src_h = m_rotated ? m_src_w : m_src_h;
        x+=int(m_hotspot.x*m_width/src_w);
        if (x<0)
            return false;
        y+=int(m_hotspot.y*m_height/src_h);
        if (y<0)
            return false;
        if (x>m_width)
            return false;
        if (y>m_height)
            return false;
        if (m_rotated) {
            float ox = x;
            float oy = y;
            x = int(m_src_x + oy * m_src_w / m_height);
            y = int(m_src_y + m_src_h - ox * m_src_h / m_width);
        } else {
            x = int(m_src_x + x * m_src_w / m_width);
            y = int(m_src_y + y * m_src_h / m_height);
        }
        if (!m_texture) return false;
        BitmaskPtr bm = m_texture->GetBitmask(resources);
        if (!bm)
            return false;
        return bm->Get(x, y);
    }
    
    sb::intrusive_ptr<Image> Image::Clone() const {
        return sb::intrusive_ptr<Image>(new Image(*this));
    }
}
