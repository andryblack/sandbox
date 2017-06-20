/*
 *  sb_container_transform.h
 *
 *  Copyright 2017 Andrey Kunitsyn. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_TRANSFORM_H
#define SB_CONTAINER_TRANSFORM_H

#include "sb_container.h"

namespace Sandbox {
    
    class ContainerTransformBase : public Container {
        SB_META_OBJECT
    public:
        virtual void DrawChilds(Graphics& g) const;
        virtual void GetTransformImpl(Transform2d& tr) const;
        virtual void GlobalToLocalImpl(Vector2f& v) const;
        virtual const Transform2d& GetTransformM() const = 0;
    };
    typedef sb::intrusive_ptr<ContainerTransformBase> ContainerTransformBasePtr;
    
    class ContainerTransform : public ContainerTransformBase {
        SB_META_OBJECT
    public:
        ContainerTransform();
        const Transform2d& GetTransform() const { return m_tr; }
        void SetTransform(const Transform2d& tr) { m_tr = tr; }
        Transform2d& GetTransformM() { return m_tr; }
        virtual const Transform2d& GetTransformM() const { return m_tr; }
    private:
        Transform2d m_tr;
    };
    typedef sb::intrusive_ptr<ContainerTransform> ContainerTransformPtr;
 
    class ContainerTransformCopy : public ContainerTransformBase {
        SB_META_OBJECT
    public:
        ContainerTransformCopy(const ContainerTransformBasePtr& tr);
        virtual const Transform2d& GetTransformM() const;
    private:
        ContainerTransformBasePtr m_copy;
    };

}

#endif /*SB_CONTAINER_TRANSFORM_H*/
