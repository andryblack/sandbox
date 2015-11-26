/*
 *  sb_container_clip.h
 *  SR
 *
 *  Copyright 2015 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_CLIP_H
#define SB_CONTAINER_CLIP_H

#include "sb_container.h"
#include "sb_graphics.h"
#include "sb_rect.h"

namespace Sandbox {
    
    class ContainerClip : public Container {
        SB_META_OBJECT
    public:
        ContainerClip();
        void Draw(Graphics& g) const;
        void SetRect(const Recti& rect) { m_rect = rect;}
        const Recti& GetRect() const { return m_rect;}
    private:
        Recti	m_rect;
    };
}
#endif /*SB_CONTAINER_CLIP_H*/
