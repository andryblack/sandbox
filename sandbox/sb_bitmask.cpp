#include "sb_bitmask.h"
#include <ghl_image.h>

namespace Sandbox {
    
    Bitmask::Bitmask(GHL::Image* img) {
        m_width = img->GetWidth();
        m_height = img->GetHeight();
        size_t bwidth = (m_width + bits_count - 1) / bits_count;
        m_data.resize(bwidth*m_height,0);
        const GHL::Byte* src = img->GetDataPtr();
        size_t bpp = 1;
        switch (img->GetFormat()) {
            case GHL::IMAGE_FORMAT_RGBA:
                bpp = 4;
                break;
            default:
                return;
                break;
        }
        size_t line_size = m_width * bpp;
        for (size_t y=0;y<m_height;++y) {
            size_t x = 0;
            const GHL::Byte* line = src;
            while (x<m_width) {
                store_t s = 0;
                for (size_t i=0;i<bits_count;++i) {
                    if (x<m_width) {
                        if (line[3]>0x04) {
                            s = s | (1<<i);
                        }
                    }
                    ++x;
                    line+=bpp;
                }
                m_data[(x-1)/bits_count+y*bwidth] = s;
            }
            src += line_size;
        }
    }
    bool Bitmask::Get(int x,int y) const {
        if (x<0)
            return false;
        if (y<0)
            return false;
        if (x>=m_width)
            return false;
        if (y>=m_height)
            return false;
        size_t bwidth = (m_width + bits_count - 1) / bits_count;
        size_t xx = x / bits_count;
        size_t xi = x % bits_count;
        return m_data[xx+y*bwidth] & (1<<xi);
    }
}