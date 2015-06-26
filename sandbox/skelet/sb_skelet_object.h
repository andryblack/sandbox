#ifndef SB_SKELET_OBJECT_H_INCLUDED
#define SB_SKELET_OBJECT_H_INCLUDED

#include "sb_scene_object.h"
#include "sb_image.h"
#include <sbstd/sb_vector.h>

namespace Sandbox {
    
    class SkeletonNode : public SceneObject {
        SB_META_OBJECT
    private:
        Transform2d m_transform;
        float       m_alpha;
        ImagePtr    m_image;
    public:
        virtual void Draw(Graphics& g) const;
        void SetTransform(const Transform2d& tr) { m_transform = tr; }
        void SetAlpha(float a) { m_alpha = a; }
        void SetImage(const ImagePtr& img) { m_image = img; }
    };
    typedef sb::intrusive_ptr<SkeletonNode> SkeletonNodePtr;
    
    class SkeletController;
    
    class SkeletObject : public SceneObject {
        SB_META_OBJECT
    public:
        /// self drawing implementation
        virtual void Draw(Graphics& g) const;
        void SetNode(size_t node, const Transform2d& tr, float a, const ImagePtr& img);
    private:
        sb::vector<SkeletonNodePtr> m_nodes;
    };
    
}

#endif /*SB_SKELET_OBJECT_H_INCLUDED*/
