/*
 *  sb_image.h
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_IMAGE_H
#define SB_IMAGE_H

#include "sb_texture.h"
#include "sb_vector2.h"
#include "sb_size.h"

namespace Sandbox {

	/// @brief Image object
    /// Rectangular part of texture
    class Image : public sb::ref_countered_base {
    private:
        TexturePtr m_texture;
        float   m_src_x;
        float   m_src_y;
        float   m_src_w;
        float   m_src_h;
        Vector2f    m_hotspot;
        float   m_width;
        float   m_height;
        bool    m_rotated;
    public:
        Image();
        Image(const TexturePtr& texture,float src_x,float src_y,float w,float h);
        Image(const TexturePtr& texture,float src_x,float src_y,float src_w,float src_h,float w,float h);
		
        const TexturePtr& GetTexture() const { return m_texture;}
        void SetTexture(const TexturePtr& tex) { m_texture = tex; }
        
        void SetTextureRect(float x,float y,float w,float h) {
            m_src_x = x; m_src_y = y; m_src_w = w; m_src_h = h;
        }
        
        void SetRotated(bool r) { m_rotated = r; }
        bool GetRotated() const { return m_rotated; }
		
        float GetTextureX() const { return m_src_x;}
        float GetTextureY() const { return m_src_y;}
        float GetTextureW() const { return m_src_w;}
        float GetTextureH() const { return m_src_h;}
        float GetTextureDrawW() const { return m_rotated ? m_src_h : m_src_w;}
        float GetTextureDrawH() const { return m_rotated ? m_src_w : m_src_h;}
        
		
        /// get hotspot ( relative to original size )
        const Vector2f& GetHotspot() const { return m_hotspot;}
        /// get hotspot in sprite-size
        Vector2f GetHotspotReal() const {
            return Vector2f(m_hotspot.x*m_width/m_src_w, m_hotspot.y*m_height/m_src_h);
        }
        void SetHotspot( const Vector2f& hs );
		
        float GetWidth() const { return m_width;}
        float GetHeight() const { return m_height;}
        Sizef GetSize() const { return Sizef(GetWidth(),GetHeight()); }
		
        /// set real size ( recalc hotspot )
        void SetSize(float w,float h);
        
        bool CheckBit(int x,int y,Resources* resources);
        
        sb::intrusive_ptr<Image> Clone() const;
    };
	
    typedef sb::intrusive_ptr<Image> ImagePtr;
	
	
	inline Image::Image()
	: m_texture(),m_src_x(0),m_src_y(0),m_src_w(1.0f),m_src_h(1.0f),
	m_hotspot(),m_width(1.0f),m_height(1.0f),m_rotated(false)
    {
		
    }
	
    inline Image::Image(const TexturePtr& texture, float src_x, float src_y, float src_w, float src_h)
	: m_texture(texture),m_src_x(src_x),m_src_y(src_y),m_src_w(src_w),m_src_h(src_h),m_rotated(false) {
        m_width = src_w;
        m_height = src_h;
    }
	
    inline Image::Image(const TexturePtr& texture, float src_x, float src_y, float src_w, float src_h,float w,float h)
	: m_texture(texture),m_src_x(src_x),m_src_y(src_y),m_src_w(src_w),m_src_h(src_h),m_rotated(false) {
        m_width = w;
        m_height = h;
    }
	
	inline void Image::SetHotspot(const Vector2f& hs) {
		m_hotspot = hs;
    }
	
    inline void Image::SetSize(float w, float h) {
        m_width = w;
        m_height = h;
    }
    
    class ImageBox : public Image {
    public:
        ImageBox() : m_offset_l(0.0f),m_offset_r(0.0f),m_offset_t(0.0f),m_offset_b(0.0f),m_tile_v(false),m_tile_h(false){}
        float GetOffsetL() const { return m_offset_l; }
        float GetOffsetR() const { return m_offset_r; }
        float GetOffsetT() const { return m_offset_t; }
        float GetOffsetB() const { return m_offset_b; }
        
        bool GetTileV() const { return m_tile_v; }
        bool GetTileH() const { return m_tile_h; }
        
        void SetOffsets(float l,float r, float t, float b) {
            m_offset_l = l; m_offset_r = r;
            m_offset_t = t; m_offset_b = b;
        }
        void SetTile(bool h,bool v) {
            m_tile_v = v;
            m_tile_h = h;
        }
    private:
        float   m_offset_l;
        float   m_offset_r;
        float   m_offset_t;
        float   m_offset_b;
        bool    m_tile_v;
        bool    m_tile_h;
    };
    typedef sb::intrusive_ptr<ImageBox> ImageBoxPtr;
	
}

#endif /*SB_IMAGE_H*/
