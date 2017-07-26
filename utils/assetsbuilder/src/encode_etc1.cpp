#include <ghl_image.h>
#include <ghl_data.h>

#include <rg_etc1.h>
#include <cstring>
#include "tasks_pool.h"

static void read_pixels(GHL::Byte* pixels,const GHL::Image* img,GHL::UInt32 bx,GHL::UInt32 by) {
    GHL::UInt32 sx = bx * 4;
    GHL::UInt32 sy = by * 4;
    GHL::UInt32 iw = img->GetWidth();
    GHL::UInt32 ih = img->GetHeight();
    for (GHL::UInt32 iy=0;iy<4;++iy) {
        GHL::UInt32 y = sy + iy;
        if (y>=ih) {
            for (GHL::UInt32 ix=0;ix<4;++ix) {
                pixels[iy*4*4+ix*4+0] = 0x00;
                pixels[iy*4*4+ix*4+1] = 0x00;
                pixels[iy*4*4+ix*4+2] = 0x00;
            }
        } else {
            const GHL::Byte* l = img->GetData()->GetData() + (y*iw*3+sx*3);
            for (GHL::UInt32 ix=0;ix<4;++ix) {
                GHL::UInt32 x = sx + ix;
                if (x>=iw) {
                    pixels[iy*4*4+ix*4+0] = 0x00;
                    pixels[iy*4*4+ix*4+1] = 0x00;
                    pixels[iy*4*4+ix*4+2] = 0x00;
                } else {
                    pixels[iy*4*4+ix*4+0] = l[ix*3+0];
                    pixels[iy*4*4+ix*4+1] = l[ix*3+1];
                    pixels[iy*4*4+ix*4+2] = l[ix*3+2];
                }
            }
        }
        
    }
    
}

//struct pkm_file_header_t {
//    GHL::Byte magic[4];
//    GHL::Byte version[2];
//    GHL::UInt16 format;
//    GHL::UInt16 width;
//    GHL::UInt16 height;
//    GHL::UInt16 original_width;
//    GHL::UInt16 original_height;
//};

static rg_etc1::etc1_pack_params params;


class ETC1Task : public Task {
private:
    GHL::Byte* m_out;
    const GHL::Image* m_img;
    GHL::UInt32 m_y;
    GHL::UInt32 m_blocks_w;
    GHL::Byte m_pixels[4*4*4];
public:
    explicit ETC1Task(GHL::Byte* out,const GHL::Image* img,GHL::UInt32 y,GHL::UInt32 blocks_w,GHL::Byte* pixels) {
        m_out = out;
        m_img = img;
        m_y = y;
        m_blocks_w = blocks_w;
        for (GHL::UInt32 i=0;i<4*4;++i) {
            m_pixels[i*4+3]=0xff;
        }
        memcpy(m_pixels, pixels, sizeof(m_pixels));
    }
    bool RunImpl() {
        for (GHL::UInt32 x=0;x<m_blocks_w;++x) {
            read_pixels(m_pixels,m_img,x,m_y);
            rg_etc1::pack_etc1_block(m_out,reinterpret_cast<const unsigned int*>(m_pixels),params);
            m_out += 8;
        }
        return true;
    }
};

#define big_word(v) GHL::Byte((((v)&0xff00)>>8)),GHL::Byte(((v)&0x00ff))

void encode_etc1_set_quality(int q) {
    if (q == 0) {
        params.m_quality = rg_etc1::cLowQuality;
    } else if (q==1) {
        params.m_quality = rg_etc1::cMediumQuality;
    } else {
        params.m_quality = rg_etc1::cHighQuality;
    }
}

GHL::Data* encode_etc1(TasksPool* pool,const GHL::Image* img,bool with_header) {
    if (img->GetFormat() != GHL::IMAGE_FORMAT_RGB)
        return 0;
    GHL::UInt32 w = img->GetWidth();
    GHL::UInt32 h = img->GetHeight();
    
    GHL::UInt32 blocks_w = (w+3)/4;
    GHL::UInt32 blocks_h = (h+3)/4;
    
    
    static bool inited = false;
    if (!inited) {
        rg_etc1::pack_etc1_block_init();
        inited = true;
    }
    
    GHL::UInt32 out_data_size = (blocks_w*blocks_h*8) + (with_header ? 16:0);
    GHL::Data* res = GHL_CreateData(out_data_size,true,0);
    GHL::Byte* out_pos = res->GetDataPtr();
    if (with_header) {
        GHL::Byte header[16] = {
            'P','K','M',' ','1','0',big_word(blocks_w*4),big_word(blocks_h*4),big_word(w),big_word(h)
        };
        memcpy(res->GetDataPtr(),&header,sizeof(header));
        out_pos+=sizeof(header);
    }
    
    
    GHL::Byte pixels[4*4*4];
    for (GHL::UInt32 i=0;i<4*4;++i) {
        pixels[i*4+3]=0xff;
    }
    for (GHL::UInt32 y=0;y<blocks_h;++y) {
        if (pool) {
            pool->AddTask(TaskPtr(new ETC1Task(out_pos,img,y,blocks_w,pixels)));
            out_pos += 8 * blocks_w;
        } else {
            for (GHL::UInt32 x=0;x<blocks_w;++x) {
                read_pixels(pixels,img,x,y);
                rg_etc1::pack_etc1_block(out_pos,reinterpret_cast<const unsigned int*>(pixels),params);
                out_pos += 8;
            }
        }
        
    }
    if (pool) {
        while (!pool->Completed()) {
            if (!pool->Process()) {
                if (res) res->Release();
                res = 0;
                break;
            }
        }
    }
    return res;
}
