#include "sb_container_transform.h"
#include "sb_graphics.h"
#include <sbstd/sb_assert.h>

SB_META_DECLARE_OBJECT(Sandbox::ContainerTransformBase, Sandbox::Container)
SB_META_DECLARE_OBJECT(Sandbox::ContainerTransform, Sandbox::ContainerTransformBase)
SB_META_DECLARE_OBJECT(Sandbox::ContainerTransformCopy, Sandbox::ContainerTransformBase)

namespace Sandbox {
    
    
    void ContainerTransformBase::DrawChilds(Graphics& g) const {
        Transform2d tr = g.GetTransform();
        g.SetTransform(tr * GetTransformM());
        Container::DrawChilds(g);
        g.SetTransform(tr);
    }
 
    void ContainerTransformBase::GetTransformImpl(Transform2d& tr) const {
        Container::GetTransformImpl(tr);
        tr = tr * GetTransformM();
    }
    
    void ContainerTransformBase::GlobalToLocalImpl(Vector2f& v) const {
        SceneObject::GlobalToLocalImpl(v);
        Transform2d tr = GetTransformM().inverted();
        v = tr.transform(v);
        if (Container::GetTransformM()) {
            Container::GetTransformM()->UnTransform(v);
        }
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
