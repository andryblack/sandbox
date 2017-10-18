#include "sb_freetype_font.h"
#include "sb_resources.h"

#include <sbstd/sb_map.h>
#include <sbstd/sb_string.h>
#include <sbstd/sb_algorithm.h>

#include <ghl_data.h>
#include <ghl_data_stream.h>
#include <ghl_texture.h>
#include <ghl_image.h>
#include "sb_graphics.h"
#include "sb_log.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SIZES_H
#include FT_STROKER_H
#include FT_BITMAP_H
#include FT_TRUETYPE_TABLES_H
#include FT_WINFONTS_H


SB_META_DECLARE_OBJECT(Sandbox::FreeTypeFont,Sandbox::Font)
SB_META_DECLARE_OBJECT(Sandbox::FreeTypeFontChild,Sandbox::Font)

namespace Sandbox {
    
    
    class TexturePool {
        int         m_initial_texture_width;
        int         m_initial_texture_height;
        Resources*  m_resources;
        sb::vector<GHL::Texture*> m_changed_textures;
        struct TextureData {
            TexturePtr  tex;
            int pos_x;
            int pos_y;
            int max_h;
        };
        typedef sb::vector<TextureData> TextureDataVector;
        TextureDataVector m_textures;
    public:
        TexturePool( Resources* resources  ) : m_resources(resources) {
            m_initial_texture_width = 512 * resources->GetScale();
            m_initial_texture_height = 512 * resources->GetScale();
        }
        
        TexturePtr alloc(int w, int h, int& x, int& y,GHL::Texture* &ntex) {
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
                GHL::Image* fill = GHL_CreateImage(m_initial_texture_width,
                                                   m_initial_texture_height,
                                                   GHL::IMAGE_FORMAT_GRAY);
                fill->Fill(0x00000000);
                TexturePtr tex = m_resources->CreateTexture(fill, 1.0f / m_resources->GetScale(), GHL::TEXTURE_FORMAT_ALPHA);
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
            ntex = data->tex->Present(m_resources);
            if (std::find(m_changed_textures.begin(),m_changed_textures.end(),ntex)==m_changed_textures.end()) {
                m_changed_textures.push_back(ntex);
            }
            
            return data->tex;
        }
        
