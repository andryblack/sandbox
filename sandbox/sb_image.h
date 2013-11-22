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

namespace Sandbox {

	/// @brief Image object
    /// Rectangular part of texture
    class Image {
    private:
        TexturePtr m_texture;
        float   m_src_x;
        float   m_src_y;
        float   m_src_w;
        float   m_src_h;
        Vector2f    m_hotspot;
        float   m_width;
        float   m_height;
    public:
        Image();
        Image(const TexturePtr& texture,float src_x,float src_y,float w,float h);
        Image(const TexturePtr& texture,float src_x,float src_y,float src_w,float src_h,float w,float h);
		
        const TexturePtr& GetTexture() const { return m_texture;}
		
        float GetTextureX() const { return m_src_x;}
        float GetTextureY() const { return m_src_y;}
        float GetTextureW() const { return m_src_w;}
        float GetTextureH() const { return m_src_h;}
		
        /// get hotspot ( relative to original size )
        const Vector2f& GetHotspot() const { return m_hotspot;}
		
        void SetHotspot( const Vector2f& hs );
		
        float GetWidth() const { return m_width;}
        float GetHeight() const { return m_height;}
		
        /// set real size ( recalc hotspot )
        void SetSize(float w,float h);
    };
	
    typedef sb::shared_ptr<Image> ImagePtr;
	
	
	inline Image::Image()
	: m_texture(),m_src_x(0),m_src_y(0),m_src_w(1.0f),m_src_h(1.0f),
	m_hotspot(),m_width(1.0f),m_height(1.0f)
    {
		
    }
	
    inline Image::Image(const TexturePtr& texture, float src_x, float src_y, float src_w, float src_h)
	: m_texture(texture),m_src_x(src_x),m_src_y(src_y),m_src_w(src_w),m_src_h(src_h) {
        m_width = src_w;
        m_height = src_h;
    }
	
    inline Image::Image(const TexturePtr& texture, float src_x, float src_y, float src_w, float src_h,float w,float h)
	: m_texture(texture),m_src_x(src_x),m_src_y(src_y),m_src_w(src_w),m_src_h(src_h) {
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
	
}

#endif /*SB_IMAGE_H*/
