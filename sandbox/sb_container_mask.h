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
#include "sb_rect.h"

namespace Sandbox {
    
    class ContainerMask : public Container {
        SB_META_OBJECT
    public:
        ContainerMask();
        void Draw(Graphics& g) const SB_OVERRIDE;
        void SetMode(MaskMode mode) { m_mode = mode;}
        MaskMode GetMode() const { return m_mode;}
        const ImagePtr& GetImage() const { return m_image; }
        void SetImage(const ImagePtr& img);
        const Rectf& GetRect() const { return m_rect; }
        void SetRect(const Rectf& r);
        void CalcRect();
    private:
        MaskMode	m_mode;
        ImagePtr    m_image;
        Rectf       m_rect;
    };

}
#endif /*SB_CONTAINER_MASK_H*/
