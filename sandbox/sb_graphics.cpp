/*
 *  sb_graphics.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 07.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_graphics.h"
#include <sbstd/sb_assert.h>
#include <ghl_image.h>
#include "sb_geometry.h"
#include "sb_resources.h"

namespace Sandbox {
	
    static const char* MODULE = "Sanbox:Graphics";
	
	Graphics::Graphics(Resources* res) : m_resources(res){
        (void)MODULE;
		m_render = 0;
	    m_vertexes.reserve(512);
        m_vertexes2tex.reserve(512);
		m_indexes.reserve(512);
        m_scale = 1.0f;
        m_filter = 0;
	}
	
	Graphics::~Graphics() {
		if (m_fake_tex_white)
			m_fake_tex_white->Release();
        if (m_fake_tex_black)
            m_fake_tex_black->Release();
	}
    
    	
	void Graphics::Load(GHL::Render* render) {
		if (!m_fake_tex_white) {
            GHL::Image* img = GHL_CreateImage(1, 1, GHL::IMAGE_FORMAT_RGBA);
            img->Fill(0xFFFFFFFF);
            GHL::Texture* tex = render->CreateTexture(1,1,GHL::TEXTURE_FORMAT_RGBA,img);
            m_fake_tex_white = TexturePtr(new Texture(tex,1.0f));
			img->Release();
            tex->DiscardInternal();
		}
        if (!m_fake_tex_black) {
            GHL::Image* img = GHL_CreateImage(1, 1, GHL::IMAGE_FORMAT_RGBA);
            img->Fill(0xFF000000);
            GHL::Texture* tex = render->CreateTexture(1,1,GHL::TEXTURE_FORMAT_RGBA,img);
            m_fake_tex_black = TexturePtr(new Texture(tex,1.0f));
			img->Release();
            tex->DiscardInternal();
		}
        
#if 0
        /// mask self test
        {
            Image img;
            m_mask_itw = 1.0f / 256;
            m_mask_ith = 1.0f / 512;
            img.SetTextureRect(16, 32, 64, 128);
            m_transform = Transform2d();
            SetMaskImageTransform(img, Rectf(10,20,100,200));
            
            
            {
                appendVertex2(10,20,0,0);
                sb_assert(m_vertexes2tex.back().x == 10.0f);
                sb_assert(m_vertexes2tex.back().y == 20.0f);
                sb_assert(m_vertexes2tex.back().t2x == 16.0f / 256);
                sb_assert(m_vertexes2tex.back().t2y == 32.0f / 512);
                
            }
            
            {
                appendVertex2(110,220,0,0);
                sb_assert(m_vertexes2tex.back().x == 110.0f);
                sb_assert(m_vertexes2tex.back().y == 220.0f);
                sb_assert(m_vertexes2tex.back().t2x == (16.0f+64.0f) / 256);
                sb_assert(m_vertexes2tex.back().t2y == (32.0f+128.0f) / 512);
                
            }
            
            {
                m_transform.translate(10,20);
                appendVertex2(100,200,0,0);
                sb_assert(m_vertexes2tex.back().x == 110.0f);
                sb_assert(m_vertexes2tex.back().y == 220.0f);
                sb_assert(m_vertexes2tex.back().t2x == (16.0f+64.0f) / 256);
                sb_assert(m_vertexes2tex.back().t2y == (32.0f+128.0f) / 512);
                
            }
            
            {
                m_transform.scale(2.0f,2.0f);
                appendVertex2(50,100,0,0);
                sb_assert(m_vertexes2tex.back().x == 110.0f);
                sb_assert(m_vertexes2tex.back().y == 220.0f);
                sb_assert(m_vertexes2tex.back().t2x == (16.0f+64.0f) / 256);
                sb_assert(m_vertexes2tex.back().t2y == (32.0f+128.0f) / 512);
                
            }
            
            {
                m_transform.scale(2.0f,2.0f);
                appendVertex2(0,0,0,0);
                sb_assert(m_vertexes2tex.back().x == 10.0f);
                sb_assert(m_vertexes2tex.back().y == 20.0f);
                sb_assert(m_vertexes2tex.back().t2x == (16.0f) / 256);
                sb_assert(m_vertexes2tex.back().t2y == (32.0f) / 512);
                
            }

            m_transform = Transform2d();
            m_transform.translate(10,20);
            m_transform.scale(2,2);
            SetMaskImageTransform(img, Rectf(0,0,50,100));
            
            m_transform = Transform2d();
            {
                appendVertex2(10,20,0,0);
                sb_assert(m_vertexes2tex.back().x == 10.0f);
                sb_assert(m_vertexes2tex.back().y == 20.0f);
                sb_assert(m_vertexes2tex.back().t2x == 16.0f / 256);
                sb_assert(m_vertexes2tex.back().t2y == 32.0f / 512);
                
            }
            
            m_transform = Transform2d();
            m_vertexes2tex.clear();
        }
#endif
	}
    
    void Graphics::SetFilter(DrawFilter* filter) {
        m_filter = filter;
    }
	
	void Graphics::SetBlendMode(BlendMode bmode) {
        sb_assert( (m_render!=0) && "scene not started" );
        m_state.blend_mode = bmode;
    }
    void Graphics::SetBlendModeI(BlendMode bmode) {
        switch (bmode) {
            case BLEND_MODE_COPY:
                m_render->SetupBlend(false, GHL::BLEND_FACTOR_ONE, GHL::BLEND_FACTOR_ZERO);
                break;
            case BLEND_MODE_ALPHABLEND:
                m_render->SetupBlend(true, GHL::BLEND_FACTOR_ONE, GHL::BLEND_FACTOR_SRC_ALPHA_INV);
                break;
            case BLEND_MODE_ADDITIVE:
                m_render->SetupBlend(true, GHL::BLEND_FACTOR_ONE, GHL::BLEND_FACTOR_ONE);
                break;
            case BLEND_MODE_MULTIPLY:
                m_render->SetupBlend(true, GHL::BLEND_FACTOR_DST_COLOR, GHL::BLEND_FACTOR_SRC_ALPHA_INV);
                break;
            case BLEND_MODE_ADDITIVE_ALPHA:
                m_render->SetupBlend(true, GHL::BLEND_FACTOR_ONE, GHL::BLEND_FACTOR_ONE);
                break;
            case BLEND_MODE_SCREEN:
                m_render->SetupBlend(true, GHL::BLEND_FACTOR_ONE, GHL::BLEND_FACTOR_SRC_COLOR_INV);
                break;
            default:
                sb_assert(false && "unknown blend mode");
                break;
        }
	}
    
    /// clear scene
    void Graphics::Clear( const Color& clr, float depth ) {
        sb_assert( (m_render!=0) && "scene not started" );
        CheckFlush(true);
        m_render->Clear(clr.r, clr.g, clr.b, clr.a, depth);
    }
    
    
    /// fill rect by pattern
    void Graphics::FillRect( const TexturePtr& texture, const Rectf& rect) {
        sb_assert( (m_render!=0) && "scene not started" );
        m_state.texture = texture;
        m_state.ptype = GHL::PRIMITIVE_TYPE_TRIANGLES;
        
        CheckFlush(false);
        
        appendQuad();
		
		GHL::UInt32 clr = (m_color).hw_premul();
        
        if (texture) {
            const GHL::Texture* t = texture->Present(m_resources);

            m_itw = 1.0f / t->GetWidth();
            m_ith = 1.0f / t->GetHeight();
        } else {
            m_itw = 1.0;
            m_ith = 1.0f;
        }
        
        const float tx = rect.x * m_itw;
        const float ty = rect.y * m_ith;
        const float tw = rect.w * m_itw;
        const float th = rect.h * m_ith;
        
        {
            appendVertex(rect.x,rect.y,
                         tx,ty,clr);
            appendVertex(rect.x+rect.w,rect.y,
                         tx+tw,ty,clr);
            appendVertex(rect.x,rect.y+rect.h,
                         tx,ty+th,clr);
            appendVertex(rect.x+rect.w,rect.y+rect.h,
                         tx+tw,ty+th,clr);
        } 
    }
	
	void Graphics::FlushImpl() {
		sb_assert( (m_render!=0) && "scene not started" );
        if (m_primitives==0) return;
        /// do batch
        SetTextureI(m_draw_state.texture ? m_draw_state.texture->Present(m_resources) : 0);
        if (m_draw_state.mask) {
            m_render->SetTexture(m_draw_state.mask->Present(m_resources),1);
        }
		
        const GHL::UInt16* indexes = m_indexes.empty() ? 0 : &m_indexes.front();
        if (!m_draw_state.calc2_tex) {
            m_render->DrawPrimitivesFromMemory(m_draw_state.ptype,
										   GHL::VERTEX_TYPE_SIMPLE,
											&m_vertexes.front(),
                                           static_cast<GHL::UInt32>(m_vertexes.size()),
                                           indexes,
                                           GHL::UInt32(m_primitives));
        } else {
            m_render->DrawPrimitivesFromMemory(m_draw_state.ptype,
                                               GHL::VERTEX_TYPE_2_TEX,
                                               &m_vertexes2tex.front(),
                                               static_cast<GHL::UInt32>(m_vertexes2tex.size()),
                                               indexes,
                                               GHL::UInt32(m_primitives));
        }
        m_batches++;
#ifdef PRINT_BATCHES
        LogDebug(MODULE) << "batch " << m_data->primitives;
#endif
        m_vertexes.clear();
        m_vertexes2tex.clear();
		m_indexes.clear();
		m_primitives = 0;
	}
    
    bool Graphics::CheckFlush(bool force) {
        bool res = force;
        if (force) {
            FlushImpl();
        }
        if (m_state.blend_mode != m_draw_state.blend_mode) {
            res = true;
            FlushImpl();
            SetBlendModeI(m_state.blend_mode);
        }
        if (m_state.mask != m_draw_state.mask ||
            m_state.mask_mode != m_draw_state.mask_mode ) {
            FlushImpl();
            if (!m_state.mask) {
                SetMaskModeI(m_state.mask_mode,0);
                m_render->SetTexture(0,1);
            } else {
                SetMaskModeI(m_state.mask_mode, m_state.mask->Present(m_resources));
            }
            res = true;
        }
        if (m_state.calc2_tex != m_draw_state.calc2_tex) {
            FlushImpl();
            res = true;
        }
        if (m_state.depth_test != m_draw_state.depth_test ||
            m_state.depth_write != m_draw_state.depth_write ) {
            FlushImpl();
            m_render->SetupDepthTest(m_state.depth_test,GHL::COMPARE_FUNC_LEQUAL,m_state.depth_write);
            res = true;
        }
        if (m_state.texture != m_draw_state.texture ||
            m_state.ptype != m_draw_state.ptype) {
            FlushImpl();
            res = true;
        }
        if (m_state.shader != m_draw_state.shader) {
            FlushImpl();
            SetShaderI(m_state.shader);
            res = true;
        }
        if (res) {
            m_draw_state = m_state;
            return true;
        }
        return false;
    }
    
    void Graphics::Flush() {
        CheckFlush(true);
    }
    
    void Graphics::BeginDrawTexture(const TexturePtr& texture) {
        if (m_state.texture != texture) {
            if (texture) {
                m_itw = 1.0f / texture->GetWidth();
                m_ith = 1.0f / texture->GetHeight();
            } else {
                m_itw = 1.0f;
                m_ith = 1.0f;
            }
        }
        m_state.texture = texture;
    }
	
	void Graphics::BeginDrawImage(const Image& img) {
        BeginDrawTexture(img.GetTexture());
        BeginDrawPrimitives(GHL::PRIMITIVE_TYPE_TRIANGLES);
    }
    
    void Graphics::BeginDrawPrimitives(GHL::PrimitiveType type) {
        m_state.ptype = type;
    }
	
    void Graphics::BeginDrawTriangles(const TexturePtr& texture) {
        BeginDrawTexture(texture);
        BeginDrawPrimitives(GHL::PRIMITIVE_TYPE_TRIANGLES);
        CheckFlush(false);
    }
    
    void Graphics::AppendVertex(const Vector2f& pos, const Vector2f& tex, const Color& clr) {
        GHL::UInt32 hclr = (m_color * clr).hw_premul();
        if (m_state.calc2_tex) {
            m_indexes.push_back(static_cast<GHL::UInt16>(m_vertexes2tex.size()));
            appendVertex2(pos.x,pos.y,
                          tex.x,
                          tex.y,hclr);
            if ((m_vertexes2tex.size() % 3) == 0) {
                m_primitives++;
            }
        } else {
            m_indexes.push_back(static_cast<GHL::UInt16>(m_vertexes.size()));
            appendVertex(pos.x,pos.y,
                         tex.x,
                         tex.y,hclr);
            if ((m_vertexes.size() % 3) == 0) {
                m_primitives++;
            }

        }
    }
    
    void Graphics::AppendQuad(const Vector2f& poslt, const Vector2f& texlt, const Color& clrlt,
                    const Vector2f& posrt, const Vector2f& texrt, const Color& clrrt,
                    const Vector2f& poslb, const Vector2f& texlb, const Color& clrlb,
                    const Vector2f& posrb, const Vector2f& texrb, const Color& clrrb) {
        appendQuad();
        if (m_state.calc2_tex) {
            appendVertex2(poslt.x,poslt.y,
                          texlt.x,texlt.y,
                          (m_color * clrlt).hw_premul());
            appendVertex2(posrt.x,posrt.y,
                          texrt.x,texrt.y,
                          (m_color * clrrt).hw_premul());
            appendVertex2(poslb.x,poslb.y,
                          texlb.x,texlb.y,
                          (m_color * clrlb).hw_premul());
            appendVertex2(posrb.x,posrb.y,
                          texrb.x,texrb.y,
                          (m_color * clrrb).hw_premul());
        } else {
            appendVertex(poslt.x,poslt.y,
                          texlt.x,texlt.y,
                          (m_color * clrlt).hw_premul());
            appendVertex(posrt.x,posrt.y,
                          texrt.x,texrt.y,
                          (m_color * clrrt).hw_premul());
            appendVertex(poslb.x,poslb.y,
                          texlb.x,texlb.y,
                          (m_color * clrlb).hw_premul());
            appendVertex(posrb.x,posrb.y,
                          texrb.x,texrb.y,
                          (m_color * clrrb).hw_premul());
        }
    }
    
	void Graphics::appendTriangle(GHL::Int16 i1,GHL::Int16 i2,GHL::Int16 i3) {
		GHL::UInt16 base = m_state.calc2_tex ? static_cast<GHL::UInt16>(m_vertexes2tex.size()) : static_cast<GHL::UInt16>(m_vertexes.size());
		m_indexes.push_back(base+i1);
		m_indexes.push_back(base+i2);
		m_indexes.push_back(base+i3);
		m_primitives++;
	}
	
	
	void Graphics::DrawImage(const Image& img,const DrawAttributes* attributes,float x,float y) {
		sb_assert( (m_render!=0) && "scene not started" );
        if (m_filter && !m_filter->DrawImage(*this, attributes, img, x, y, Sandbox::Color(), 1.0))
            return;
        BeginDrawImage(img);
		const float w = img.GetWidth();
		const float h = img.GetHeight();
		x-=img.GetHotspot().x*w/img.GetTextureDrawW();
        y-=img.GetHotspot().y*h/img.GetTextureDrawH();
		
        CheckFlush(false);
        
        appendQuad();
		
		GHL::UInt32 clr = (m_color).hw_premul();
        const float tx0 = img.GetTextureX()*m_itw;
        const float ty0 = img.GetTextureY()*m_ith;
        const float tx1 = tx0 + img.GetTextureW()*m_itw;
        const float ty1 = ty0 + img.GetTextureH()*m_ith;
        if (!m_state.calc2_tex)
        {
            if (img.GetRotated()) {
                appendVertex(x+w,y,
                             tx0,
                             ty0,clr);
                appendVertex(x+w,y+h,
                             tx1,
                             ty0,clr);
                appendVertex(x,y,
                             tx0,
                             ty1,clr);
                appendVertex(x,y+h,
                             tx1,
                             ty1,clr);
            } else {
                appendVertex(x,y,
                             tx0,
                             ty0,clr);
                appendVertex(x+w,y,
                             tx1,
                             ty0,clr);
                appendVertex(x,y+h,
                             tx0,
                             ty1,clr);
                appendVertex(x+w,y+h,
                             tx1,
                             ty1,clr);
            }
        }  else {
            appendVertex2(x,y,
                         tx0,
                         ty0,clr);
            appendVertex2(x+w,y,
                         tx1,
                         ty0,clr);
            appendVertex2(x,y+h,
                         tx0,
                         ty1,clr);
            appendVertex2(x+w,y+h,
                         tx1,
                         ty1,clr);
        }
	}
    
    void Graphics::DrawImage(const Image& img,const DrawAttributes* attributes,
                   const Vector2f& pos,
                   const Rectf& crop) {
        sb_assert( (m_render!=0) && "scene not started" );
        float x = pos.x;
        float y = pos.y;
        
        const float w = img.GetWidth();
        const float h = img.GetHeight();
        x-=img.GetHotspot().x*w/img.GetTextureDrawW();
        y-=img.GetHotspot().y*h/img.GetTextureDrawH();
        
        Rectf cropped = crop.GetIntersect(Rectf(x,y,w,h));
        if (cropped.w <= 0.0f || cropped.h <= 0.0f)
            return;
        
        if (m_filter && !m_filter->DrawImage(*this, attributes, img, x, y, Sandbox::Color(), 1.0))
            return;
        BeginDrawImage(img);
        
        CheckFlush(false);
        
        appendQuad();
        
        Rectf trect = Rectf(img.GetTextureX(),
                            img.GetTextureY(),
                            img.GetTextureW(),
                            img.GetTextureH());
        
        trect.x += ((cropped.x-x) / w) * trect.w;
        trect.y += ((cropped.y-y) / h) * trect.h;
       
        trect.w *= cropped.w / w;
        trect.h *= cropped.h / h;
        
        GHL::UInt32 clr = (m_color).hw_premul();
        
        if (!m_state.calc2_tex)
        {
            appendVertex(cropped.GetLeft(),cropped.GetTop(),
                         trect.GetLeft()*m_itw,
                         trect.GetTop()*m_ith,clr);
            appendVertex(cropped.GetRight(),cropped.GetTop(),
                         trect.GetRight()*m_itw,
                         trect.GetTop()*m_ith,clr);
            appendVertex(cropped.GetLeft(),cropped.GetBottom(),
                         trect.GetLeft()*m_itw,
                         trect.GetBottom()*m_ith,clr);
            appendVertex(cropped.GetRight(),cropped.GetBottom(),
                         trect.GetRight()*m_itw,
                         trect.GetBottom()*m_ith,clr);
        }  else {
            appendVertex2(cropped.GetLeft(),cropped.GetTop(),
                         trect.GetLeft()*m_itw,
                         trect.GetTop()*m_ith,clr);
            appendVertex2(cropped.GetRight(),cropped.GetTop(),
                         trect.GetRight()*m_itw,
                         trect.GetTop()*m_ith,clr);
            appendVertex2(cropped.GetLeft(),cropped.GetBottom(),
                         trect.GetLeft()*m_itw,
                         trect.GetBottom()*m_ith,clr);
            appendVertex2(cropped.GetRight(),cropped.GetBottom(),
                         trect.GetRight()*m_itw,
                         trect.GetBottom()*m_ith,clr);
        }
    }
	void Graphics::DrawImage(const Image& img,const DrawAttributes* attributes,float x,float y,const Color& _clr) {
		sb_assert( (m_render!=0) && "scene not started" );
        if (m_filter && !m_filter->DrawImage(*this,attributes, img, x, y, _clr, 1.0))
            return;
        BeginDrawImage(img);
        const float w = img.GetWidth();
		const float h = img.GetHeight();
		x-=img.GetHotspot().x*w/img.GetTextureDrawW();
        y-=img.GetHotspot().y*h/img.GetTextureDrawH();
        CheckFlush(false);
        
        appendQuad();
		
		GHL::UInt32 clr = (m_color*_clr).hw_premul();
		if (!m_state.calc2_tex)
        {
            appendVertex(x,y,
						 img.GetTextureX()*m_itw,
                         img.GetTextureY()*m_ith,clr);
            appendVertex(x+w,y,
						 img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                         img.GetTextureY()*m_ith,clr);
            appendVertex(x,y+h,
						 img.GetTextureX()*m_itw,
                         img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
            appendVertex(x+w,y+h,
						 img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                         img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
        } else {
            appendVertex2(x,y,
						 img.GetTextureX()*m_itw,
                         img.GetTextureY()*m_ith,clr);
            appendVertex2(x+w,y,
						 img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                         img.GetTextureY()*m_ith,clr);
            appendVertex2(x,y+h,
						 img.GetTextureX()*m_itw,
                         img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
            appendVertex2(x+w,y+h,
						 img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                         img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
        }
		
	}
	void Graphics::DrawImage(const Image& img,const DrawAttributes* attributes,float x,float y,const Color& _clr,float scale) {
		sb_assert( (m_render!=0) && "scene not started" );
        if (m_filter && !m_filter->DrawImage(*this, attributes, img, x, y, _clr, scale))
            return;
        BeginDrawImage(img);
		const float w = img.GetWidth()*scale;
		const float h = img.GetHeight()*scale;
		x-=img.GetHotspot().x*w/img.GetTextureDrawW();
        y-=img.GetHotspot().y*h/img.GetTextureDrawH();
        
        CheckFlush(false);
        
        appendQuad();
		
		GHL::UInt32 clr = (m_color*_clr).hw_premul();
		
        if (!m_state.calc2_tex)
        {
            if (img.GetRotated()) {
                appendVertex(x+w,y,
                             img.GetTextureX()*m_itw,
                             img.GetTextureY()*m_ith,clr);
                appendVertex(x+w,y+h,
                             img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                             img.GetTextureY()*m_ith,clr);
                appendVertex(x,y,
                             img.GetTextureX()*m_itw,
                             img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
                appendVertex(x,y+h,
                             img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                             img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
            } else {
                appendVertex(x,y,
                             img.GetTextureX()*m_itw,
                             img.GetTextureY()*m_ith,clr);
                appendVertex(x+w,y,
                             img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                             img.GetTextureY()*m_ith,clr);
                appendVertex(x,y+h,
                             img.GetTextureX()*m_itw,
                             img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
                appendVertex(x+w,y+h,
                             img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                             img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
            }
        } else {
            appendVertex2(x,y,
                          img.GetTextureX()*m_itw,
                          img.GetTextureY()*m_ith,clr);
            appendVertex2(x+w,y,
                          img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                          img.GetTextureY()*m_ith,clr);
            appendVertex2(x,y+h,
                          img.GetTextureX()*m_itw,
                          img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
            appendVertex2(x+w,y+h,
                          img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                          img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);

        }
		
	}
    
    
    void Graphics::DrawQuadRaw(const Image& img,const DrawAttributes* attributes,
                            const Vector2f& poslt,const Vector2f& posrt,
                               const Vector2f& poslb,const Vector2f& posrb, GHL::UInt32 clr) {
        sb_assert( (m_render!=0) && "scene not started" );
        BeginDrawImage(img);
        
        CheckFlush(false);
        
        appendQuad();
        
        
        if (!m_state.calc2_tex)
        {
            appendVertexRaw(poslt.x,poslt.y,
                         img.GetTextureX()*m_itw,
                         img.GetTextureY()*m_ith,clr);
            appendVertexRaw(posrt.x,posrt.y,
                         img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                         img.GetTextureY()*m_ith,clr);
            appendVertexRaw(poslb.x,poslb.y,
                         img.GetTextureX()*m_itw,
                         img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
            appendVertexRaw(posrb.x,posrb.y,
                         img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                         img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
        } else {
            appendVertex2Raw(poslt.x,poslt.y,
                          img.GetTextureX()*m_itw,
                          img.GetTextureY()*m_ith,clr);
            appendVertex2Raw(posrt.x,posrt.y,
                          img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                          img.GetTextureY()*m_ith,clr);
            appendVertex2Raw(poslb.x,poslb.y,
                          img.GetTextureX()*m_itw,
                          img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
            appendVertex2Raw(posrb.x,posrb.y,
                          img.GetTextureX()*m_itw+img.GetTextureW()*m_itw,
                          img.GetTextureY()*m_ith+img.GetTextureH()*m_ith,clr);
            
        }

    }
    
    void Graphics::DrawImageBox(const ImageBox& img, const DrawAttributes* attributes,const Vector2f& pos, const Sizef& size) {
        sb_assert( (m_render!=0) && "scene not started" );
        BeginDrawImage(img);
        float x1 = pos.x;
        float y1 = pos.y;
		const float w = img.GetWidth();
		const float h = img.GetHeight();
        const float hsx = img.GetHotspot().x*w/img.GetTextureW();
        const float hsy = img.GetHotspot().y*h/img.GetTextureH();
        
		x1-=hsx;
        y1-=hsy;
        
		float x4 = pos.x + size.w + hsx;
        float y4 = pos.y + size.h + hsy;
        
        float osl = img.GetOffsetL()*w/img.GetTextureW();
        float osr = img.GetOffsetR()*w/img.GetTextureW();
        float ost = img.GetOffsetT()*h/img.GetTextureH();
        float osb = img.GetOffsetB()*h/img.GetTextureH();
        
        if ( size.w < (osl + osr) ) {
            float sx = size.w / (osl + osr);
            osl *= sx;
            osr *= sx;
        }
        if (size.h < (ost + osb)) {
            float sy = size.h / (ost + osb);
            ost *= sy;
            osb *= sy;
        }
        
        float x2 = x1 + osl;
        float x3 = x4 - osr;
        float y2 = y1 + ost;
        float y3 = y4 - osb;
        
		
        float tx1 = img.GetTextureX() * m_itw;
        float tx2 = (img.GetTextureX()+img.GetOffsetL()) * m_itw;
        float tx3 = (img.GetTextureX()+img.GetTextureW()-img.GetOffsetR()) * m_itw;
        float tx4 = (img.GetTextureX()+img.GetTextureW()) * m_itw;
        
        float ty1 = img.GetTextureY() * m_ith;
        float ty2 = (img.GetTextureY()+img.GetOffsetT()) * m_ith;
        float ty3 = (img.GetTextureY()+img.GetTextureH()-img.GetOffsetB()) * m_ith;
        float ty4 = (img.GetTextureY()+img.GetTextureH()) * m_ith;
        
        int count_x = 1;
        float stepx = x3-x2;
        float stepy = y3-y2;
        if (img.GetTileH()) {
            float sx = (img.GetTextureW()-img.GetOffsetL()-img.GetOffsetR())*w/img.GetTextureW();
            count_x = int(::roundf(stepx/sx));
            stepx = stepx / count_x;
        }
        int count_y = 1;
        if (img.GetTileV()) {
            float sy = (img.GetTextureH()-img.GetOffsetT()-img.GetOffsetB())*h/img.GetTextureH();
            count_y = int(::roundf(stepy/sy));
            stepy = stepy / count_y;
        }
        
		GHL::UInt32 clr = m_color.hw_premul();
		
        CheckFlush(false);
        
        appendQuad();
        if (!m_state.calc2_tex)
        {
            appendVertex(x1,y1,tx1,ty1,clr);
            appendVertex(x2,y1,tx2,ty1,clr);
            appendVertex(x1,y2,tx1,ty2,clr);
            appendVertex(x2,y2,tx2,ty2,clr);
        } else {
            appendVertex2(x1,y1,tx1,ty1,clr);
            appendVertex2(x2,y1,tx2,ty1,clr);
            appendVertex2(x1,y2,tx1,ty2,clr);
            appendVertex2(x2,y2,tx2,ty2,clr);
        }
        
        float lx2 = x2;
        float lx3 = x2+stepx;
        for (int i=0;i<count_x;++i) {
            appendQuad();
            if (!m_state.calc2_tex)
            {
                appendVertex(lx2,y1,tx2,ty1,clr);
                appendVertex(lx3,y1,tx3,ty1,clr);
                appendVertex(lx2,y2,tx2,ty2,clr);
                appendVertex(lx3,y2,tx3,ty2,clr);
            } else {
                appendVertex2(lx2,y1,tx2,ty1,clr);
                appendVertex2(lx3,y1,tx3,ty1,clr);
                appendVertex2(lx2,y2,tx2,ty2,clr);
                appendVertex2(lx3,y2,tx3,ty2,clr);
            }
            lx2 += stepx;
            lx3 += stepx;
        }
        
        appendQuad();
        if (!m_state.calc2_tex)
        {
            appendVertex(x3,y1,tx3,ty1,clr);
            appendVertex(x4,y1,tx4,ty1,clr);
            appendVertex(x3,y2,tx3,ty2,clr);
            appendVertex(x4,y2,tx4,ty2,clr);
        } else {
            appendVertex2(x3,y1,tx3,ty1,clr);
            appendVertex2(x4,y1,tx4,ty1,clr);
            appendVertex2(x3,y2,tx3,ty2,clr);
            appendVertex2(x4,y2,tx4,ty2,clr);
        }
        
        
        
        float ly2 = y2;
        float ly3 = y2+stepy;
        for (int j=0;j<count_y;++j) {
            
            appendQuad();
            if (!m_state.calc2_tex)
            {
                appendVertex(x1,ly2,tx1,ty2,clr);
                appendVertex(x2,ly2,tx2,ty2,clr);
                appendVertex(x1,ly3,tx1,ty3,clr);
                appendVertex(x2,ly3,tx2,ty3,clr);
            } else {
                appendVertex2(x1,ly2,tx1,ty2,clr);
                appendVertex2(x2,ly2,tx2,ty2,clr);
                appendVertex2(x1,ly3,tx1,ty3,clr);
                appendVertex2(x2,ly3,tx2,ty3,clr);
            }
            
            lx2 = x2;
            lx3 = x2+stepx;
            for (int i=0;i<count_x;++i) {
                appendQuad();
                if (!m_state.calc2_tex)
                {
                    appendVertex(lx2,ly2,tx2,ty2,clr);
                    appendVertex(lx3,ly2,tx3,ty2,clr);
                    appendVertex(lx2,ly3,tx2,ty3,clr);
                    appendVertex(lx3,ly3,tx3,ty3,clr);
                } else {
                    appendVertex2(lx2,ly2,tx2,ty2,clr);
                    appendVertex2(lx3,ly2,tx3,ty2,clr);
                    appendVertex2(lx2,ly3,tx2,ty3,clr);
                    appendVertex2(lx3,ly3,tx3,ty3,clr);
                }
                lx2 += stepx;
                lx3 += stepx;
            }
            
            
            appendQuad();
            if (!m_state.calc2_tex)
            {
                appendVertex(x3,ly2,tx3,ty2,clr);
                appendVertex(x4,ly2,tx4,ty2,clr);
                appendVertex(x3,ly3,tx3,ty3,clr);
                appendVertex(x4,ly3,tx4,ty3,clr);
            } else {
                appendVertex2(x3,ly2,tx3,ty2,clr);
                appendVertex2(x4,ly2,tx4,ty2,clr);
                appendVertex2(x3,ly3,tx3,ty3,clr);
                appendVertex2(x4,ly3,tx4,ty3,clr);
            }
            
            ly2 += stepy;
            ly3 += stepy;
        }
        
        
        
        appendQuad();
        if (!m_state.calc2_tex)
        {
            appendVertex(x1,y3,tx1,ty3,clr);
            appendVertex(x2,y3,tx2,ty3,clr);
            appendVertex(x1,y4,tx1,ty4,clr);
            appendVertex(x2,y4,tx2,ty4,clr);
        } else {
            appendVertex2(x1,y3,tx1,ty3,clr);
            appendVertex2(x2,y3,tx2,ty3,clr);
            appendVertex2(x1,y4,tx1,ty4,clr);
            appendVertex2(x2,y4,tx2,ty4,clr);
        }
        
        lx2 = x2;
        lx3 = x2+stepx;
        for (int i=0;i<count_x;++i) {
            appendQuad();
            if (!m_state.calc2_tex)
            {
                appendVertex(lx2,y3,tx2,ty3,clr);
                appendVertex(lx3,y3,tx3,ty3,clr);
                appendVertex(lx2,y4,tx2,ty4,clr);
                appendVertex(lx3,y4,tx3,ty4,clr);
            } else {
                appendVertex2(lx2,y3,tx2,ty3,clr);
                appendVertex2(lx3,y3,tx3,ty3,clr);
                appendVertex2(lx2,y4,tx2,ty4,clr);
                appendVertex2(lx3,y4,tx3,ty4,clr);
            }
            lx2 += stepx;
            lx3 += stepx;
        }
        
        
        appendQuad();
        if (!m_state.calc2_tex)
        {
            appendVertex(x3,y3,tx3,ty3,clr);
            appendVertex(x4,y3,tx4,ty3,clr);
            appendVertex(x3,y4,tx3,ty4,clr);
            appendVertex(x4,y4,tx4,ty4,clr);
        } else {
            appendVertex2(x3,y3,tx3,ty3,clr);
            appendVertex2(x4,y3,tx4,ty3,clr);
            appendVertex2(x3,y4,tx3,ty4,clr);
            appendVertex2(x4,y4,tx4,ty4,clr);
        }

        
    }
	
	void Graphics::BeginDrawLines() {
		m_state.texture = TexturePtr();
        m_state.ptype = GHL::PRIMITIVE_TYPE_LINES;
	}
	void Graphics::DrawLine(const Vector2f& from, const Vector2f& to) {
		sb_assert( (m_render!=0) && "scene not started" );
#ifndef GHL_PLATFORM_FLASH
		BeginDrawLines();
        CheckFlush(false);
		GHL::UInt32 clr = m_color.hw_premul();
		GHL::UInt16 base = static_cast<GHL::UInt16>(m_vertexes.size());
		m_indexes.push_back(base+0);
		m_indexes.push_back(base+1);
		m_primitives++;
		appendVertex(from.x, from.y, 0, 0, clr);
		appendVertex(to.x,to.y,0,0,clr);
#endif
	}
	void Graphics::DrawLine(const Vector2f& from, const Vector2f& to,const Color& clr_) {
		sb_assert( (m_render!=0) && "scene not started" );
#ifndef GHL_PLATFORM_FLASH
		BeginDrawLines();
        CheckFlush(false);
		GHL::UInt32 clr = (m_color*clr_).hw_premul();
		GHL::UInt16 base = static_cast<GHL::UInt16>(m_vertexes.size());
		m_indexes.push_back(base+0);
		m_indexes.push_back(base+1);
		m_primitives++;
		appendVertex(from.x, from.y, 0, 0, clr);
		appendVertex(to.x,to.y,0,0,clr);
#endif
	}
	
	void Graphics::DrawLineStrip(const std::vector<Vector2f>& points) {
		sb_assert( (m_render!=0) && "scene not started" );
#ifndef GHL_PLATFORM_FLASH
        if (points.size()<2) return;
		m_state.texture = TexturePtr();
        m_state.ptype = GHL::PRIMITIVE_TYPE_LINE_STRIP;
        CheckFlush(false);
		GHL::UInt32 clr = m_color.hw_premul();
		GHL::UInt16 base = static_cast<GHL::UInt16>(m_vertexes.size());
		for (GHL::UInt16 i=0;i<static_cast<GHL::UInt16>(points.size());i++) {
			m_indexes.push_back(base+i);
		}
		for (size_t i=0;i<points.size();i++) {
			appendVertex(points[i].x, points[i].y, 0, 0, clr);
		}
        m_primitives = GHL::UInt32(points.size())-1;
#endif
	}
	void Graphics::DrawLineStrip(const std::vector<Vector2f>& points,const Color& clr_) {
		sb_assert( (m_render!=0) && "scene not started" );
#ifndef GHL_PLATFORM_FLASH
        if (points.size()<2) return;
		m_state.texture = TexturePtr();
        m_state.ptype = GHL::PRIMITIVE_TYPE_LINE_STRIP;
        CheckFlush(false);
        GHL::UInt32 clr = (m_color*clr_).hw_premul();
		GHL::UInt16 base = static_cast<GHL::UInt16>(m_vertexes.size());
		for (GHL::UInt16 i=0;i<static_cast<GHL::UInt16>(points.size());i++) {
			m_indexes.push_back(base+i);
		}
		for (size_t i=0;i<points.size();i++) {
			appendVertex(points[i].x, points[i].y, 0, 0, clr);
		}
		m_primitives = GHL::UInt32(points.size())-1;
#endif
	}
	
	void Graphics::BeginDrawCircle() {
		m_state.texture = TexturePtr();
        m_state.ptype = GHL::PRIMITIVE_TYPE_LINE_STRIP;
	}
	void Graphics::DrawCircle(const Vector2f& pos, float r) {
		sb_assert( (m_render!=0) && "scene not started" );
#ifndef GHL_PLATFORM_FLASH
		BeginDrawCircle();
		GHL::UInt32 clr = m_color.hw_premul();
		DrawCircle(pos,r,clr);
#endif
	}
	void Graphics::DrawCircle(const Vector2f& pos, float r,const Color& clr_) {
		sb_assert( (m_render!=0) && "scene not started" );
#ifndef GHL_PLATFORM_FLASH
		BeginDrawCircle();
		GHL::UInt32 clr = (m_color*clr_).hw_premul();
		DrawCircle(pos,r,clr);
#endif
	}
	void Graphics::DrawCircle(const Vector2f& pos, float r,GHL::UInt32 clr) {
#ifndef GHL_PLATFORM_FLASH
        CheckFlush(false);
        if (r < 0.0f) {
            r = r * -1.0f;
        }
		size_t subdivs = int(2*M_PI*r)/5;
		if (subdivs<8) subdivs = 8;
		const float step = static_cast<float>(M_PI*2/subdivs);
		GHL::UInt16 base = static_cast<GHL::UInt16>(m_vertexes.size());
		for (GHL::UInt16 i=0;i<subdivs;i++) {
			m_indexes.push_back(base+i);
		}
		m_indexes.push_back(base);
		float a = 0;
		for (size_t i=0;i<subdivs;i++) {
			float x = pos.x + r * sinf(a);
			float y = pos.y + r * cosf(a);
			a+=step;
			appendVertex(x, y, 0, 0, clr);
		}
		m_primitives+=subdivs;
#endif
	}
	
    void Graphics::DrawParticles( const std::vector<Particle>& particles,
                       const std::vector<ImagePtr>& images ) {
        if (particles.empty()) return;
        if (images.empty()) return;
        
        sb_assert( (m_render!=0) && "scene not started" );
        const Image* prev_image = 0;
        for ( size_t i=0;i<particles.size();i++) {
            const Particle& p = particles[i];
        
            const Image* _img = images[ p.image % images.size() ].get();
            if (!_img) continue;
            
            const Image& img = *_img;
            if (prev_image!=_img) {
                BeginDrawImage(img);
                CheckFlush(false);
            }
            
            const float x = p.pos.x - img.GetHotspot().x * p.scale;
            const float y = p.pos.y - img.GetHotspot().y * p.scale;
            const float w = img.GetWidth() * p.scale;
            const float h = img.GetHeight() * p.scale;
            
            
            appendQuad();
            
            GHL::UInt32 clr = (m_color * p.color).hw_premul();
            
            if (!m_state.calc2_tex)
            {
                appendVertex(x,y,
                             img.GetTextureX()*m_itw,img.GetTextureY()*m_ith,clr);
                appendVertex(x+w,y,
                             (img.GetTextureX()+img.GetTextureW())*m_itw,img.GetTextureY()*m_ith,clr);
                appendVertex(x,y+h,
                             img.GetTextureX()*m_itw,(img.GetTextureY()+img.GetTextureH())*m_ith,clr);
                appendVertex(x+w,y+h,
                             (img.GetTextureX()+img.GetTextureW())*m_itw,(img.GetTextureY()+img.GetTextureH())*m_ith,clr);
            } else {
                appendVertex2(x,y,
                             img.GetTextureX()*m_itw,img.GetTextureY()*m_ith,clr);
                appendVertex2(x+w,y,
                             (img.GetTextureX()+img.GetTextureW())*m_itw,img.GetTextureY()*m_ith,clr);
                appendVertex2(x,y+h,
                             img.GetTextureX()*m_itw,(img.GetTextureY()+img.GetTextureH())*m_ith,clr);
                appendVertex2(x+w,y+h,
                             (img.GetTextureX()+img.GetTextureW())*m_itw,(img.GetTextureY()+img.GetTextureH())*m_ith,clr);
            }

        }
    }

	
	void Graphics::DrawGeometry(const GeometryData& geomentry,bool transform) {
        BeginDrawTexture(geomentry.texture);
        BeginDrawPrimitives(geomentry.primitives);
        CheckFlush(!transform);
        size_t index_offset = m_vertexes.size();
        if (transform)
		{
            for (size_t i=0;i<geomentry.vertexes.size();i++) {
                appendVertex(geomentry.vertexes[i].x,
							 geomentry.vertexes[i].y,
							 geomentry.vertexes[i].tx,
							 geomentry.vertexes[i].ty,
							 (Color(geomentry.vertexes[i].color)*m_color).hw_premul());
            }
        }
        
        if (m_state.ptype==GHL::PRIMITIVE_TYPE_TRIANGLES)
            m_primitives += static_cast<GHL::UInt32>(geomentry.indexes.size())/3;
        else if (m_state.ptype==GHL::PRIMITIVE_TYPE_TRIANGLE_STRIP){
            sb_assert(geomentry.indexes.size()>2);
            m_primitives += static_cast<GHL::UInt32>(geomentry.indexes.size()-2);
        }
        
        if (transform) {
            for (sb::vector<GHL::UInt16>::const_iterator it = geomentry.indexes.begin();it!=geomentry.indexes.end();++it) {
                m_indexes.push_back(GHL::UInt16(*it+index_offset));
            }
        } else {
            
            
            if (m_state.texture) {
                SetTextureI(m_state.texture->Present(m_resources));
            } else {
                SetTextureI(0);
            }
            m_render->DrawPrimitivesFromMemory(m_state.ptype, GHL::VERTEX_TYPE_SIMPLE, &geomentry.vertexes[0], GHL::UInt32(geomentry.vertexes.size()), &geomentry.indexes[0], m_primitives);
            m_primitives = 0;
        }
		
	}
	
	void Graphics::SetShader(const ShaderPtr& sh) {
		m_state.shader = sh;
	}
    void Graphics::SetShaderI(const ShaderPtr& sh) {
        if (sh)
            sh->Set(m_render);
        else
            m_render->SetShader(0);
    }
    
    void Graphics::SetDepthWrite(bool write) {
        m_state.depth_write = write;
    }
    void Graphics::SetDepthTest(bool test) {
        m_state.depth_test = test;
    }
    
    void Graphics::SetMask(MaskMode mode, const TexturePtr& mask_tex,const Transform2d& tr) {
        SetMaskTexture(mask_tex);
        SetMaskMode(mode);
        m_mask_transform = tr;
    }
    
    void Graphics::StoreMask(MaskContext& ctx) const {
        ctx.mode = GetMaskMode();
        ctx.texture = GetMaskTexture();
        ctx.tr = GetMaskTransform();
    }
    void Graphics::RestoreMask(const MaskContext& ctx) {
        SetMask(ctx.mode, ctx.texture, ctx.tr);
    }
    
    void Graphics::SetMask(MaskMode mode, const Image& mask_img,const Rectf& rect) {
        SetMaskTexture(mask_img.GetTexture());
        SetMaskMode(mode);
        SetMaskImageTransform(mask_img,rect);
    }
    
    void Graphics::SetMaskImageTransform(const Image& img,const Rectf& r) {
        Transform2d tr;
        tr.translate(img.GetTextureX(), img.GetTextureY());
        tr.scale(img.GetTextureW()/r.w, img.GetTextureH()/r.h);
        tr.translate(-Vector2f(r.GetTopLeft()));
        tr*=m_transform.inverted();
        m_mask_transform = tr;
    }
    
    void Graphics::SetMaskTexture(const TexturePtr& tex,bool autocalc) {
        m_state.mask = tex;
        bool needCalc2 = autocalc && tex && tex!=m_fake_tex_black && tex!=m_fake_tex_white;
        m_state.calc2_tex =needCalc2;
        if (tex) {
            m_mask_itw = 1.0f / tex->GetWidth();
            m_mask_ith = 1.0f / tex->GetHeight();
        }
    }
    void Graphics::SetMaskMode(MaskMode mode) {
        if (!m_state.mask && mode!=MASK_MODE_NONE) {
            SetMaskTexture(m_fake_tex_black);
        }
        m_state.mask_mode = mode;
    }
    void Graphics::SetTextureI(GHL::Texture* tex) {
        if (tex) {
            m_render->SetTexture(tex,0);
            if (tex->GetFormat() == GHL::TEXTURE_FORMAT_ALPHA) {
                m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE_ALPHA,GHL::TEX_ARG_CURRENT,0);
            } else {
                m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
            }
        } else {
            m_render->SetTexture(0,0);
        }
    }
    void Graphics::SetMaskModeI(MaskMode mode,GHL::Texture* tex) {
        GHL::TextureArgument tex_arg = GHL::TEX_ARG_TEXTURE;
        if (tex && tex->GetFormat() == GHL::TEXTURE_FORMAT_ALPHA)
            tex_arg = GHL::TEX_ARG_TEXTURE_ALPHA;
        switch (mode) {
            case MASK_MODE_NONE:
                m_render->SetupTextureStageColorOp(GHL::TEX_OP_DISABLE,tex_arg,GHL::TEX_ARG_CURRENT,1);
                m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_DISABLE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
                break;
                
            case MASK_MODE_ALPHA:
                m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE,tex_arg,GHL::TEX_ARG_CURRENT,1);
                m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
                break;
                
            case MASK_MODE_SCREEN:
                m_render->SetupTextureStageColorOp(GHL::TEX_OP_INT_CURRENT_ALPHA,tex_arg,GHL::TEX_ARG_CURRENT,1);
                m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_SELECT_2,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
                break;
            case MASK_MODE_MULTIPLY:
                // alpha 0 -> 1
                // aplha 1 -> current
                m_render->SetupTextureStageColorOp(GHL::TEX_OP_INT_CURRENT_ALPHA,tex_arg,GHL::TEX_ARG_CURRENT,1);
                m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_SELECT_2,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
                break;

        }
    }
  	
	void Graphics::SetProjectionMatrix(const Matrix4f& m) {
		CheckFlush(true);
		m_projection_matrix = m;
		m_render->SetProjectionMatrix(m.matrix);
	}
	
	void Graphics::SetViewMatrix(const Matrix4f& m) {
		CheckFlush(true);
		m_view_matrix = m;
		m_render->SetViewMatrix(m.matrix);
	}
	
	void Graphics::SetViewport(const Recti& rect) {
        CheckFlush(true);
		m_viewport = rect;
	}
	
	void Graphics::SetClipRect(const Recti& rect) {
		CheckFlush(true);
        m_clip_rect = rect;
		if (m_clip_rect==m_viewport) {
			m_render->SetupScisor( false );
		} else {
            float draw_scale = (m_render_to_target ? m_render_to_target->GetScale() : m_scale * m_resources->GetScale()) ;
			m_render->SetupScisor( true, 
				GHL::UInt32(rect.x * draw_scale),
				GHL::UInt32(rect.y * draw_scale), 
				GHL::UInt32(rect.w * draw_scale),
				GHL::UInt32(rect.h*draw_scale));
		}
	}
		
    GHL::UInt32 Graphics::GetDrawWidth() const {
        float draw_scale = m_scale * (m_resources ? m_resources->GetScale() : 1.0f);
        if (m_render_to_target) {
            draw_scale = m_render_to_target->GetScale();
        }
        return GHL::UInt32((m_render ? m_render->GetWidth() : 0) / draw_scale);
    }
    
    GHL::UInt32 Graphics::GetDrawHeight() const {
        float draw_scale = m_scale * (m_resources ? m_resources->GetScale() : 1.0f);
        if (m_render_to_target) {
            draw_scale = m_render_to_target->GetScale();
        }
        return GHL::UInt32((m_render ? m_render->GetHeight() : 0) / draw_scale);
    }
		
    void Graphics::BeginScene(GHL::Render* render, const RenderTargetPtr& target) {
        sb_assert( (m_render==0) && "scene already started" );
        sb_assert( render );
		m_render = render;
        m_render_to_target = target;
		m_state.blend_mode = BLEND_MODE_COPY;
		m_render->SetupBlend(false);
		m_state.texture = TexturePtr();
        m_state.mask = TexturePtr();
        m_state.mask_mode = MASK_MODE_NONE;
		m_render->SetTexture(0,0);
		m_state.ptype = GHL::PRIMITIVE_TYPE_TRIANGLES;
		m_vertexes.clear();
		m_indexes.clear();
		m_primitives = 0;
		m_batches = 0;
		m_color = Color(1,1,1,1);
		m_transform = Transform2d();
		m_state.shader = ShaderPtr();
        m_state.calc2_tex = false;
        m_state.depth_test = false;
        m_state.depth_write = false;
        m_state.texture = TexturePtr();
        
        m_draw_state = m_state;
        
        float draw_scale = (target ? target->GetScale() : m_scale * m_resources->GetScale()) ;
        
		SetProjectionMatrix(Matrix4f::ortho(0,float(render->GetWidth())/draw_scale,
											float(render->GetHeight())/draw_scale,0,-10,10));
		SetViewMatrix(Matrix4f::identity());
		SetViewport(Recti(0,0,
			int(render->GetWidth() / draw_scale) ,
			int(render->GetHeight() / draw_scale) ));
		SetClipRect(GetViewport());
        m_render->SetTexture(0,0);
        m_render->SetTexture(0,1);
        m_render->SetIndexBuffer(0);
        m_render->SetVertexBuffer(0);
        m_render->SetShader(0);
        m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
        m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
        m_render->SetupDepthTest(m_state.depth_test,GHL::COMPARE_FUNC_LEQUAL,m_state.depth_write);
        m_render->SetupFaceCull(false);
        m_draw_state = m_state;
    }
	size_t Graphics::EndScene() {
		sb_assert( (m_render!=0) && "scene not started" );
		CheckFlush(true);
        m_render->SetTexture(0,0);
        m_render->SetTexture(0,1);
		m_render->SetShader(0);
		m_state.texture = TexturePtr();
        m_state.mask = TexturePtr();
		m_state.shader = ShaderPtr();
		m_render = 0;
        m_state.mask_mode = MASK_MODE_NONE;
        m_draw_state = m_state;
        return m_batches;
	}
	
	
	GHL::Render* Graphics::BeginNative() {
        sb_assert( (m_render!=0) && "scene not started" );
        CheckFlush(true);
        GHL::Render* render = m_render;
		if (render) {
			EndScene();
		}
		return render;
	}
	
	void Graphics::EndNative(GHL::Render* render) {
        size_t prev_batches = m_batches;
		if (render) {
			BeginScene(render,m_render_to_target);
            m_batches+=prev_batches;
		}
	}
    
    const GHL::Texture* Graphics::Present( const TexturePtr& tex ) {
        if (!tex) return 0;
        return tex->Present(m_resources);
    }
	
    void Graphics::SetScale( float scale ) {
        m_scale = scale;
    }
    
}
