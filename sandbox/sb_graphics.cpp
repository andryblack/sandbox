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

namespace Sandbox {
	
    static const char* MODULE = "Sanbox:Graphics";
	
	Graphics::Graphics(Resources* res) : m_resources(res){
        (void)MODULE;
		m_render = 0;
		m_fake_tex_white = 0;
        m_vertexes.reserve(512);
        m_vertexes2tex.reserve(512);
		m_indexes.reserve(512);
	}
	
	Graphics::~Graphics() {
		if (m_fake_tex_white)
			m_fake_tex_white->Release();
        if (m_fake_tex_black)
            m_fake_tex_black->Release();
		m_fake_tex_white = 0;
	}
    
    	
	void Graphics::Load(GHL::Render* render) {
		if (!m_fake_tex_white) {
            GHL::Image* img = GHL_CreateImage(1, 1, GHL::IMAGE_FORMAT_RGBA);
            img->Fill(0xFFFFFFFF);
            m_fake_tex_white = render->CreateTexture(1,1,GHL::TEXTURE_FORMAT_RGBA,img);
			img->Release();
            m_fake_tex_white->DiscardInternal();
		}
        if (!m_fake_tex_black) {
            GHL::Image* img = GHL_CreateImage(1, 1, GHL::IMAGE_FORMAT_RGBA);
            img->Fill(0xFF000000);
            GHL::Texture* tex = render->CreateTexture(1,1,GHL::TEXTURE_FORMAT_RGBA,img);
            m_fake_tex_black = TexturePtr(new Texture(tex));
			img->Release();
            tex->DiscardInternal();
		}
	}
	
