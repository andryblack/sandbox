#ifndef SB_SPINE_ANIMATION_H_INCLUDED
#define SB_SPINE_ANIMATION_H_INCLUDED

#include "sb_thread.h"
#include "sb_scene_object.h"
#include "sb_event.h"

struct spSkeleton;
struct spAnimationState;
struct spAnimation;

namespace Sandbox {
    
    class SpineData;
    typedef sb::intrusive_ptr<SpineData> SpineDataPtr;
    
    class SpineAnimation : public Thread {
        SB_META_OBJECT
    public:
        explicit SpineAnimation(const SpineDataPtr& data);
        ~SpineAnimation();
        virtual bool Update(float dt);
        void StartAnimation(const char* name,int loop);
        void SetAnimation(const char* name);
        void AddAnimation(const char* name,int loop);
        bool HasAnimation(const char* name) const;
        
        void SetEndEvent(const EventPtr& e) { m_end_event = e;}
        const EventPtr& GetEndEvent() const { return m_end_event; }
        
        void SetTime( float time );
        float GetCurrentAnimationLength() const;
        
        bool IsComplete() const;
        
    private:
        SpineDataPtr    m_data;
        spSkeleton*     m_skeleton;
        spAnimationState* m_state;
        spAnimation* m_last_animation;
        friend class SpineSceneObject;
        friend void OnAnimationComplete(SpineAnimation*);
        void OnAnimationComplete();
        friend void OnAnimationStarted(SpineAnimation*);
        void OnAnimationStarted();
        EventPtr    m_end_event;
    };
    typedef sb::intrusive_ptr<SpineAnimation> SpineAnimationPtr;
    
    class SpineSceneObject : public SceneObjectWithPosition {
        SB_META_OBJECT
    public:
        explicit SpineSceneObject( const SpineAnimationPtr& animation );
        /// self drawing implementation
        virtual void Draw(Graphics& g) const;
    private:
        SpineAnimationPtr   m_animation;
    };
    
}

#endif /*SB_SPINE_ANIMATION_H_INCLUDED*/
