/*
 *  sb_container_blend.h
 *  SR
 *
 *  Created by Андрей Куницын on 11.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_BLEND_H
#define SB_CONTAINER_BLEND_H

#include "sb_container.h"
#include "sb_graphics.h"

namespace Sandbox {
	
	class ContainerBlend : public Container {
        SB_META_OBJECT
    public:
        ContainerBlend();
		void Draw(Graphics& g) const;
		void SetMode(BlendMode mode) { m_mode = mode;}
		BlendMode GetMode() const { return m_mode;}
	private:
		BlendMode	m_mode;
	};
    
    class ContainerMask : public Container {
        SB_META_OBJECT
    public:
        ContainerMask();
		void Draw(Graphics& g) const;
		void SetMode(MaskMode mode) { m_mode = mode;}
		MaskMode GetMode() const { return m_mode;}
        const Transform2d& GetTransform() const { return m_transform; }
        void SetTransform( const Transform2d& tr ) { m_transform = tr; }
        const TexturePtr& GetTexture() const { return m_texture; }
        void SetTexture(const TexturePtr& t) { m_texture = t; }
	private:
		MaskMode	m_mode;
        TexturePtr  m_texture;
        Transform2d m_transform;
	};
}
#endif /*SB_CONTAINER_BLEND_H*/
