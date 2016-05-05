#ifndef SB_SKELET_OBJECT_H_INCLUDED
#define SB_SKELET_OBJECT_H_INCLUDED

#include "sb_scene_object.h"
#include "sb_image.h"
#include "sb_graphics.h"

#include <sbstd/sb_vector.h>

namespace Sandbox {
    
    class SkeletonNodeFrame;
    class SkeletonNodeData;
    
    class SkeletonNode : public SceneObject {
        SB_META_OBJECT
    private:
        Transform2d m_transform;
        Sandbox::Color       m_color;
        ImagePtr    m_image;
        BlendMode   m_blend;
    public:
        SkeletonNode();
        virtual void Draw(Graphics& g) const;
        void SetNode(const SkeletonNodeData& mode);
        void SetFrame(const SkeletonNodeFrame& frame);
        void SetImage(const ImagePtr& img) { m_image = img; }
    };
    typedef sb::intrusive_ptr<SkeletonNode> SkeletonNodePtr;
    
    class SkeletController;
    
    
    class SkeletObject : public SceneObject {
        SB_META_OBJECT
    public:
        /// self drawing implementation
        virtual void Draw(Graphics& g) const;
        void SetSlot(size_t idx,
                     const SkeletonNodeData& node,
                     const SkeletonNodeFrame& frame,
                     const ImagePtr& image);
    private:
        sb::vector<SkeletonNodePtr> m_nodes;
    };
    
}

#endif /*SB_SKELET_OBJECT_H_INCLUDED*/
