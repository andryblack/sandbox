#ifndef SB_SKELET_CONTROLLER_H_INCLUDED
#define SB_SKELET_CONTROLLER_H_INCLUDED

#include "sb_thread.h"
#include "sb_transform2d.h"
#include "sb_event.h"
#include <sbstd/sb_string.h>
#include <sbstd/sb_list.h>

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
        void StartAnimation(const char* name,int loop);
        void AddAnimation(const char* name,int loop);
        void ApplyFrame();
        void AddObject(const SkeletObjectPtr& obj);
        
        void SetEndEvent(const EventPtr& e) { m_end_event = e;}
        const EventPtr& GetEndEvent() const { return m_end_event; }
        
        SkeletonAnimation* GetAnimation() { return m_animation; }
        void SetFrame( size_t frame );
    private:
        size_t  m_crnt_frame;
        float   m_frame_time;
        SkeletonDataPtr     m_data;
        SkeletonAnimation*  m_animation;
        int    m_loop;
        sb::vector<SkeletObjectPtr> m_objects;
        EventPtr    m_end_event;
        struct AnimationEntry {
            sb::string name;
            int loop;
        };
        sb::list<AnimationEntry> m_added_animations;
    };
    typedef sb::intrusive_ptr<SkeletController> SkeletControllerPtr;
}

#endif /*SB_SKELET_CONTROLLER_H_INCLUDED*/