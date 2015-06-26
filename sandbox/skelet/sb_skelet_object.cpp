#include "sb_skelet_object.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::SkeletonNode,Sandbox::SceneObject)
SB_META_DECLARE_OBJECT(Sandbox::SkeletObject,Sandbox::SceneObject)

namespace Sandbox {
    
    void SkeletonNode::Draw(Graphics& g) const {
        if (!m_image) return;
        Transform2d tr = g.GetTransform();
        Color c = g.GetColor();
        g.SetTransform(tr * m_transform);
        g.SetColor(c * Color(1.0f,1.0f,1.0f,m_alpha));
        g.DrawImage(*m_image,0,0);
        //g.DrawCircle(Vector2f(0,0), 3);
        g.SetColor(c);
        g.SetTransform(tr);
    }
    
    void SkeletObject::Draw(Graphics &g) const {
        for (sb::vector<SkeletonNodePtr>::const_iterator it = m_nodes.begin();it!=m_nodes.end();++it) {
            if (*it) (*it)->Draw(g);
        }
    }
    
    void SkeletObject::SetNode(size_t node, const Transform2d& tr, float a, const ImagePtr& img) {
        if (m_nodes.size() <= node) {
            m_nodes.resize(node+1);
        }
        SkeletonNode* n = m_nodes[node].get();
        if (!n) {
            n = new SkeletonNode();
            m_nodes[node] = SkeletonNodePtr(n);
        }
        n->SetTransform(tr);
        n->SetAlpha(a);
        n->SetImage(img);
    }
}

