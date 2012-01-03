/*
 *  sb_graphics.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 07.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_graphics.h"
#include "sb_assert.h"

namespace Sandbox {
	
    static const char* MODULE = "Sanbox:Graphics";
	
	Graphics::Graphics() {
        (void)MODULE;
		m_render = 0;
		m_fake_tex_white = 0;
		m_vertexes.reserve(512);
		m_indexes.reserve(512);
	}
	
	Graphics::~Graphics() {
		if (m_fake_tex_white)
			m_fake_tex_white->Release();
		m_fake_tex_white = 0;
	}
	
	void Graphics::Load(GHL::Render* render) {
		if (!m_fake_tex_white) {
			m_fake_tex_white = render->CreateTexture(1,1,GHL::TEXTURE_FORMAT_RGBA,false);
			GHL::Byte tex[4] = {0xff,0xff,0xff,0xff};
			m_fake_tex_white->SetData(0,0,1,1,tex,0);
		}
	}
	
	void Graphics::SetBlendMode(BlendMode bmode) {
		if (m_blend_mode!=bmode) {
			Flush();
			switch (bmode) {
				case BLEND_MODE_COPY: 
					m_render->SetTexture(0,1);
					m_render->SetupAlphaTest(false, GHL::COMPARE_FUNC_GREATER, 0.0f);
					m_render->SetupBlend(false, GHL::BLEND_FACTOR_SRC_COLOR, GHL::BLEND_FACTOR_ZERO); 
					m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
					m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
					m_render->SetupTextureStageColorOp(GHL::TEX_OP_DISABLE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
					m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_DISABLE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
					break;
				case BLEND_MODE_ALPHABLEND: 
					m_render->SetTexture(0,1);
					m_render->SetupAlphaTest(true, GHL::COMPARE_FUNC_GREATER, 0.0f);
					m_render->SetupBlend(true, GHL::BLEND_FACTOR_SRC_ALPHA, GHL::BLEND_FACTOR_SRC_ALPHA_INV); 
					m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
					m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
					m_render->SetupTextureStageColorOp(GHL::TEX_OP_DISABLE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
					m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_DISABLE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
					break;
				case BLEND_MODE_ADDITIVE: 
					m_render->SetTexture(0,1);
					m_render->SetupAlphaTest(false, GHL::COMPARE_FUNC_GREATER, 0.0f);
					m_render->SetupBlend(true, GHL::BLEND_FACTOR_ONE, GHL::BLEND_FACTOR_ONE); 
					m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
					m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
					m_render->SetupTextureStageColorOp(GHL::TEX_OP_DISABLE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
					m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_DISABLE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
					break;
				case BLEND_MODE_ADDITIVE_ALPHA: 
					m_render->SetTexture(0,1);
					m_render->SetupAlphaTest(true, GHL::COMPARE_FUNC_GREATER, 0.0f);
					m_render->SetupBlend(true, GHL::BLEND_FACTOR_SRC_ALPHA, GHL::BLEND_FACTOR_ONE); 
					m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
					m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,0);
					m_render->SetupTextureStageColorOp(GHL::TEX_OP_DISABLE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
					m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_DISABLE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
					break;
				case BLEND_MODE_SCREEN:
					m_render->SetTexture(m_fake_tex_white,1);
					m_render->SetupAlphaTest(true, GHL::COMPARE_FUNC_GREATER, 0.0f);
					m_render->SetupBlend(true, GHL::BLEND_FACTOR_DST_COLOR_INV, GHL::BLEND_FACTOR_ONE);
					m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_DIFFUSE,0);
					m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_MODULATE,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_DIFFUSE,0);
					m_render->SetupTextureStageColorOp(GHL::TEX_OP_INT_CURRENT_ALPHA,GHL::TEX_ARG_CURRENT,GHL::TEX_ARG_TEXTURE_INV,1);
					m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_SELECT_2,GHL::TEX_ARG_TEXTURE,GHL::TEX_ARG_CURRENT,1);
					break;
				default:
					sb_assert(false && "unknown blend mode");
					break;
			}
			m_blend_mode = bmode;
		}
		
	}
	
	void Graphics::Flush() {
		sb_assert( (m_render!=0) && "scene not started" );
        if (m_primitives==0) return;
        /// do batch
		m_render->SetTexture(m_texture ? m_texture->Present() : 0,0);
		//m_render->SetTexture(m_texture ? m_texture->Present() : 0,1);
		m_render->DrawPrimitivesFromMemory(m_ptype,
										   GHL::VERTEX_TYPE_SIMPLE,
											&m_vertexes.front(),
                                           static_cast<GHL::UInt32>(m_vertexes.size()),
                                               &m_indexes.front(),
                                               m_primitives);
        m_batches++;
#ifdef PRINT_BATCHES
        LogDebug(MODULE) << "batch " << m_data->primitives;
#endif
        m_vertexes.clear();
		m_indexes.clear();
		m_primitives = 0;
	}
	
	void Graphics::BeginDrawImage(const ImagePtr& img) {
        TexturePtr texture = img->GetTexture();
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
    }
	
	void Graphics::appendTriangle(int i1,int i2,int i3) {
		GHL::UInt16 base = m_vertexes.size();
		m_indexes.push_back(base+i1);
		m_indexes.push_back(base+i2);
		m_indexes.push_back(base+i3);
		m_primitives++;
	}
	void Graphics::appendQuad() {
		GHL::UInt16 base = m_vertexes.size();
		m_indexes.push_back(base+0);
		m_indexes.push_back(base+1);
		m_indexes.push_back(base+2);
		m_indexes.push_back(base+2);
		m_indexes.push_back(base+1);
		m_indexes.push_back(base+3);
		m_primitives+=2;
	}
	
	void Graphics::DrawImage(const ImagePtr& _img,float x,float y) {
		sb_assert( (m_render!=0) && "scene not started" );
        if (!_img) return;
        BeginDrawImage(_img);
		/// small speedup
		const Image* img = _img.get();
        x-=img->GetHotspot().x;
        y-=img->GetHotspot().y;
		const float w = img->GetWidth();
		const float h = img->GetHeight();
		
        appendQuad();
		
		GHL::UInt32 clr = (m_color).hw();
		
        {
            appendVertex(x,y,
                                 img->GetTextureX(),img->GetTextureY(),clr);
            appendVertex(x+w,y,
                                 img->GetTextureX()+img->GetTextureW(),img->GetTextureY(),clr);
            appendVertex(x,y+h,
                                 img->GetTextureX(),img->GetTextureY()+img->GetTextureH(),clr);
            appendVertex(x+w,y+h,
                                 img->GetTextureX()+img->GetTextureW(),img->GetTextureY()+img->GetTextureH(),clr);
        } 
	}
	void Graphics::DrawImage(const ImagePtr& _img,float x,float y,const Color& _clr) {
		sb_assert( (m_render!=0) && "scene not started" );
        if (!_img) return;
        BeginDrawImage(_img);
		/// small speedup
		const Image* img = _img.get();
        x-=img->GetHotspot().x;
        y-=img->GetHotspot().y;
		const float w = img->GetWidth();
		const float h = img->GetHeight();
		
        appendQuad();
		
		GHL::UInt32 clr = (m_color*_clr).hw();
		
        {
            appendVertex(x,y,
						 img->GetTextureX(),img->GetTextureY(),clr);
            appendVertex(x+w,y,
						 img->GetTextureX()+img->GetTextureW(),img->GetTextureY(),clr);
            appendVertex(x,y+h,
						 img->GetTextureX(),img->GetTextureY()+img->GetTextureH(),clr);
            appendVertex(x+w,y+h,
						 img->GetTextureX()+img->GetTextureW(),img->GetTextureY()+img->GetTextureH(),clr);
        } 
		
	}
	void Graphics::DrawImage(const ImagePtr& _img,float x,float y,const Color& _clr,float scale) {
		sb_assert( (m_render!=0) && "scene not started" );
        if (!_img) return;
        BeginDrawImage(_img);
		/// small speedup
		const Image* img = _img.get();
        x-=img->GetHotspot().x*scale;
        y-=img->GetHotspot().y*scale;
		const float w = img->GetWidth()*scale;
		const float h = img->GetHeight()*scale;
		
        appendQuad();
		
		GHL::UInt32 clr = (m_color*_clr).hw();
		
        {
            appendVertex(x,y,
						 img->GetTextureX(),img->GetTextureY(),clr);
            appendVertex(x+w,y,
						 img->GetTextureX()+img->GetTextureW(),img->GetTextureY(),clr);
            appendVertex(x,y+h,
						 img->GetTextureX(),img->GetTextureY()+img->GetTextureH(),clr);
            appendVertex(x+w,y+h,
						 img->GetTextureX()+img->GetTextureW(),img->GetTextureY()+img->GetTextureH(),clr);
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
		BeginDrawLines();
		GHL::UInt32 clr = m_color.hw();
		GHL::UInt16 base = m_vertexes.size();
		m_indexes.push_back(base+0);
		m_indexes.push_back(base+1);
		m_primitives++;
		appendVertex(from.x, from.y, 0, 0, clr);
		appendVertex(to.x,to.y,0,0,clr);
	}
	void Graphics::DrawLine(const Vector2f& from, const Vector2f& to,const Color& clr_) {
		sb_assert( (m_render!=0) && "scene not started" );
		BeginDrawLines();
		GHL::UInt32 clr = (m_color*clr_).hw();
		GHL::UInt16 base = m_vertexes.size();
		m_indexes.push_back(base+0);
		m_indexes.push_back(base+1);
		m_primitives++;
		appendVertex(from.x, from.y, 0, 0, clr);
		appendVertex(to.x,to.y,0,0,clr);
	}
	
	void Graphics::DrawLineStrip(const std::vector<Vector2f>& points) {
		sb_assert( (m_render!=0) && "scene not started" );
        if (points.size()<2) return;
		Flush();
		m_texture = TexturePtr();
        m_ptype = GHL::PRIMITIVE_TYPE_LINE_STRIP;
		GHL::UInt32 clr = m_color.hw();
		GHL::UInt16 base = m_vertexes.size();
		for (size_t i=0;i<points.size();i++) {
			m_indexes.push_back(base+i);
		}
		for (size_t i=0;i<points.size();i++) {
			appendVertex(points[i].x, points[i].y, 0, 0, clr);
		}
		m_primitives = points.size()-1;
		Flush();
	}
	void Graphics::DrawLineStrip(const std::vector<Vector2f>& points,const Color& clr_) {
		sb_assert( (m_render!=0) && "scene not started" );
        if (points.size()<2) return;
		Flush();
		m_texture = TexturePtr();
        m_ptype = GHL::PRIMITIVE_TYPE_LINE_STRIP;
		GHL::UInt32 clr = (m_color*clr_).hw();
		GHL::UInt16 base = m_vertexes.size();
		for (size_t i=0;i<points.size();i++) {
			m_indexes.push_back(base+i);
		}
		for (size_t i=0;i<points.size();i++) {
			appendVertex(points[i].x, points[i].y, 0, 0, clr);
		}
		m_primitives = points.size()-1;
		Flush();
	}
	
	void Graphics::BeginDrawCircle() {
		Flush();
		m_texture = TexturePtr();
        m_ptype = GHL::PRIMITIVE_TYPE_LINE_STRIP;
	}
	void Graphics::DrawCircle(const Vector2f& pos, float r) {
		sb_assert( (m_render!=0) && "scene not started" );
		BeginDrawCircle();
		GHL::UInt32 clr = m_color.hw();
		DrawCircle(pos,r,clr);
		Flush();
	}
	void Graphics::DrawCircle(const Vector2f& pos, float r,const Color& clr_) {
		sb_assert( (m_render!=0) && "scene not started" );
		BeginDrawCircle();
		GHL::UInt32 clr = (m_color*clr_).hw(); 
		DrawCircle(pos,r,clr);
		Flush();
	}
	void Graphics::DrawCircle(const Vector2f& pos, float r,GHL::UInt32 clr) {
		size_t subdivs = int(2*M_PI*r)/5;
		if (subdivs<8) subdivs = 8;
		const float step = M_PI*2/subdivs;
		GHL::UInt16 base = m_vertexes.size();
		for (size_t i=0;i<subdivs;i++) {
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
	}
	
	
	void Graphics::DrawBuffer(const TexturePtr& texture,GHL::PrimitiveType prim,
					const std::vector<GHL::Vertex>& vertices,
					const std::vector<GHL::UInt16>& indices) { 
		Flush();
        //if (!texture) return;
		{
            for (size_t i=0;i<vertices.size();i++) {
                appendVertex(vertices[i].x,
							 vertices[i].y,
							 vertices[i].tx,
							 vertices[i].ty,
							 (Color(*reinterpret_cast<const GHL::UInt32*>(vertices[i].color))*m_color).hw());
            }
        } 
		if (texture)
			m_render->SetTexture(texture->Present(),0);
		else {
			m_render->SetTexture(0,0);
		}
		m_texture = texture;

        
        m_texture = texture;
        m_ptype = prim;
        m_indexes = indices;
		
        if (prim==GHL::PRIMITIVE_TYPE_TRIANGLES)
            m_primitives = static_cast<GHL::UInt32>(m_indexes.size())/3;
		else if (prim==GHL::PRIMITIVE_TYPE_TRIANGLE_STRIP){
			sb_assert(m_indexes.size()>2);
			m_primitives = static_cast<GHL::UInt32>(m_indexes.size()-2);
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
		m_render->SetViewport(rect.x, rect.y, rect.w, rect.h);
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
		m_render = render;
		m_render->SetupAlphaTest(false, GHL::COMPARE_FUNC_GREATER, 0.0f);
		m_blend_mode = BLEND_MODE_COPY;
		m_render->SetupBlend(false);
		m_texture = TexturePtr();
		m_render->SetTexture(0,0);
		m_ptype = GHL::PRIMITIVE_TYPE_TRIANGLES;
		m_vertexes.clear();
		m_indexes.clear();
		m_primitives = 0;
		m_batches = 0;
		m_color = Color(1,1,1,1);
		m_transform = Transform2d();
		m_shader = ShaderPtr();
		SetProjectionMatrix(Matrix4f::ortho(0,float(render->GetWidth()),
											float(render->GetHeight()),0,-10,10));
		SetViewMatrix(Matrix4f::identity());
		SetViewport(Recti(0,0,render->GetWidth(),render->GetHeight()));
		SetClipRect(GetViewport());
	}
	void Graphics::EndScene() {
		sb_assert( (m_render!=0) && "scene not started" );
		Flush();
		m_render->SetTexture(0,0);
		m_render->SetShader(0);
		m_texture = TexturePtr();
		m_shader = ShaderPtr();
		m_render = 0;
	}
	
	
	GHL::Render* Graphics::BeginNative() {
		GHL::Render* render = m_render;
		if (render) {
			EndScene();
		}
		return render;
	}
	
	void Graphics::EndNative(GHL::Render* render) {
		if (render) {
			BeginScene(render);
		}
	}
	
	
	
}