	void Graphics::SetBlendMode(BlendMode bmode) {
        sb_assert( (m_render!=0) && "scene not started" );
		if (m_blend_mode!=bmode) {
			Flush();
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
				case BLEND_MODE_ADDITIVE_ALPHA: 
					m_render->SetupBlend(true, GHL::BLEND_FACTOR_ONE, GHL::BLEND_FACTOR_ONE);
					break;
//				case BLEND_MODE_SCREEN:
//					m_render->SetTexture(m_fake_tex_black,1);
//					m_render->SetupBlend(true, GHL::BLEND_FACTOR_DST_COLOR_INV, GHL::BLEND_FACTOR_ONE);
//					m_render->SetupTextureStageColorOp(GHL::TEX_OP_INT_CURRENT_ALPHA,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
//					m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_SELECT_2,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
					break;
				default:
					sb_assert(false && "unknown blend mode");
					break;
			}
			m_blend_mode = bmode;
		}
		
	}
    
    /// clear scene
    void Graphics::Clear( const Color& clr, float depth ) {
        sb_assert( (m_render!=0) && "scene not started" );
        Flush();
        m_render->Clear(clr.r, clr.g, clr.b, clr.a, depth);
    }
    
    
    /// fill rect by pattern
    void Graphics::FillRect( const TexturePtr& texture, const Rectf& rect) {
        sb_assert( (m_render!=0) && "scene not started" );
        bool flush = false;
		if (m_texture!=texture) {
            flush = true;
        } else if (m_ptype!=GHL::PRIMITIVE_TYPE_TRIANGLES) {
            flush = true;
        }
        if (flush) {
            Flush();
        }
		
        m_texture = texture;
        m_ptype = GHL::PRIMITIVE_TYPE_TRIANGLES;
        
        
        appendQuad();
		
		GHL::UInt32 clr = (m_color).hw();
        
        const GHL::Texture* t = texture->Present(m_resources);
		
        m_itw = 1.0f / t->GetWidth();
        m_ith = 1.0f / t->GetHeight();
        
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
	
	void Graphics::Flush() {
		sb_assert( (m_render!=0) && "scene not started" );
        if (m_primitives==0) return;
        /// do batch
		m_render->SetTexture(m_texture ? m_texture->Present(m_resources) : 0,0);
        if (m_mask) {
            m_render->SetTexture(m_mask->Present(m_resources),1);
        }
		
        const GHL::UInt16* indexes = m_indexes.empty() ? 0 : &m_indexes.front();
        if (!m_calc2_tex) {
            m_render->DrawPrimitivesFromMemory(m_ptype,
										   GHL::VERTEX_TYPE_SIMPLE,
											&m_vertexes.front(),
                                           static_cast<GHL::UInt32>(m_vertexes.size()),
                                           indexes,
                                           GHL::UInt32(m_primitives));
        } else {
            m_render->DrawPrimitivesFromMemory(m_ptype,
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
	
	void Graphics::BeginDrawImage(const Image& img) {
        TexturePtr texture = img.GetTexture();
        bool flush = false;
		if (m_texture!=texture) {
            flush = true;
            const GHL::Texture* tex = texture->Present(m_resources);
            m_itw = 1.0f / tex->GetWidth();
            m_ith = 1.0f / tex->GetHeight();
        } else if (m_ptype!=GHL::PRIMITIVE_TYPE_TRIANGLES) {
            flush = true;
        }
        if (flush) {
            Flush();
        }
		
        m_texture = texture;
        m_ptype = GHL::PRIMITIVE_TYPE_TRIANGLES;
    }
	
	void Graphics::appendTriangle(GHL::Int16 i1,GHL::Int16 i2,GHL::Int16 i3) {
		GHL::UInt16 base = m_calc2_tex ? static_cast<GHL::UInt16>(m_vertexes2tex.size()) : static_cast<GHL::UInt16>(m_vertexes.size());
		m_indexes.push_back(base+i1);
		m_indexes.push_back(base+i2);
		m_indexes.push_back(base+i3);
		m_primitives++;
	}
	void Graphics::appendQuad() {
		GHL::UInt16 base = m_calc2_tex ? static_cast<GHL::UInt16>(m_vertexes2tex.size()) : static_cast<GHL::UInt16>(m_vertexes.size());
		m_indexes.push_back(base+0);
		m_indexes.push_back(base+1);
		m_indexes.push_back(base+2);
		m_indexes.push_back(base+2);
		m_indexes.push_back(base+1);
		m_indexes.push_back(base+3);
		m_primitives+=2;
	}
	
	void Graphics::DrawImage(const Image& img,float x,float y) {
		sb_assert( (m_render!=0) && "scene not started" );
        BeginDrawImage(img);
		const float w = img.GetWidth();
		const float h = img.GetHeight();
		x-=img.GetHotspot().x*w/img.GetTextureW();
        y-=img.GetHotspot().y*h/img.GetTextureH();
		
        appendQuad();
		
		GHL::UInt32 clr = (m_color).hw();
		
        if (!m_calc2_tex)
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
        }  else {
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
	void Graphics::DrawImage(const Image& img,float x,float y,const Color& _clr) {
		sb_assert( (m_render!=0) && "scene not started" );
        BeginDrawImage(img);
        const float w = img.GetWidth();
		const float h = img.GetHeight();
		x-=img.GetHotspot().x*w/img.GetTextureW();
        y-=img.GetHotspot().y*h/img.GetTextureH();
		
        appendQuad();
		
		GHL::UInt32 clr = (m_color*_clr).hw();
		if (!m_calc2_tex)
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
	void Graphics::DrawImage(const Image& img,float x,float y,const Color& _clr,float scale) {
		sb_assert( (m_render!=0) && "scene not started" );
        BeginDrawImage(img);
		const float w = img.GetWidth()*scale;
		const float h = img.GetHeight()*scale;
		x-=img.GetHotspot().x*w/img.GetTextureW();
        y-=img.GetHotspot().y*h/img.GetTextureH();
		
        appendQuad();
		
		GHL::UInt32 clr = (m_color*_clr).hw();
		
        if (!m_calc2_tex)
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
    
    void Graphics::DrawImageBox(const ImageBox& img, const Vector2f& pos, const Vector2f& size) {
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
        
		float x4 = pos.x + size.x + hsx;
        float y4 = pos.y + size.y + hsy;
        
        float osl = img.GetOffsetL()*w/img.GetTextureW();
        float osr = img.GetOffsetR()*w/img.GetTextureW();
        float ost = img.GetOffsetT()*h/img.GetTextureH();
        float osb = img.GetOffsetB()*h/img.GetTextureH();
        
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
            count_x = ::roundf(stepx/sx);
            stepx = stepx / count_x;
        }
        int count_y = 1;
        if (img.GetTileV()) {
            float sy = (img.GetTextureH()-img.GetOffsetT()-img.GetOffsetB())*h/img.GetTextureH();
            count_y = ::roundf(stepy/sy);
            stepy = stepy / count_y;
        }
        
		GHL::UInt32 clr = m_color.hw();
		
        appendQuad();
        {
            appendVertex(x1,y1,tx1,ty1,clr);
            appendVertex(x2,y1,tx2,ty1,clr);
            appendVertex(x1,y2,tx1,ty2,clr);
            appendVertex(x2,y2,tx2,ty2,clr);
        }
        
        float lx2 = x2;
        float lx3 = x2+stepx;
        for (int i=0;i<count_x;++i) {
            appendQuad();
            {
                appendVertex(lx2,y1,tx2,ty1,clr);
                appendVertex(lx3,y1,tx3,ty1,clr);
                appendVertex(lx2,y2,tx2,ty2,clr);
                appendVertex(lx3,y2,tx3,ty2,clr);
            }
            lx2 += stepx;
            lx3 += stepx;
        }
        
        appendQuad();
        {
            appendVertex(x3,y1,tx3,ty1,clr);
            appendVertex(x4,y1,tx4,ty1,clr);
            appendVertex(x3,y2,tx3,ty2,clr);
            appendVertex(x4,y2,tx4,ty2,clr);
        }
        
        
        
        float ly2 = y2;
        float ly3 = y2+stepy;
        for (int j=0;j<count_y;++j) {
            
            appendQuad();
            {
                appendVertex(x1,ly2,tx1,ty2,clr);
                appendVertex(x2,ly2,tx2,ty2,clr);
                appendVertex(x1,ly3,tx1,ty3,clr);
                appendVertex(x2,ly3,tx2,ty3,clr);
            }
            
            lx2 = x2;
            lx3 = x2+stepx;
            for (int i=0;i<count_x;++i) {
                appendQuad();
                {
                    appendVertex(lx2,ly2,tx2,ty2,clr);
                    appendVertex(lx3,ly2,tx3,ty2,clr);
                    appendVertex(lx2,ly3,tx2,ty3,clr);
                    appendVertex(lx3,ly3,tx3,ty3,clr);
                }
                lx2 += stepx;
                lx3 += stepx;
            }
            
            
            appendQuad();
            {
                appendVertex(x3,ly2,tx3,ty2,clr);
                appendVertex(x4,ly2,tx4,ty2,clr);
                appendVertex(x3,ly3,tx3,ty3,clr);
                appendVertex(x4,ly3,tx4,ty3,clr);
            }
            
            ly2 += stepy;
            ly3 += stepy;
        }
        
        
        
        appendQuad();
        {
            appendVertex(x1,y3,tx1,ty3,clr);
            appendVertex(x2,y3,tx2,ty3,clr);
            appendVertex(x1,y4,tx1,ty4,clr);
            appendVertex(x2,y4,tx2,ty4,clr);
        }
        
        lx2 = x2;
        lx3 = x2+stepx;
        for (int i=0;i<count_x;++i) {
            appendQuad();
            {
                appendVertex(lx2,y3,tx2,ty3,clr);
                appendVertex(lx3,y3,tx3,ty3,clr);
                appendVertex(lx2,y4,tx2,ty4,clr);
                appendVertex(lx3,y4,tx3,ty4,clr);
            }
            lx2 += stepx;
            lx3 += stepx;
        }
        
        
        appendQuad();
        {
            appendVertex(x3,y3,tx3,ty3,clr);
            appendVertex(x4,y3,tx4,ty3,clr);
            appendVertex(x3,y4,tx3,ty4,clr);
            appendVertex(x4,y4,tx4,ty4,clr);
        }

        
    }
	
	void Graphics::BeginDrawLines() {
		bool flush = false;
		if (m_texture) {
            flush = true;
        } else if (m_ptype!=GHL::PRIMITIVE_TYPE_LINES) {
            flush = true;
        }
        if (flush) {
            Flush();
        }
		m_texture = TexturePtr();
        m_ptype = GHL::PRIMITIVE_TYPE_LINES;
	}
	void Graphics::DrawLine(const Vector2f& from, const Vector2f& to) {
		sb_assert( (m_render!=0) && "scene not started" );
#ifndef GHL_PLATFORM_FLASH
		BeginDrawLines();
		GHL::UInt32 clr = m_color.hw();
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
		GHL::UInt32 clr = (m_color*clr_).hw();
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
		Flush();
		m_texture = TexturePtr();
        m_ptype = GHL::PRIMITIVE_TYPE_LINE_STRIP;
		GHL::UInt32 clr = m_color.hw();
		GHL::UInt16 base = static_cast<GHL::UInt16>(m_vertexes.size());
		for (GHL::UInt16 i=0;i<static_cast<GHL::UInt16>(points.size());i++) {
			m_indexes.push_back(base+i);
		}
		for (size_t i=0;i<points.size();i++) {
			appendVertex(points[i].x, points[i].y, 0, 0, clr);
		}
		m_primitives = points.size()-1;
		Flush();
#endif
	}
	void Graphics::DrawLineStrip(const std::vector<Vector2f>& points,const Color& clr_) {
		sb_assert( (m_render!=0) && "scene not started" );
#ifndef GHL_PLATFORM_FLASH
        if (points.size()<2) return;
		Flush();
		m_texture = TexturePtr();
        m_ptype = GHL::PRIMITIVE_TYPE_LINE_STRIP;
		GHL::UInt32 clr = (m_color*clr_).hw();
		GHL::UInt16 base = static_cast<GHL::UInt16>(m_vertexes.size());
		for (GHL::UInt16 i=0;i<static_cast<GHL::UInt16>(points.size());i++) {
			m_indexes.push_back(base+i);
		}
		for (size_t i=0;i<points.size();i++) {
			appendVertex(points[i].x, points[i].y, 0, 0, clr);
		}
		m_primitives = points.size()-1;
		Flush();
#endif
	}
	
	void Graphics::BeginDrawCircle() {
		Flush();
		m_texture = TexturePtr();
        m_ptype = GHL::PRIMITIVE_TYPE_LINE_STRIP;
	}
	void Graphics::DrawCircle(const Vector2f& pos, float r) {
		sb_assert( (m_render!=0) && "scene not started" );
#ifndef GHL_PLATFORM_FLASH
		BeginDrawCircle();
		GHL::UInt32 clr = m_color.hw();
		DrawCircle(pos,r,clr);
		Flush();
#endif
	}
	void Graphics::DrawCircle(const Vector2f& pos, float r,const Color& clr_) {
		sb_assert( (m_render!=0) && "scene not started" );
#ifndef GHL_PLATFORM_FLASH
		BeginDrawCircle();
		GHL::UInt32 clr = (m_color*clr_).hw(); 
		DrawCircle(pos,r,clr);
		Flush();
#endif
	}
	void Graphics::DrawCircle(const Vector2f& pos, float r,GHL::UInt32 clr) {
#ifndef GHL_PLATFORM_FLASH
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
                       const std::vector<const Image*>& images ) {
        if (particles.empty()) return;
        if (images.empty()) return;
        
        sb_assert( (m_render!=0) && "scene not started" );
        const Image* prev_image = 0;
        for ( size_t i=0;i<particles.size();i++) {
            const Particle& p = particles[i];
        
            const Image* _img = images[ p.image % images.size() ];
            if (!_img) continue;
            
            const Image& img = *_img;
            if (prev_image!=_img)
                BeginDrawImage(img);
            
            const float x = p.pos.x - img.GetHotspot().x * p.scale;
            const float y = p.pos.y - img.GetHotspot().y * p.scale;
            const float w = img.GetWidth() * p.scale;
            const float h = img.GetHeight() * p.scale;
            
            appendQuad();
            
            GHL::UInt32 clr = (m_color * p.color).hw();
            
            if (!m_calc2_tex)
            {
                appendVertex(x,y,
                             img.GetTextureX(),img.GetTextureY(),clr);
                appendVertex(x+w,y,
                             img.GetTextureX()+img.GetTextureW(),img.GetTextureY(),clr);
                appendVertex(x,y+h,
                             img.GetTextureX(),img.GetTextureY()+img.GetTextureH(),clr);
                appendVertex(x+w,y+h,
                             img.GetTextureX()+img.GetTextureW(),img.GetTextureY()+img.GetTextureH(),clr);
            } else {
                appendVertex2(x,y,
                             img.GetTextureX(),img.GetTextureY(),clr);
                appendVertex2(x+w,y,
                             img.GetTextureX()+img.GetTextureW(),img.GetTextureY(),clr);
                appendVertex2(x,y+h,
                             img.GetTextureX(),img.GetTextureY()+img.GetTextureH(),clr);
                appendVertex2(x+w,y+h,
                             img.GetTextureX()+img.GetTextureW(),img.GetTextureY()+img.GetTextureH(),clr);
            }

        }
    }

	
	void Graphics::DrawGeometry(const GeometryData& geomentry,bool transform) {
		Flush();
        if (transform)
		{
            for (size_t i=0;i<geomentry.vertexes.size();i++) {
                appendVertex(geomentry.vertexes[i].x,
							 geomentry.vertexes[i].y,
							 geomentry.vertexes[i].tx,
							 geomentry.vertexes[i].ty,
							 (Color(*reinterpret_cast<const GHL::UInt32*>(geomentry.vertexes[i].color))*m_color).hw());
            }
        } 
		if (geomentry.texture) {
			m_render->SetTexture(geomentry.texture->Present(m_resources),0);
		} else {
			m_render->SetTexture(0,0);
		}
		
        
        m_texture = geomentry.texture;
        m_ptype = geomentry.primitives;
        
        if (m_ptype==GHL::PRIMITIVE_TYPE_TRIANGLES)
            m_primitives = static_cast<GHL::UInt32>(geomentry.indexes.size())/3;
        else if (m_ptype==GHL::PRIMITIVE_TYPE_TRIANGLE_STRIP){
            sb_assert(geomentry.indexes.size()>2);
            m_primitives = static_cast<GHL::UInt32>(geomentry.indexes.size()-2);
        }
        
        if (transform) {
            m_indexes = geomentry.indexes;
        } else {
            m_render->DrawPrimitivesFromMemory(geomentry.primitives, GHL::VERTEX_TYPE_SIMPLE, &geomentry.vertexes[0], geomentry.vertexes.size(), &geomentry.indexes[0], m_primitives);
            m_primitives = 0;
        }
		
        
        Flush();
	}
	
	void Graphics::SetShader(const ShaderPtr& sh) {
		Flush();
		m_shader = sh;
		if (sh)
			sh->Set(m_render);
		else
			m_render->SetShader(0);

	}
    
    void Graphics::SetMask(MaskMode mode, const TexturePtr& mask_tex,const Transform2d& tr) {
        SetMaskTexture(mask_tex);
        SetMaskMode(mode);
        m_mask_transform = tr;
    }
    void Graphics::SetMaskTexture(const TexturePtr& tex) {
        bool need_flush = false;
        if (m_mask != tex) {
            need_flush = true;
        }
        bool needCalc2 = tex && tex!=m_fake_tex_black;
        if (needCalc2!=m_calc2_tex) {
            need_flush = true;
        }
        if (need_flush) {
            Flush();
        }
        m_mask = tex;
        m_calc2_tex = needCalc2;
        if (!tex) {
            m_render->SetTexture(0,1);
        }
    }
    void Graphics::SetMaskMode(MaskMode mode) {
        if (mode != m_mask_mode) {
            Flush();
        }
        if (!m_mask && mode!=MASK_MODE_NONE) {
            SetMaskTexture(m_fake_tex_black);
        }
        switch (mode) {
            case MASK_MODE_NONE:
                m_render->SetupTextureStageColorOp(GHL::TEX_OP_DISABLE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
                m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_DISABLE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
                break;
                
            case MASK_MODE_ALPHA:
                m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
                m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
                break;
                
            case MASK_MODE_SCREEN:
                m_render->SetupTextureStageColorOp(GHL::TEX_OP_INT_CURRENT_ALPHA,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
                m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_SELECT_2,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
                break;
        }
    
        m_mask_mode = mode;
        
    }
  	
	void Graphics::SetProjectionMatrix(const Matrix4f& m) {
		Flush();
		m_projection_matrix = m;
		m_render->SetProjectionMatrix(m.matrix);
	}
	
	void Graphics::SetViewMatrix(const Matrix4f& m) {
		Flush();
		m_view_matrix = m;
		m_render->SetViewMatrix(m.matrix);
	}
	
	void Graphics::SetViewport(const Recti& rect) {
		Flush();
		m_viewport = rect;
	}
	
	void Graphics::SetClipRect(const Recti& rect) {
		Flush();
		m_clip_rect = rect;
		if (m_clip_rect==m_viewport) {
			m_render->SetupScisor( false );
		} else {
			m_render->SetupScisor( true, rect.x,rect.y, rect.w,rect.h);
		}
	}
		
		
	void Graphics::BeginScene(GHL::Render* render) {
        sb_assert( (m_render==0) && "scene already started" );
		m_render = render;
		m_blend_mode = BLEND_MODE_COPY;
		m_render->SetupBlend(false);
		m_texture = TexturePtr();
        m_mask = TexturePtr();
        m_mask_mode = MASK_MODE_NONE;
		m_render->SetTexture(0,0);
		m_ptype = GHL::PRIMITIVE_TYPE_TRIANGLES;
		m_vertexes.clear();
		m_indexes.clear();
		m_primitives = 0;
		m_batches = 0;
		m_color = Color(1,1,1,1);
		m_transform = Transform2d();
		m_shader = ShaderPtr();
        m_calc2_tex = false;
		SetProjectionMatrix(Matrix4f::ortho(0,float(render->GetWidth()),
											float(render->GetHeight()),0,-10,10));
		SetViewMatrix(Matrix4f::identity());
		SetViewport(Recti(0,0,render->GetWidth(),render->GetHeight()));
		SetClipRect(GetViewport());
        m_render->SetTexture(0,0);
        m_render->SetTexture(0,1);
        m_render->SetIndexBuffer(0);
        m_render->SetVertexBuffer(0);
        m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
        m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
    }
	size_t Graphics::EndScene() {
		sb_assert( (m_render!=0) && "scene not started" );
		Flush();
		m_render->SetTexture(0,0);
        m_render->SetTexture(0,1);
		m_render->SetShader(0);
		m_texture = TexturePtr();
        m_mask = TexturePtr();
		m_shader = ShaderPtr();
		m_render = 0;
        m_mask_mode = MASK_MODE_NONE;
        return m_batches;
	}
	
	
	GHL::Render* Graphics::BeginNative() {
        sb_assert( (m_render!=0) && "scene not started" );
        Flush();
		GHL::Render* render = m_render;
		if (render) {
			EndScene();
		}
		return render;
	}
	
	void Graphics::EndNative(GHL::Render* render) {
        size_t prev_batches = m_batches;
		if (render) {
			BeginScene(render);
            m_batches+=prev_batches;
		}
	}
    
    const GHL::Texture* Graphics::Present( const TexturePtr& tex ) {
        if (!tex) return 0;
        return tex->Present(m_resources);
    }
	
}
