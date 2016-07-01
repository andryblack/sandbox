#include "sb_freetype_font.h"
#include "sb_resources.h"

#include <sbstd/sb_map.h>
#include <sbstd/sb_string.h>
#include <sbstd/sb_algorithm.h>

#include <ghl_data.h>
#include <ghl_data_stream.h>
#include <ghl_texture.h>
#include <ghl_image.h>

#include "sb_log.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SIZES_H
#include FT_STROKER_H

SB_META_DECLARE_OBJECT(Sandbox::FreeTypeFont,Sandbox::Font)

namespace Sandbox {
    
    struct Face : public sb::ref_countered_base {
        FT_Face face;
        GHL::Data* data;
        Face() : face(0),data(0) {}
        ~Face() {
            if (face) {
                FT_Done_Face(face);
            }
            if (data) {
                data->Release();
            }
        }
    };
    typedef sb::intrusive_ptr<Face> FacePtr;
 
    struct Library  : public sb::ref_countered_base {
        FT_Library  library;
        Library() {
            if (FT_Init_FreeType(&library)) {
                LogError() << "failed init freetype";
            }
            sb_assert(m_instance == 0);
            m_instance = this;
        }
        ~Library() {
            m_face_cache.clear();
            FT_Done_FreeType(library);
            sb_assert(m_instance == this);
            m_instance = 0;
        }
        static Library*  m_instance;
        static sb::intrusive_ptr<Library> get() {
            if (!m_instance) {
                m_instance = new Library();
            }
            return sb::intrusive_ptr<Library>(m_instance);
        }
        typedef sb::map<sb::string,FacePtr> FaceMap;
        FaceMap m_face_cache;
        
        FacePtr get_face(Resources* res, const char* file) {
            sb::string facename = file;
            FaceMap::const_iterator it = m_face_cache.find(facename);
            if (it!=m_face_cache.end())
                return it->second;
            GHL::DataStream* ds = res->OpenFile(file);
            if (!ds) {
                LogError() << "failed opening font file " << file;
                return FacePtr();
            }
            GHL::Data* data = GHL_ReadAllData(ds);
            sb_assert(data);
            FacePtr face(new Face);
            face->data = data;
            if (FT_New_Memory_Face(library,data->GetData(),data->GetSize(),0,&face->face)) {
                LogError() << "failed loading font file " << file;
                return FacePtr();
            }
            if (int error = FT_Select_Charmap(  face->face,               /* target face object */
                                  FT_ENCODING_UNICODE )) {
                LogError() << "FT_Select_CharMap " << error;
            }
            m_face_cache[facename] = face;
            return face;
        }
    };
    typedef sb::intrusive_ptr<Library> LibraryPtr;
    
    Library* Library::m_instance = 0;
    
    struct FreeTypeFont::Impl {
        LibraryPtr  library;
        FacePtr     face;
        FT_Size     size;
        Resources*  resources;
        FreeTypeFontConfig  config;
        
        int         initial_texture_width;
        int         initial_texture_height;
        
        Impl() : library(Library::get()) {
            initial_texture_width = 128;
            initial_texture_height = 128;
        }
        
        sb::vector<GHL::Texture*> m_changed_textures;
        
        struct TextureData {
            TexturePtr  tex;
            int pos_x;
            int pos_y;
            int max_h;
        };
        typedef sb::vector<TextureData> TextureDataVector;
        TextureDataVector m_textures;
        
        TexturePtr alloc_texture(int w, int h, int& x, int& y) {
            TextureData* data = 0;
            for (TextureDataVector::iterator it = m_textures.begin();it!=m_textures.end();++it) {
                int tw = it->tex->GetRealWidth();
                int th = it->tex->GetRealHeight();
                if ((it->pos_x + w) > tw) {
                    if ((it->pos_y + it->max_h + h) > th) {
                        continue;
                    }
                }
                if ((it->pos_y+h) > th) {
                    continue;
                }
                data = &(*it);
                break;
            }
            if (!data) {
                /// allocate new texture
                GHL::Image* fill = GHL_CreateImage(initial_texture_width, initial_texture_height, GHL::IMAGE_FORMAT_RGBA);
                fill->Fill(0x00000000);
                TexturePtr tex = resources->CreateTexture(initial_texture_width, initial_texture_height, 1.0f / resources->GetScale(), true, fill);
                fill->Release();
                tex->SetFiltered(true);
                TextureData d;
                d.tex = tex;
                d.pos_x = 1;
                d.pos_y = 1;
                d.max_h = 0;
                m_textures.push_back(d);
                data = &m_textures.back();
            }
            int tw = data->tex->GetRealWidth();
            int th = data->tex->GetRealHeight();
            if ((data->pos_x + w) > tw) {
                data->pos_x = 1;
                data->pos_y += data->max_h;
                data->max_h = 0;
            }
            x = data->pos_x;
            y = data->pos_y;
            sb_assert((x+w)<=tw);
            sb_assert((y+h)<=th);
            
            if (h>data->max_h)
                data->max_h = h;
            data->pos_x += w;
            return data->tex;
        }
        
