/*
 *  sb_graphics.h
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_GRAPHICS_H
#define SB_GRAPHICS_H

#include "sb_transform2d.h"
#include "sb_color.h"
#include "sb_image.h"
#include "sb_shader.h"
#include "sb_matrix4.h"
#include "sb_rect.h"
#include "sb_particle.h"

#include <ghl_render.h>
#include <vector>

namespace Sandbox {
	
	enum BlendMode {
        BLEND_MODE_COPY,
        BLEND_MODE_ALPHABLEND,
        BLEND_MODE_ADDITIVE,
        BLEND_MODE_ADDITIVE_ALPHA,
    };
    
    enum MaskMode {
        MASK_MODE_NONE,
        MASK_MODE_ALPHA,
        MASK_MODE_SCREEN,
    };
	
    struct GeometryData;
    
	class Graphics {
	public:

		explicit Graphics( Resources* resources );
		~Graphics();
		
		void Load(GHL::Render* render);
		
		/// @brief begin draw scene
		void BeginScene(GHL::Render* render, bool target);
        
        /// clear scene
        void Clear( const Color& clr , float depth );
   
		/// global transform
		const Transform2d& GetTransform() const { return m_transform;}
		void SetTransform(const Transform2d& tr) { m_transform = tr;}

		/// projection matrix
		const Matrix4f& GetProjectionMatrix() const { return m_projection_matrix;}
		void SetProjectionMatrix(const Matrix4f& m);
		/// view matrix
		const Matrix4f& GetViewMatrix() const { return m_view_matrix;}
		void SetViewMatrix(const Matrix4f& m);
		/// viewport
		const Recti& GetViewport() const { return m_viewport;}
		void SetViewport(const Recti& rect);
		/// clip 
		const Recti& GetClipRect() const { return m_clip_rect;}
		void SetClipRect(const Recti& rect);
		/// global color
		const Color& GetColor() const { return m_color;}
		void SetColor(const Color& color) { m_color = color;}

		/// blending
		BlendMode GetBlendMode() const { return m_blend_mode; }
		void SetBlendMode(BlendMode bm) ;

		/// shader
		ShaderPtr GetShader() const { return m_shader; }
		void SetShader(const ShaderPtr& sh);
        
        /// depth
        bool GetDepthWrite() const { return m_depth_write; }
        bool GetDepthTest() const { return m_depth_test; }
        void SetDepthWrite(bool write);
        void SetDepthTest(bool test);

        /// mask
        void SetMask(MaskMode mode, const TexturePtr& mask_tex,const Transform2d& tr);
        const TexturePtr& GetMaskTexture() const { return m_mask; }
        void SetMaskTexture(const TexturePtr& tex);
        MaskMode GetMaskMode() const { return m_mask_mode; }
        void SetMaskMode(MaskMode mode);
        const Transform2d& GetMaskTransform() const { return m_mask_transform; }
        void SetMaskTransform( const Transform2d& tr ) { m_mask_transform = tr; }
        
		/// draw image
		/// @{

		void DrawImage(const Image& img,float x,float y);
		void DrawImage(const Image& img,float x,float y,const Color& clr);
		void DrawImage(const Image& img,float x,float y,const Color& clr,float scale);
        void DrawImage(const Image& img,const Vector2f& pos) {
            DrawImage(img,pos.x,pos.y);
        }
		void DrawImage(const Image& img,const Vector2f& pos,const Color& clr) {
            DrawImage(img,pos.x,pos.y,clr);
        }
		void DrawImage(const Image& img,const Vector2f& pos,const Color& clr,float scale) {
            DrawImage(img,pos.x,pos.y,clr,scale);
        }
        void DrawImageBox(const ImageBox& img, const Vector2f& pos, const Vector2f& size);
        /// @}
        
        /// fill rect by pattern
        void FillRect( const TexturePtr& texture, const Rectf& rect);
		
		/// draw lines
		/// @{
		void DrawLine(const Vector2f& from, const Vector2f& to);
		void DrawLine(const Vector2f& from, const Vector2f& to,const Color& clr);
		/// @}
		
		/// draw line strips
		/// @{
		void DrawLineStrip(const std::vector<Vector2f>& points);
		void DrawLineStrip(const std::vector<Vector2f>& points,const Color& clr);
		/// @}
		
		/// draw circles
		/// @{
		void DrawCircle(const Vector2f& pos, float r);
		void DrawCircle(const Vector2f& pos, float r,const Color& clr);
		/// @}

        /// draw particles
        void DrawParticles( const std::vector<Particle>& particles,
                           const std::vector<const Image*>& images );
	
		void DrawGeometry(const GeometryData& buffer,bool transform);
        
		/// @brief flush batches
		void Flush();
		/// @brief end draw scene
		size_t EndScene();

		/// get "native" render
		GHL::Render* BeginNative();
		void EndNative(GHL::Render* render);
        const GHL::Texture* Present( const TexturePtr& tex );
		
        GHL::UInt32 GetScreenWidth() const { if (m_render) return m_render->GetWidth(); return 0;}
		GHL::UInt32 GetScreenHeight() const { if (m_render) return m_render->GetHeight(); return 0;}
        
        void SetScale(float scale);
        float GetScele() const { return m_scale; }
   private:
        Resources*  m_resources;
		GHL::Render* m_render;
        bool m_render_to_target;
		GHL::Texture* m_fake_tex_white;
        TexturePtr  m_fake_tex_black;
        
		Transform2d     m_transform;
        Transform2d     m_mask_transform;
        float           m_scale;
        
		Matrix4f	m_projection_matrix;
		Matrix4f	m_view_matrix;
		Recti		m_viewport;
		Recti		m_clip_rect;
		Color		m_color;
		BlendMode	m_blend_mode;
        MaskMode    m_mask_mode;
		TexturePtr  m_texture;
        TexturePtr  m_mask;
        float       m_itw;
        float       m_ith;
		ShaderPtr	m_shader;
        bool        m_depth_write;
        bool        m_depth_test;
		GHL::PrimitiveType	m_ptype;
        GHL::UInt32      m_primitives;
		std::vector<GHL::Vertex> m_vertexes;
        std::vector<GHL::Vertex2Tex> m_vertexes2tex;
		std::vector<GHL::UInt16> m_indexes;
        bool    m_calc2_tex;
	
		void BeginDrawImage(const Image& img);
		void BeginDrawLines();
		void BeginDrawCircle();
		void DrawCircle(const Vector2f& pos, float r,GHL::UInt32 clr);
		inline void appendVertex(float x,float y,float tx,float ty,GHL::UInt32 color=0xffffffff) {
			m_vertexes.push_back(GHL::Vertex());
			GHL::Vertex& v(m_vertexes.back());
			m_transform.transform(x,y,v.x,v.y);
			v.z = 0.0f;
			v.color[0]=color & 0xff;
			v.color[1]=(color >> 8)&0xff;
			v.color[2]=(color >> 16)&0xff;
			v.color[3]=(color >> 24)&0xff;
			v.tx = tx;
			v.ty = ty;
		}
        inline void appendVertex2(float x,float y,float tx,float ty,GHL::UInt32 color=0xffffffff) {
			m_vertexes2tex.push_back(GHL::Vertex2Tex());
			GHL::Vertex2Tex& v(m_vertexes2tex.back());
			m_transform.transform(x,y,v.x,v.y);
			v.z = 0.0f;
			v.color[0]=color & 0xff;
			v.color[1]=(color >> 8)&0xff;
			v.color[2]=(color >> 16)&0xff;
			v.color[3]=(color >> 24)&0xff;
			v.tx = tx;
			v.ty = ty;
            m_mask_transform.transform(x, y, v.t2x, v.t2y);
		}
		void appendTriangle(GHL::Int16 i1,GHL::Int16 i2,GHL::Int16 i3);
		void appendQuad();
		size_t m_batches;
	};
}

#endif 
