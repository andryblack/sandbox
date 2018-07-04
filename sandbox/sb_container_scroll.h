/*
 *  sb_container_scroll.h
 *  Copyright 2016 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_SCROLL_H_INCLUDED
#define SB_CONTAINER_SCROLL_H_INCLUDED

#include "sb_container.h"
#include "sb_scroll.h"

namespace Sandbox {
    
    class ContainerScroll : public Container, public Scroll {
        SB_META_OBJECT
    public:
        explicit ContainerScroll();
        
        virtual void Update(float dt) SB_OVERRIDE;
        virtual void OnScrollMove() SB_OVERRIDE;
       
        virtual void Draw( Graphics& g ) const SB_OVERRIDE;
    private:
        Vector2f    m_offset;
        virtual Vector2f GetOffset() const SB_OVERRIDE {
            return m_offset;
        }
        virtual void SetOffset(const Vector2f& pos) SB_OVERRIDE {
            m_offset = pos;
        }
        virtual void GlobalToLocalImpl(Vector2f& v) const SB_OVERRIDE;
        virtual void GetTransformImpl(Transform2d& tr) const SB_OVERRIDE;
        virtual void GetTransformToImpl(const SceneObject* root,Transform2d& tr) const SB_OVERRIDE;
        
    };
    typedef sb::intrusive_ptr<ContainerScroll> ContainerScrollPtr;
}

#endif /*SB_CONTAINER_SCROLL_H_INCLUDED*/
