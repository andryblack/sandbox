#ifndef SB_SKELET_CONTROLLER_H_INCLUDED
#define SB_SKELET_CONTROLLER_H_INCLUDED

#include "sb_thread.h"
#include "sb_transform2d.h"

namespace Sandbox {
    
    class SkeletonData;
    typedef sb::intrusive_ptr<SkeletonData> SkeletonDataPtr;
    class SkeletonAnimation;
    class SkeletObject;
    typedef sb::intrusive_ptr<SkeletObject> SkeletObjectPtr;
        
    class SkeletController : public Thread {
        SB_META_OBJECT
    public:
        explicit SkeletController( const SkeletonDataPtr& data );
        virtual bool Update(float dt);
        void StartAnimation(const char* name,bool loop);
        void ApplyFrame();
        void AddObject(const SkeletObjectPtr& obj);
    private:
        size_t  m_crnt_frame;
        float   m_frame_time;
        SkeletonDataPtr     m_data;
        SkeletonAnimation*  m_animation;
        bool    m_loop;
        sb::vector<SkeletObjectPtr> m_objects;
    };
    
}

#endif /*SB_SKELET_CONTROLLER_H_INCLUDED*/