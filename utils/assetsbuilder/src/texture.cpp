#include <utils/sb_md5.h>

extern "C" {
#include <utils/md5.h>
}

#include <utils/sb_hex.h>
#include <sb_color.h>

#include "texture.h"

TextureSubData::TextureSubData( GHL::Image* image,const Sandbox::Recti& rect) : Texture(rect.w,rect.h), m_data(image),m_rect(rect) {
    if (m_data) {
        m_data->AddRef();
    }
}


const GHL::Image* TextureSubData::GetSubImage() {
    if (m_rect.x == 0 &&
        m_rect.y == 0 &&
        GHL::UInt32(m_rect.w) == m_data->GetWidth() &&
        GHL::UInt32(m_rect.h) == m_data->GetHeight()) {
        return m_data;
    }
    GHL::Image* sub = m_data->SubImage(m_rect.x, m_rect.y, m_rect.w, m_rect.h);
    m_data->Release();
    m_data = sub;
    m_rect.x = 0;
    m_rect.y = 0;
    return  m_data;
}

GHL::Image* TextureSubData::GetSubImageRotated() {
    GHL::Image* src = 0;
    if (m_rect.x == 0 &&
        m_rect.y == 0 &&
        GHL::UInt32(m_rect.w) == m_data->GetWidth() &&
        GHL::UInt32(m_rect.h) == m_data->GetHeight()) {
        src = m_data->Clone();
    } else {
        src = m_data->SubImage(m_rect.x, m_rect.y, m_rect.w, m_rect.h);
    }
    src->RotateCW();
    return src;
}
GHL::Image* TextureSubData::GetSubImageUnrotated() {
    GHL::Image* src = 0;
    if (m_rect.x == 0 &&
        m_rect.y == 0 &&
        GHL::UInt32(m_rect.w) == m_data->GetWidth() &&
        GHL::UInt32(m_rect.h) == m_data->GetHeight()) {
        src = m_data->Clone();
    } else {
        src = m_data->SubImage(m_rect.x, m_rect.y, m_rect.w, m_rect.h);
    }
    src->RotateCCW();
    return src;
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
            const GHL::Byte* dd = ddS + ((m_rect.x + i) + (m_rect.y + m_rect.h - j - 1)*m_data->GetWidth())*bpp;
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

void TextureSubData::SetImage(GHL::Image* img) {
    Free();
    m_data = img;
    m_data->AddRef();
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
    m_image_file_format = GHL::IMAGE_FILE_FORMAT_PNG;
    sb_assert(GetImage());
    GetImage()->Release();
    GetImage()->Fill(0x00000000);
}

TextureData::TextureData( GHL::Image* img ) : TextureSubData(img,Sandbox::Recti(0,0,img->GetWidth(),img->GetHeight())) {
    img->Release();
    m_image_file_format = GHL::IMAGE_FILE_FORMAT_PNG;
}

void TextureData::SetImageFileFormatPNG() {
    m_image_file_format = GHL::IMAGE_FILE_FORMAT_PNG;
}
void TextureData::SetImageFileFormatJPEG() {
    if (GetImage()) {
        GetImage()->Convert(GHL::IMAGE_FORMAT_RGB);
    }
    m_image_file_format = GHL::IMAGE_FILE_FORMAT_JPEG;
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

bool TextureData::Grayscale() {
    sb_assert(GetImage());
    if (GetImage()->GetFormat()==GHL::IMAGE_FORMAT_RGB) {
        return GetImage()->Convert(GHL::IMAGE_FORMAT_GRAY);
    }
    return false;
}

void TextureData::Convert(GHL::ImageFormat fmt) {
    sb_assert(GetImage());
    GetImage()->Convert(fmt);
}
void TextureData::Invert() {
    if (!GetImage()) return;
    if (GetFormat()!=GHL::IMAGE_FORMAT_GRAY) {
        /// @todo
        return;
    }
    GHL::UInt32 w = GetImage()->GetWidth();
    GHL::UInt32 h = GetImage()->GetHeight();
    GHL::Data* new_data = GHL_CreateData(w*h);
    GHL::Byte* dst = new_data->GetDataPtr();
    const GHL::Byte* src = GetImage()->GetData()->GetData();
    for (GHL::UInt32 i=0;i<w*h;++i) {
        *dst = 0xff - *src;
        ++dst;++src;
    }
    GHL::Image* new_img = GHL_CreateImageWithData(w, h, GHL::IMAGE_FORMAT_GRAY,new_data);
    new_data->Release();
    SetImage(new_img);
    new_img->Release();
}
GHL::ImageFormat TextureData::GetFormat() const {
    if (GetImage()) {
        return GetImage()->GetFormat();
    }
    return GHL::IMAGE_FORMAT_UNKNOWN;
}
void TextureData::Place( GHL::UInt32 x, GHL::UInt32 y, const TextureSubDataPtr& img ) {
    sb_assert(img);
    GetImage()->Draw(x, y, img->GetSubImage());
}
void TextureData::Blend( GHL::UInt32 x, GHL::UInt32 y,
               const TextureSubDataPtr& img ) {
    GetImage()->Convert(GHL::IMAGE_FORMAT_RGBA);
    const GHL::Image* img_src = img->GetSubImage();
    
    GHL::UInt32 w = img_src->GetWidth();
    GHL::UInt32 h = img_src->GetHeight();
    if ((x + w) > width()) {
        w = width() - x;
    }
    if ((y + h) > height()) {
        h = height() - y;
    }

    GHL::Byte* dst = GetImage()->GetData()->GetDataPtr();
    dst += y * 4 * width() + x * 4;

    const GHL::Byte* src = img_src->GetData()->GetData();
    for (GHL::UInt32 yy=0;yy<h;++yy) {

        GHL::UInt32* vdst = reinterpret_cast<GHL::UInt32*>(dst);
        const GHL::UInt32* vsrc = reinterpret_cast<const GHL::UInt32*>(src);
        for (GHL::UInt32 xx=0;xx<w;++xx) {
            Sandbox::Color csrc(*vsrc);
            Sandbox::Color cdst(*vdst);

            cdst = cdst*(1.0f-csrc.a)+csrc;
            *vdst = cdst.hw();

            ++vsrc;
            ++vdst;
        }

        dst += 4 * width();
        src += 4 * img_src->GetWidth();
    }

}
void TextureData::PlaceRotated( GHL::UInt32 x, GHL::UInt32 y,
                  const TextureSubDataPtr& img ) {
    sb_assert(img);
    GHL::Image* i = img->GetSubImageRotated();
    GetImage()->Draw(x, y, i);
    i->Release();
}
void TextureData::Rotate( bool ccw ) {
    sb_assert(img);
    if (ccw) 
        GetImage()->RotateCW();
    else
        GetImage()->RotateCCW();
}
void TextureData::PlaceUnrotated( GHL::UInt32 x, GHL::UInt32 y,
                    const TextureSubDataPtr& img ) {
    GHL::Image* i = img->GetSubImageUnrotated();
    GetImage()->Draw(x, y, i);
    i->Release();
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

bool TextureData::HasBits(int treshold) {
    GHL::Image* image = GetImage();
    if (!image) return false;
    if (!image->GetData()) return false;
    const GHL::Byte* data = image->GetData()->GetData();
    if (!data) return false;
    
    if (GetImage()->GetFormat()!=GHL::IMAGE_FORMAT_RGBA) {
        return true;
    }

    size_t len = image->GetWidth() * image->GetHeight();
    for (size_t i=0;i<len;i++) {
        if (data[i*4+3] > treshold) {
            return true;
        }
    }

    return false;
}



