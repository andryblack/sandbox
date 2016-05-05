#ifndef SB_SKELET_DATA_H_INCLUDED
#define SB_SKELET_DATA_H_INCLUDED

#include "sb_transform2d.h"
#include "sb_graphics.h"

namespace GHL {
    struct Data;
}

namespace Sandbox {

    class Image;
    typedef sb::intrusive_ptr<Image> ImagePtr;
    
    struct SkeletonNodeFrame {
        Transform2d transform;
        Color   color;
        size_t  image;
        size_t  node;
    };
    
    class Resources;
    
    class SkeletonAnimation : public meta::object {
        SB_META_OBJECT
    private:
        sb::string          m_name;
        SkeletonNodeFrame*  m_data;
        float               m_fps;
        size_t              m_nodes;
        size_t              m_frames;
    public:
        explicit SkeletonAnimation( const sb::string& name );
        ~SkeletonAnimation();
        SkeletonNodeFrame* AllocData(size_t nodes,size_t frames);
        const sb::string& GetName() const { return m_name; }
        float GetFPS() const { return m_fps; }
        void SetFPS(float fps) { m_fps = fps; }
        size_t GetFrames() const { return m_frames; }
        size_t GetNodesCount() const { return m_nodes; }
        const SkeletonNodeFrame& GetNodeFrame(size_t frame,size_t node) const;
    };
    typedef sb::intrusive_ptr<SkeletonAnimation> SkeletonAnimationPtr;

    struct SkeletonNodeData {
        sb::string name;
        BlendMode blend;
    };
    
    class SkeletonData;
    typedef sb::intrusive_ptr<SkeletonData> SkeletonDataPtr;
    class SkeletonData : public meta::object {
        SB_META_OBJECT
    public:
    private:
        sb::map<sb::string,SkeletonAnimationPtr>    m_animations;
        sb::vector<ImagePtr>    m_images;
        sb::map<sb::string,size_t> m_images_map;
        
        sb::vector<SkeletonNodeData> m_nodes;
    public:
        void AddNode(const SkeletonNodeData& n) { m_nodes.push_back(n); }
        size_t GetNodesCount() const { return m_nodes.size(); }
        const sb::vector<SkeletonNodeData>& GetNodes() const { return m_nodes; }
        void AddAnimation( const SkeletonAnimationPtr& animation);
        static SkeletonDataPtr Load(const char* file, Resources* resources);
        const SkeletonAnimationPtr& GetAnimation(const sb::string& name) const;
        const SkeletonNodeData& GetNode(size_t idx) const;
        const ImagePtr& GetImage(size_t index) const;
        void SetImage(size_t index,const sb::string& name, const ImagePtr& img);
        bool HasAnimation( const sb::string& name ) const;
    };
    
}

#endif /*SB_SKELET_DATA_H_INCLUDED*/