        struct ImageData {
            GHL::Image* img;
            int x;
            int y;
            int w;
            int h;
            int left;
            int top;
        };
        
        void put_bitmap(const FT_Bitmap* bm,GHL::UInt32 clr,ImageData& data) {
            int w = bm->width;
            int h = bm->rows;
            
            data.img = 0;
            if (w * h) {
                GHL::Image* aimg = GHL_CreateImageWithData(w, h, GHL::IMAGE_FORMAT_GRAY, bm->buffer);
                data.img = GHL_CreateImage(w, h, GHL::IMAGE_FORMAT_RGBA);
                data.img->Fill(clr);
                data.img->SetAlpha(aimg);
                aimg->Release();
            }
            
            data.w = w;
            data.h = h;
            
        }
        
        void render_outline(Font::Glypth* gl, ImageData& data) {
            // Set up a stroker.
            float outlineWidth = 1.0 * resources->GetScale();
            FT_Stroker stroker;
            FT_Stroker_New(library->library, &stroker);
            FT_Stroker_Set(stroker,
                           (int)(outlineWidth * 64),
                           FT_STROKER_LINECAP_ROUND,
                           FT_STROKER_LINEJOIN_ROUND,
                           0);
            
            FT_Glyph glyph;
            if (FT_Get_Glyph(face->face->glyph, &glyph) == 0)
            {
                FT_Glyph_StrokeBorder(&glyph, stroker,0, 1);
                // Render the outline spans to the span list
                FT_Glyph_To_Bitmap(&glyph,FT_RENDER_MODE_NORMAL,0,1);
                FT_BitmapGlyph ft_bitmap_glyph = (FT_BitmapGlyph) glyph;
                const FT_Bitmap* bm = &ft_bitmap_glyph->bitmap;
                put_bitmap(bm,0x00000000,data);
                data.left = ft_bitmap_glyph->left;
                data.top = ft_bitmap_glyph->top;
            } else {
                LogError() << "FT_Get_Glyph failed";
            }
            // Clean up afterwards.
            FT_Stroker_Done(stroker);
            FT_Done_Glyph(glyph);
        }
        
        void render_glyph(Font::Glypth* gl) {
            ImageData img;
            const FT_GlyphSlot  slot = face->face->glyph;
            if (config.outline) {
                render_outline(gl,img);
            }
            int error = FT_Render_Glyph( slot,  FT_RENDER_MODE_NORMAL  );
            if (error) return;
            
            const FT_Bitmap* bm = &(slot->bitmap);
            if (!config.outline)
            {
                put_bitmap(bm,0xffffffff,img);
                img.left = slot->bitmap_left;
                img.top = slot->bitmap_top;
            } else {
                if (img.img && (bm->rows * bm->width)) {
                    GHL::Byte* dst_data = img.img->GetDataPtr();
                    const GHL::Byte* src_data = bm->buffer;
                    
                    int ox = slot->bitmap_left-img.left;
                    int oy = -(slot->bitmap_top-img.top);
                    
                    dst_data+=img.img->GetWidth()*4*oy;
                    dst_data+=4*ox;
                    
                    for (int y=0;y<bm->rows;++y) {
                        GHL::Byte* dst = dst_data;
                        for (int x=0;x<bm->width;++x) {
                            GHL::UInt16 c = *src_data++;
                            GHL::UInt16 ic = 0xff-c;
                            *dst = ((*dst * ic) / 0xff) + c; ++dst;
                            *dst = ((*dst * ic) / 0xff) + c; ++dst;
                            *dst = ((*dst * ic) / 0xff) + c; ++dst;
                            *dst = ((*dst * ic) / 0xff) + c; ++dst;
                        }
                        dst_data+=img.img->GetWidth()*4;
                    }
                }
            }
            
            TexturePtr tex;
            
            if (img.img) {
                img.img->PremultiplyAlpha();
                tex = alloc_texture(img.w+1,img.h+1,img.x,img.y);
                GHL::Texture* ntex = tex->Present(resources);
                ntex->SetData(img.x, img.y, img.img);
                if (std::find(m_changed_textures.begin(),m_changed_textures.end(),ntex)==m_changed_textures.end()) {
                    m_changed_textures.push_back(ntex);
                }
            }
            
            float iscale = 1.0f / resources->GetScale();
            
            gl->img = Image(tex,iscale*img.x ,iscale*img.y,iscale*img.w,iscale*img.h);
            gl->img.SetHotspot(Sandbox::Vector2f(-img.left * iscale,img.top * iscale));
            gl->asc = (face->face->glyph->advance.x/64) * iscale;
        }
        
