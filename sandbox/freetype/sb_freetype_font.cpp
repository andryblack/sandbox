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
#include "sb_texture_pool.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SIZES_H
#include FT_STROKER_H
#include FT_BITMAP_H
#include FT_TRUETYPE_TABLES_H
#include FT_WINFONTS_H


SB_META_DECLARE_OBJECT(Sandbox::FreeTypeFont,Sandbox::OutlineFontDataProvider)


namespace Sandbox {

    static const char* MODULE = "ft";
    
    
    
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
        static void check_released() {
            if (m_instance != 0) {
                SB_LOGE("not all fonts released on exit");
                sb_assert(false);
                m_instance->textures.Clear();
            }
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
                SB_LOGE("failed opening font file " << file);
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
                SB_LOGE("FT_Select_CharMap " << error);
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
                SB_LOGE("FT_Get_Glyph failed");
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
                tex = library->textures.Alloc(img.w+1,img.h+1,img.x,img.y,ntex);
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
    
    
    
    
    template<typename T>
    void setMax(T& _var, const T& _newValue)
    {
        if (_var < _newValue)
            _var = _newValue;
    }

    
    FreeTypeFont::FreeTypeFont(Impl* impl) : OutlineFontDataProvider(), m_impl(impl) {
        if (m_impl->config.outline) {
            set_outline_data(FontDataPtr(new FontData()));
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
        gl = m_data->add_glypth_int(0x200B);
        gl->asc = 0.0f;
        m_impl->put_image(gl, img);
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
            SB_LOGE( "FT_Set_Char_Size " << error );
        }
        
        Impl* impl = new Impl(library,face);
        impl->size = size;
        impl->scale = scale;
        impl->outline_width = 1.0f * scale * config.outline_width;
        impl->config = config;
        
        res.reset(new FreeTypeFont(impl));
        
        library->textures.Commit();
        return res;
    }
    
    void FreeTypeFont::Release() {
        Library::check_released();
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
    
    
    void FreeTypeFont::AllocateSymbols( const char* _text ) {
        FontDataProvider::AllocateSymbols(_text);
        m_impl->library->textures.Commit();
    }
    
    
}
