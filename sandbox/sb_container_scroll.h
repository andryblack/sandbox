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
        
        virtual void Update(float dt);
        virtual void OnScrollMove();
       
        virtual void Draw( Graphics& g ) const;
    private:
        Vector2f    m_offset;
        virtual Vector2f GetOffset() const {
            return m_offset;
        }
        virtual void SetOffset(const Vector2f& pos) {
            m_offset = pos;
        }
        virtual void GlobalToLocalImpl(Vector2f& v) const;
        virtual void GetTransformImpl(Transform2d& tr) const;
        
    };
    typedef sb::intrusive_ptr<ContainerScroll> ContainerScrollPtr;
}

#endif /*SB_CONTAINER_SCROLL_H_INCLUDED*/
