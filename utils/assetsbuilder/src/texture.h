#ifndef _AB_TEXTURE_H_INCLUDED_
#define _AB_TEXTURE_H_INCLUDED_

#include <meta/sb_meta.h>
#include <ghl_image_decoder.h>
#include <sb_rect.h>

namespace GHL {
    struct ImageDecoder;
    struct Image;
}

class Texture : public Sandbox::meta::object {
    SB_META_OBJECT
private:
    GHL::UInt32         m_width;
    GHL::UInt32         m_height;
protected:
    void SetSize(GHL::UInt32 w,GHL::UInt32 h) { m_width = w; m_height = h;}
public:
    explicit Texture( GHL::UInt32 w, GHL::UInt32 h) :
    m_width(w), m_height(h) {}
    GHL::UInt32 width() const { return m_width; }
    GHL::UInt32 height() const { return m_height; }
};
typedef sb::intrusive_ptr<Texture> TexturePtr;

class TextureSubData;
typedef sb::intrusive_ptr<TextureSubData> TextureSubDataPtr;

class TextureSubData : public Texture {
    SB_META_OBJECT
protected:
private:
    GHL::Image* m_data;
    Sandbox::Recti m_rect;
protected:
    GHL::Image* GetImage() { return m_data; }
    const GHL::Image* GetImage() const { return m_data; }
    void SetImage(GHL::Image* img);
public:
    explicit TextureSubData(GHL::Image* img,const Sandbox::Recti& rect);
    ~TextureSubData();
    const GHL::Image* GetSubImage();
    GHL::Image* GetSubImageRotated();
    GHL::Image* GetSubImageUnrotated();
    
    const Sandbox::Recti& GetRect() const { return m_rect; }
    TextureSubDataPtr Extract( const Sandbox::Recti& rect );
    sb::string GetMD5() const;
    sb::string GetMD5Rotated() const;
    
    bool Crop();
    void Free();
   
};

class TextureData;
typedef sb::intrusive_ptr<TextureData> TextureDataPtr;

class TextureData : public TextureSubData {
    SB_META_OBJECT
private:
    GHL::ImageFileFormat m_image_file_format;
public:
    explicit TextureData( GHL::UInt32 w, GHL::UInt32 h );
    explicit TextureData( GHL::Image* img );
    ~TextureData();
    void PremultiplyAlpha();
    bool Grayscale();    
    void Convert(GHL::ImageFormat fmt);
    void Invert();
    void Rotate(bool ccw);
    void Place( GHL::UInt32 x, GHL::UInt32 y,
               const TextureSubDataPtr& img );
    void Blend( GHL::UInt32 x, GHL::UInt32 y,
               const TextureSubDataPtr& img );
    void PlaceRotated( GHL::UInt32 x, GHL::UInt32 y,
                      const TextureSubDataPtr& img );
    void PlaceUnrotated( GHL::UInt32 x, GHL::UInt32 y,
                      const TextureSubDataPtr& img );
    
    bool SetAlpha( const sb::intrusive_ptr<TextureData>& alpha_tex );
     
    TextureDataPtr ExtractSubData(int x,int y,int w,int h);
    
    GHL::ImageFormat GetFormat() const;
    GHL::ImageFileFormat GetImageFileFormat() const { return m_image_file_format; }
    void SetImageFileFormatPNG();
    void SetImageFileFormatJPEG();
    void SetImageFileFormatETC1();
    bool IsJPEG() const { return m_image_file_format == GHL::IMAGE_FILE_FORMAT_JPEG; }
    bool HasBits(int treshold);
    
};



#endif /*_AB_TEXTURE_H_INCLUDED_*/
