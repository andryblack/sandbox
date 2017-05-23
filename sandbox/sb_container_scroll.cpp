#include "sb_container_scroll.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::ContainerScroll, Sandbox::Container)

namespace Sandbox {
    
    
    ContainerScroll::ContainerScroll() {
        
    }
    
    void ContainerScroll::Update(float dt) {
        Scroll::Update(dt);
        Container::Update(dt);
    }

    void ContainerScroll::OnScrollMove() {
    }
    
    void ContainerScroll::Draw(Graphics &g) const {
        Transform2d tr = g.GetTransform();
        g.SetTransform(tr.translated(-m_offset));
        Container::Draw(g);
        g.SetTransform(tr);
    }
    
    void ContainerScroll::GlobalToLocalImpl(Vector2f& v) const {
        SceneObject::GlobalToLocalImpl(v);
        v += m_offset;
        if (GetTransformM()) {
            GetTransformM()->UnTransform(v);
        }
    }
    void ContainerScroll::GetTransformImpl(Transform2d& tr) const {
        SceneObject::GetTransformImpl(tr);
        tr.translate(-m_offset);
        if (GetTransformM()) {
            GetTransformM()->Apply(tr);
        }
    }
}


