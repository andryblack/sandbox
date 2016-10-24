#ifndef SB_SKELET_CONTROLLER_H_INCLUDED
#define SB_SKELET_CONTROLLER_H_INCLUDED

#include "sb_thread.h"
#include "sb_transform2d.h"
#include "sb_signal.h"
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
        void ChangeData( const SkeletonDataPtr& data );
        virtual bool Update(float dt);
        void StartAnimation(const char* name,int loop);
        void SetAnimation(const char* name);
        void AddAnimation(const char* name,int loop);
        virtual void ApplyFrame();
        void AddObject(const SkeletObjectPtr& obj);
        bool HasAnimation(const char* name) const;
        
        void SetOnEnd(const SignalPtr& e) { m_end_signal = e;}
        const SignalPtr& GetOnEnd() const { return m_end_signal; }
        
        SkeletonAnimation* GetAnimation() { return m_animation; }
        void SetFrame( size_t frame );
        void SetTime( float time );
        
        float GetCurrentAnimationLength() const;
        size_t GetCurrentAnimationFrames() const;
        
        bool IsComplete() const { return !m_animation || !m_started; }
        
        virtual void Clear();
    protected:
        size_t  m_crnt_frame;
        float   m_frame_time;
        bool    m_started;
        SkeletonDataPtr     m_data;
        SkeletonAnimation*  m_animation;
        int    m_loop;
        sb::vector<SkeletObjectPtr> m_objects;
        SignalPtr    m_end_signal;
        struct AnimationEntry {
            sb::string name;
            int loop;
        };
        sb::list<AnimationEntry> m_added_animations;
        void InitObjectNodes(const SkeletObjectPtr& obj);
    };
    typedef sb::intrusive_ptr<SkeletController> SkeletControllerPtr;
    
    class InterpolatedSkeletController: public SkeletController {
        SB_META_OBJECT
    public:
        explicit InterpolatedSkeletController( const SkeletonDataPtr& data );
        virtual void ApplyFrame();
    };
}

#endif /*SB_SKELET_CONTROLLER_H_INCLUDED*/
