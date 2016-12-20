#ifndef SB_SPINE_ANIMATION_H_INCLUDED
#define SB_SPINE_ANIMATION_H_INCLUDED

#include "sb_thread.h"
#include "sb_scene_object.h"
#include <spine/AnimationState.h>
#include "sb_signal.h"
#include "sb_rect.h"
#include "sb_color.h"


struct spSkeleton;
struct spAnimationState;
struct spAnimation;
struct spEvent;

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
        
        void SetOnEnd(const SignalPtr& e) { m_end_signal = e;}
        const SignalPtr& GetOnEnd() const { return m_end_signal; }
        
        void SetOnEvent(const SignalPtr& e) { m_event_signal = e;}
        const SignalPtr& GetOnEvent() const { return m_event_signal; }
        
        Color GetSlotColor(const char* slot_name) const;
        
        void SetTime( float time );
        float GetCurrentAnimationLength() const;
        
        bool IsComplete() const;
        
        virtual void Clear();
        
        Rectf CalcBoundingBox() const;
    private:
        SpineDataPtr    m_data;
        spSkeleton*     m_skeleton;
        spAnimationState* m_state;
        spAnimation* m_last_animation;
        friend class SpineSceneObject;
        friend void spine_animation_event_listener(spAnimationState* state, int trackIndex, spEventType type, spEvent* event,
                                                        int loopCount);
        void OnAnimationComplete();
        void OnAnimationStarted();
        void OnAnimationEvent(spEvent* e);
        bool    m_block_events;
        SignalPtr    m_end_signal;
        SignalPtr    m_event_signal;
    };
    typedef sb::intrusive_ptr<SpineAnimation> SpineAnimationPtr;
    
    class SpineSceneObject : public SceneObjectWithPosition {
        SB_META_OBJECT
    public:
        explicit SpineSceneObject( const SpineAnimationPtr& animation );
        /// self drawing implementation
        virtual void Draw(Graphics& g) const;
        void SetAttachement(const sb::string& slot_name,const SceneObjectPtr& object);
        void RemoveAttachement(const sb::string& slot_name);
    private:
        SpineAnimationPtr   m_animation;
        typedef sb::map<sb::string,SceneObjectPtr> AttachementMap;
        AttachementMap m_attachements;
    };
    
}

#endif /*SB_SPINE_ANIMATION_H_INCLUDED*/