        void commit() {
            for (sb::vector<GHL::Texture*>::iterator it = m_changed_textures.begin();it!=m_changed_textures.end();++it) {
                (*it)->DiscardInternal();
            }
            m_changed_textures.clear();
        }

    };
    
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
        TexturePool textures;
        Library(Resources* resources) : textures(resources) {
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
        static sb::intrusive_ptr<Library> get(Resources* resources) {
            if (!m_instance) {
                m_instance = new Library(resources);
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
            ds->Release();
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
        FreeTypeFontConfig  config;
        float scale;
        float outline_width;
        
        Impl(const LibraryPtr& library, const FacePtr& face) : library(library),face(face) {
            scale = 1.0f;
            outline_width = 1.0f;
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
        
        void put_bitmap(const FT_Bitmap* bm,ImageData& data) {
            int w = bm->width;
            int h = bm->rows;
            
            data.img = 0;
            if ((w * h)>0) {
        
                data.img = GHL_CreateImage(w,h,GHL::IMAGE_FORMAT_GRAY);
                ::memcpy(data.img->GetData()->GetDataPtr(), bm->buffer, w*h);
            }
            
            data.w = w;
            data.h = h;
            
        }
        
        void render_outline(FontData::Glypth* gl, ImageData& data) {
            // Set up a stroker.
            FT_Stroker stroker;
            FT_Stroker_New(library->library, &stroker);
            FT_Stroker_Set(stroker,
                           (int)(outline_width * 64),
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
                FT_Bitmap* bm = &ft_bitmap_glyph->bitmap;
                put_bitmap(bm,data);
                data.left = ft_bitmap_glyph->left;
                data.top = ft_bitmap_glyph->top;
            } else {
                LogError() << "FT_Get_Glyph failed";
            }
            // Clean up afterwards.
            FT_Stroker_Done(stroker);
            FT_Done_Glyph(glyph);
        }
        
        void put_image( FontData::Glypth* gl, ImageData& img ) {
            TexturePtr tex;
            
            float iscale = 1.0f / scale;
            
            if (img.img) {
                GHL::Texture* ntex = 0;
                tex = library->textures.alloc(img.w+1,img.h+1,img.x,img.y,ntex);
                ntex->SetData(img.x, img.y, img.img);
                img.img->Release();
                img.img = 0;
                iscale = tex->GetScale();
            }
            
            
            
            gl->img = Image(tex,iscale*img.x ,iscale*img.y,iscale*img.w,iscale*img.h);
            gl->img.SetHotspot(Sandbox::Vector2f(-float(img.left) * iscale,float(img.top) * iscale));
            gl->img.SetSize(float(img.w) / scale , float(img.h) / scale );
        }
        
        void render_glyph(FontData::Glypth* gl,bool ol) {
            ImageData img;
            const FT_GlyphSlot  slot = face->face->glyph;
            if (ol) {
                render_outline(gl,img);
            } else {
                int error = FT_Render_Glyph( slot,  FT_RENDER_MODE_NORMAL  );
                if (error) return;
                
                const FT_Bitmap* bm = &(slot->bitmap);
                put_bitmap(bm,img);
                img.left = slot->bitmap_left;
                img.top = slot->bitmap_top;
            }
            
            put_image(gl, img);
            
           
            gl->asc = (face->face->glyph->advance.x/64) / scale;
        }
        
        
    };
    
    
    FreeTypeFontChild::FreeTypeFontChild(const FontPtr& font, const FontDataPtr& data) : Font(data),m_parent(font) {
        set_size(m_parent->GetSize());
        set_height(m_parent->GetHeight());
        set_baseline(m_parent->GetBaseline());
    }
    
    void FreeTypeFontChild::AllocateSymbols(const char *text) {
        if (m_parent)
            m_parent->AllocateSymbols(text);
    }
    
    template<typename T>
    void setMax(T& _var, const T& _newValue)
    {
        if (_var < _newValue)
            _var = _newValue;
    }

    
    FreeTypeFont::FreeTypeFont(Impl* impl) : Font(FontDataPtr(new FontData())), m_impl(impl) {
        if (m_impl->config.outline) {
            m_outline_data.reset(new FontData());
        }
        FT_Face ftFace = m_impl->face->face;
        int fontAscent = int(ftFace->size->metrics.ascender >> 6);
        int fontDescent = -int(ftFace->size->metrics.descender >> 6);
        
        TT_OS2* os2 = (TT_OS2*)FT_Get_Sfnt_Table(ftFace, ft_sfnt_os2);
        
        if (os2)
        {
            int fixFontAscent = os2->usWinAscent * ftFace->size->metrics.y_ppem / ftFace->units_per_EM;
            setMax(fontAscent, fixFontAscent );
            int fixFontDescent = os2->usWinDescent * ftFace->size->metrics.y_ppem / ftFace->units_per_EM;
            setMax(fontDescent, fixFontDescent );
            
            fixFontAscent = os2->sTypoAscender * ftFace->size->metrics.y_ppem / ftFace->units_per_EM;
            setMax(fontAscent, fixFontAscent);
            fixFontDescent = -os2->sTypoDescender * ftFace->size->metrics.y_ppem / ftFace->units_per_EM;
            setMax(fontDescent, fixFontDescent );
        }
        set_size(m_impl->config.size);
        set_height(float(fontAscent+fontDescent)/m_impl->scale);
        set_baseline(float(fontDescent)/m_impl->scale);
        if (m_impl->config.substitute_code) {
            preallocate_symb(m_impl->config.substitute_code);
            m_data->SetSubsituteCode(m_impl->config.substitute_code);
        }
        // cursor
        FontData::Glypth* gl = m_data->add_glypth_int(0xFFFFFFFE);
        gl->asc = 2;
        Impl::ImageData img;
        img.h = GetHeight() * m_impl->scale;
        img.w = 2.0f * m_impl->scale;
        img.left = 0;
        img.top = fontAscent;
        img.img = GHL_CreateImage(img.w, img.h, GHL::IMAGE_FORMAT_GRAY);
        img.img->Fill(0xff);
        m_impl->put_image(gl, img);
        if (m_outline_data) {
            gl = m_outline_data->add_glypth_int(0xFFFFFFFE);
            gl->asc = 2;
        }
        preallocate_symb(' ');
        gl = m_data->get_glypth_int(' ');
        if (gl) {
            float asc = gl->asc;
            gl = m_data->add_glypth_int(0x00A0);
            gl->asc = asc;
            if (m_outline_data) {
                gl = m_outline_data->add_glypth_int(0x00A0);
                gl->asc = asc;
            }
        }
        /// empty symbols
        img.h = 0;
        img.w = 0;
        img.left = 0;
        img.top = fontAscent;
        gl = m_data->add_glypth_int('\r');
        gl->asc = 0.0f;
        m_impl->put_image(gl, img);
        gl = m_data->add_glypth_int('\n');
        gl->asc = 0.0f;
        m_impl->put_image(gl, img);
    }
    
    void FreeTypeFont::SetCharImage(GHL::UInt32 code,const Sandbox::ImagePtr& image,float advance) {
        FontData::Glypth* g = m_data->add_glypth_int(code);
        if (image)
            g->img = *image;
        g->asc = advance;
        if (m_outline_data) {
            g = m_outline_data->add_glypth_int(code);
            g->asc = advance;
        }
    }
    
    void FreeTypeFont::AddCharImage(GHL::UInt32 code,const Sandbox::ImagePtr& image,float advance) {
        if (HasGlyph(code))
            return;
        SetCharImage(code,image,advance);
    }
    
    FreeTypeFont::~FreeTypeFont() {
        delete m_impl;
    }
    
    sb::intrusive_ptr<FreeTypeFont> FreeTypeFont::Load( Resources* resources, const char* file, const FreeTypeFontConfig& config ) {
        LibraryPtr library = Library::get(resources);
        FacePtr face = library->get_face(resources,file);
        sb::intrusive_ptr<FreeTypeFont> res;
        if (!face) {
            return res;
        }
        FT_Size size;
        FT_New_Size(face->face,&size);
        FT_Activate_Size(size);
        
        float scale = resources->GetScale() * config.scale;
        
        // The font is scalable, so set the font size by first converting the requested size to FreeType's 26.6 fixed-point
        // format.
       
        FT_F26Dot6 size_x = static_cast<FT_F26Dot6>(config.size* (1 << 6)*scale);
        FT_F26Dot6 size_y = static_cast<FT_F26Dot6>(config.size* (1 << 6)*scale);
        if (int error = FT_Set_Char_Size(face->face,
                                     size_x,
                                     size_y,
                                     config.dpi * config.x_scale,
                                     config.dpi)) {
            LogError() << "FT_Set_Char_Size " << error;
        }
        
        Impl* impl = new Impl(library,face);
        impl->size = size;
        impl->scale = scale;
        impl->outline_width = 1.0f * scale * config.outline_width;
        impl->config = config;
        
        res.reset(new FreeTypeFont(impl));
        
        library->textures.commit();
        return res;
    }
    
    bool FreeTypeFont::preallocate_symb(GHL::UInt32 ch) {
        FT_Activate_Size(m_impl->size);
        FT_Int32 flags = FT_LOAD_DEFAULT;
        flags = flags | FT_LOAD_TARGET_NORMAL;
        FT_UInt glyph_index = FT_Get_Char_Index( m_impl->face->face, ch );
        if (!glyph_index)
            return false;
        if (m_impl->config.outline)
            flags = flags | FT_LOAD_NO_BITMAP;
        int error = FT_Load_Glyph( m_impl->face->face, glyph_index, flags );
        if ( error )
            return false;
        FontData::Glypth* gl = 0;
        if (m_impl->config.outline) {
            gl = m_outline_data->add_glypth_int(ch);
            m_impl->render_glyph(gl, true);
        }
        gl = m_data->add_glypth_int(ch);
        m_impl->render_glyph(gl,false);
        return true;
    }
    
    void FreeTypeFont::preallocate_text(const char* text) {
        while (*text) {
            UTF32Char ch = 0;
            text = get_char(text,ch);
            if (ch) {
                FontData::Glypth* gl = m_data->get_glypth_int(ch);
                if (gl) {
                    continue;
                }
                preallocate_symb(ch);
            } else {
                break;
            }
        }
        m_impl->library->textures.commit();
    }
    
    bool FreeTypeFont::HasGlyph(GHL::UInt32 code) const {
        if (Font::HasGlyph(code))
            return true;
        if (const_cast<FreeTypeFont*>(this)->preallocate_symb(code)) {
            m_impl->library->textures.commit();
            return true;
        }
        return false;
    }
    
    void FreeTypeFont::AllocateSymbols( const char* _text ) {
        if (_text)
            const_cast<FreeTypeFont*>(this)->preallocate_text(_text);
    }
    
    FreeTypeFontChildPtr FreeTypeFont::CreateMainChild() {
        return FreeTypeFontChildPtr(new FreeTypeFontChild(FontPtr(this),GetMainData()));
    }
    FreeTypeFontChildPtr FreeTypeFont::CreateOutlineChild() {
        return FreeTypeFontChildPtr(new FreeTypeFontChild(FontPtr(this),GetOutlineData()));
    }
}
