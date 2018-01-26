#include <utils/sb_md5.h>

extern "C" {
#include <utils/md5.h>
}

#include <utils/sb_hex.h>

#include "texture.h"

TextureSubData::TextureSubData( GHL::Image* image,const Sandbox::Recti& rect) : Texture(rect.w,rect.h), m_data(image),m_rect(rect) {
    if (m_data) {
        m_data->AddRef();
    }
}


const GHL::Image* TextureSubData::GetSubImage() {
    if (m_rect.x == 0 &&
        m_rect.y == 0 &&
        m_rect.w == m_data->GetWidth() &&
        m_rect.h == m_data->GetHeight()) {
        return m_data;
    }
    m_data = m_data->SubImage(m_rect.x, m_rect.y, m_rect.w, m_rect.h);
    m_rect.x = 0;
    m_rect.y = 0;
    return  m_data;
}

TextureSubDataPtr TextureSubData::Extract(const Sandbox::Recti &rect) {
    return TextureSubDataPtr(new TextureSubData(m_data,
                                             Sandbox::Recti(
                                                            m_rect.x+rect.x,
                                                            m_rect.y+rect.y,
                                                           rect.w,
                                                           rect.h)));
}

static size_t get_bpp( GHL::ImageFormat fmt) {
    switch (fmt) {
        case GHL::IMAGE_FORMAT_GRAY:
            return 1;
        case GHL::IMAGE_FORMAT_565:
        case GHL::IMAGE_FORMAT_4444:
            return 2;
        case GHL::IMAGE_FORMAT_RGB:
            return 3;
        case GHL::IMAGE_FORMAT_RGBA:
        case GHL::IMAGE_FORMAT_RGBX:
            return 4;
        default:
            sb_assert(false);
            break;
    }
    return 1;
}

sb::string TextureSubData::GetMD5() const {
    if (!m_data) return "nil";
    const GHL::Byte* dd = m_data->GetData()->GetData();
    size_t bpp = get_bpp(m_data->GetFormat());
    dd += (m_data->GetWidth() * m_rect.y + m_rect.x)*bpp;
    const size_t line_step = m_data->GetWidth() * bpp;
    const size_t line_size = m_rect.w * bpp;
    
    MD5_CTX ctx;
    MD5_Init(&ctx);
    
    
    for (int i=0;i<m_rect.h;++i) {
        MD5_Update(&ctx, dd, line_size);
        dd += line_step;
    }
    unsigned char sum[16];
    MD5_Final(sum, &ctx);
    return Sandbox::DataToHex(sum,16);
}

sb::string TextureSubData::GetMD5Rotated() const {
    if (!m_data) return "nil";
    const GHL::Byte* ddS = m_data->GetData()->GetData();
    size_t bpp = get_bpp(m_data->GetFormat());
    
    MD5_CTX ctx;
    MD5_Init(&ctx);
    
    for (int i=0;i<m_rect.w;++i) {
        for (int j=0;j<m_rect.h;++j) {
            const GHL::Byte* dd = ddS + ((m_rect.x + m_rect.w - i - 1) + (m_rect.y + j)*m_data->GetWidth())*bpp;
            MD5_Update(&ctx, dd, bpp);
        }
    }
    unsigned char sum[16];
    MD5_Final(sum, &ctx);
    return Sandbox::DataToHex(sum,16);
}


TextureSubData::~TextureSubData() {
    if (m_data) {
        m_data->Release();
    }
}

void TextureSubData::Free() {
    if (m_data) {
        m_data->Release();
        m_data = 0;
    }
}

bool TextureSubData::Crop() {
    if (!m_data) return false;
    if (m_data->GetFormat() != GHL::IMAGE_FORMAT_RGBA) return false;
    GHL::UInt32 min_x = width() - 1;
    GHL::UInt32 max_x = 0;
    GHL::UInt32 min_y = height() - 1;
    GHL::UInt32 max_y = 0;
    for (GHL::UInt32 y = 0; y < height(); ++y) {
        const GHL::Byte* line = m_data->GetData()->GetData() + y * width() * 4;
        bool has_pixel = false;
        
        for (GHL::UInt32 x = 0; x < width(); ++x) {
            if (line[x*4+3]) {
                has_pixel = true;
                if (x>max_x) max_x = x;
                if (x<min_x) min_x = x;
            }
        }
        if (has_pixel) {
            if (y < min_y) min_y = y;
            if (y > max_y) max_y = y;
        }
    }
    
    if (max_x < min_x || max_y < min_y) {
        /// empty image, save pixel
        min_x = max_x = min_y = max_y = 0;
    }
    if (min_x>=max_x ||
        min_y>=max_y)
        return false;
    m_rect.x = min_x;
    m_rect.y = min_y;
    m_rect.w = max_x-min_x+1;
    m_rect.h = max_y-min_y+1;
    SetSize(m_rect.w, m_rect.h);
    return true;
}


TextureData::TextureData( GHL::UInt32 w, GHL::UInt32 h) : TextureSubData(GHL_CreateImage(w, h, GHL::IMAGE_FORMAT_RGBA),Sandbox::Recti(0,0,w,h)) {
     m_encode_settings = 0;
    m_image_file_format = GHL::IMAGE_FILE_FORMAT_PNG;
    sb_assert(GetImage());
    GetImage()->Release();
    GetImage()->Fill(0x00000000);
}

TextureData::TextureData( GHL::Image* img ) : TextureSubData(img,Sandbox::Recti(0,0,img->GetWidth(),img->GetHeight())) {
    img->Release();
    m_encode_settings = 0;
    m_image_file_format = GHL::IMAGE_FILE_FORMAT_PNG;
}

void TextureData::SetImageFileFormatPNG(int settings) {
    m_image_file_format = GHL::IMAGE_FILE_FORMAT_PNG;
    m_encode_settings = settings;
}
void TextureData::SetImageFileFormatJPEG(int settings) {
    if (GetImage()) {
        GetImage()->Convert(GHL::IMAGE_FORMAT_RGB);
    }
    m_image_file_format = GHL::IMAGE_FILE_FORMAT_JPEG;
    m_encode_settings = settings;
}

void TextureData::SetImageFileFormatETC1() {
    if (GetImage()) {
        GetImage()->Convert(GHL::IMAGE_FORMAT_RGB);
    }
    m_image_file_format = GHL::IMAGE_FILE_FORMAT_ETC;
}

TextureData::~TextureData() {
    
}

void TextureData::PremultiplyAlpha() {
    sb_assert(GetImage());
    if (GetImage()->GetFormat()==GHL::IMAGE_FORMAT_4444) {
        GetImage()->Convert(GHL::IMAGE_FORMAT_RGBA);
    }
    GetImage()->PremultiplyAlpha();
}

void TextureData::Place( GHL::UInt32 x, GHL::UInt32 y, const TextureSubDataPtr& img ) {
    sb_assert(img);
    GetImage()->Draw(x, y, img->GetSubImage());
}

bool TextureData::SetAlpha( const sb::intrusive_ptr<TextureData>& alpha_tex ) {
    if (!GetImage()) return false;
    return GetImage()->SetAlpha(alpha_tex->GetImage());
}

TextureDataPtr TextureData::ExtractSubData(int x,int y,int w,int h) {
    GHL::Image* cropped_img = GetSubImage()->SubImage(x, y, w, h);
    if (!cropped_img) return TextureDataPtr();
    return TextureDataPtr(new TextureData(cropped_img));
}



