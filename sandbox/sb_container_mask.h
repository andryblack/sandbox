/*
 *  sb_container_mask.h
 *
 *  Copyright 2016 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_MASK_H
#define SB_CONTAINER_MASK_H

#include "sb_container.h"
#include "sb_graphics.h"
#include "sb_image.h"

namespace Sandbox {
    
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
        void SetImage(const ImagePtr& img);
    private:
        MaskMode	m_mode;
        TexturePtr  m_texture;
        Transform2d m_transform;
    };

}
#endif /*SB_CONTAINER_MASK_H*/
