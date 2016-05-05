#include "sb_skelet_object.h"
#include "sb_graphics.h"
#include "sb_skelet_data.h"

SB_META_DECLARE_OBJECT(Sandbox::SkeletonNode,Sandbox::SceneObject)
SB_META_DECLARE_OBJECT(Sandbox::SkeletObject,Sandbox::SceneObject)

namespace Sandbox {
    
    SkeletonNode::SkeletonNode() : m_color(1.0f,1.0f,1.0f,1.0f),m_blend(BLEND_MODE_ALPHABLEND) {}
    
    void SkeletonNode::Draw(Graphics& g) const {
        if (!m_image) return;
        Transform2d tr = g.GetTransform();
        Color c = g.GetColor();
        BlendMode b = g.GetBlendMode();
        g.SetTransform(tr * m_transform);
        g.SetColor(c * m_color);
        g.SetBlendMode(m_blend);
        g.DrawImage(*m_image,0,0);
        //g.DrawCircle(Vector2f(0,0), 3);
        g.SetColor(c);
        g.SetTransform(tr);
        g.SetBlendMode(b);
    }
    
    void SkeletonNode::SetNode(const SkeletonNodeData &node) {
        m_blend = node.blend;
    }
    void SkeletonNode::SetFrame(const SkeletonNodeFrame &frame) {
        m_color = frame.color;
        m_transform = frame.transform;
    }
    
    void SkeletObject::Draw(Graphics &g) const {
        for (sb::vector<SkeletonNodePtr>::const_iterator it = m_nodes.begin();it!=m_nodes.end();++it) {
            if (*it) (*it)->Draw(g);
        }
    }
    
    void SkeletObject::SetSlot(size_t idx,const SkeletonNodeData& node,const SkeletonNodeFrame& frame, const ImagePtr& image) {
        if (m_nodes.size() <= idx) {
            m_nodes.resize(idx+1);
        }
        SkeletonNode* n = m_nodes[idx].get();
        if (!n) {
            n = new SkeletonNode();
            m_nodes[idx] = SkeletonNodePtr(n);
        }
        n->SetNode(node);
        n->SetFrame(frame);
        n->SetImage(image);
    }
}

