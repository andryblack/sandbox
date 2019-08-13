#include "sb_container_transform.h"
#include "sb_graphics.h"
#include <sbstd/sb_assert.h>

SB_META_DECLARE_OBJECT(Sandbox::ContainerTransformBase, Sandbox::ContainerBase)
SB_META_DECLARE_OBJECT(Sandbox::ContainerTransform, Sandbox::ContainerTransformBase)
SB_META_DECLARE_OBJECT(Sandbox::ContainerTransformCopy, Sandbox::ContainerTransformBase)

namespace Sandbox {
    
    
    void ContainerTransformBase::DrawChilds(Graphics& g) const {
        Transform2d tr = g.GetTransform();
        g.SetTransform(tr * GetTransformM());
        DrawTransformed(g);
        g.SetTransform(tr);
    }
    
    void ContainerTransformBase::DrawTransformed(Graphics& g) const {
        ContainerBase::DrawChilds(g);
    }
 
    void ContainerTransformBase::GetTransformImpl(Transform2d& tr) const {
        ContainerBase::GetTransformImpl(tr);
        tr = tr * GetTransformM();
    }
    
    void ContainerTransformBase::GetTransformToImpl(const SceneObject* root,Transform2d& tr) const {
        ContainerBase::GetTransformToImpl(root,tr);
        if (root != this) {
            tr = tr * GetTransformM();
        }
    }
    
    void ContainerTransformBase::GlobalToLocalImpl(Vector2f& v) const {
        SceneObject::GlobalToLocalImpl(v);
        Transform2d tr = GetTransformM().inverted();
        v = tr.transform(v);
    }
    
    ContainerTransform::ContainerTransform() {
        
    }
    
    ContainerTransformCopy::ContainerTransformCopy(const ContainerTransformBasePtr& tr) : m_copy(tr) {
        sb_assert(m_copy);
    }
    const Transform2d& ContainerTransformCopy::GetTransformM() const {
        return m_copy->GetTransformM();
    }

}