        void commit() {
            for (sb::vector<GHL::Texture*>::iterator it = m_changed_textures.begin();it!=m_changed_textures.end();++it) {
                (*it)->DiscardInternal();
            }
            m_changed_textures.clear();
        }
    };
    
    FreeTypeFont::FreeTypeFont(Impl* impl) : m_impl(impl) {
        
    }
    
    FreeTypeFont::~FreeTypeFont() {
        delete m_impl;
    }
    
    sb::intrusive_ptr<FreeTypeFont> FreeTypeFont::Load( Resources* resources, const char* file, const FreeTypeFontConfig& config ) {
        LibraryPtr library = Library::get();
        FacePtr face = library->get_face(resources,file);
        sb::intrusive_ptr<FreeTypeFont> res;
        if (!face) {
            return res;
        }
        FT_Size size;
        FT_New_Size(face->face,&size);
        FT_Activate_Size(size);
        
        float scale = resources->GetScale();
        
        int size_x = static_cast<int>(config.size*64.0f);
        int size_y = static_cast<int>(config.size*64.0f);
        if (int error = FT_Set_Char_Size(face->face,
                                     FT_F26Dot6(size_x),
                                     FT_F26Dot6(size_y),
                                     config.dpi*scale,
                             config.dpi*scale)) {
            LogError() << "FT_Set_Char_Size " << error;
        }
        
        Impl* impl = new Impl;
        impl->size = size;
        impl->face = face;
        impl->resources = resources;
        impl->config = config;
        impl->initial_texture_height *= resources->GetScale();
        impl->initial_texture_width *= resources->GetScale();
        
        res.reset(new FreeTypeFont(impl));
        
        return res;
    }
    
    void FreeTypeFont::preallocate_text(const char* text) {
        while (*text) {
            UTF32Char ch = 0;
            text = get_char(text,ch);
            if (ch) {
                Glypth* gl = get_glypth_int(ch);
                if (gl) {
                    continue;
                }
                FT_Activate_Size(m_impl->size);
                FT_Int32 flags = FT_LOAD_DEFAULT;
                flags = flags | FT_LOAD_TARGET_NORMAL;
                int glyph_index = FT_Get_Char_Index( m_impl->face->face, ch );
                if (m_impl->config.outline)
                    flags = flags | FT_LOAD_NO_BITMAP;
                int error = FT_Load_Glyph( m_impl->face->face, glyph_index, flags );
                if ( error )
                    continue;
                gl = add_glypth_int(ch);
                m_impl->render_glyph(gl);
            } else {
                break;
            }
        }
        m_impl->commit();
    }
    
    float FreeTypeFont::Draw(Graphics& g,const DrawAttributesPtr& attributes,const Vector2f& pos,const char* text,FontAlign align) const {
        const_cast<FreeTypeFont*>(this)->preallocate_text(text);
        return Font::Draw(g, attributes, pos, text, align);
    }
    float FreeTypeFont::GetTextWidth(const char* text) const {
        const_cast<FreeTypeFont*>(this)->preallocate_text(text);
        return Font::GetTextWidth(text);
    }
    
}