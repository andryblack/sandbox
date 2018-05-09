#ifndef SB_SPINE_ANIMATION_H_INCLUDED
#define SB_SPINE_ANIMATION_H_INCLUDED

#include "sb_thread.h"
#include "sb_container.h"
#include <spine/AnimationState.h>
#include "sb_signal.h"
#include "sb_rect.h"
#include "sb_color.h"
#include "sb_object_proxy.h"
#include "sb_container_transform.h"


struct spSkeleton;
struct spAnimationState;
struct spAnimation;
struct spEvent;
struct spSlot;

namespace Sandbox {
    
    class SpineData;
    typedef sb::intrusive_ptr<SpineData> SpineDataPtr;
    class Resources;
    
    class SpineAnimation : public Thread {
        SB_META_OBJECT
    public:
        explicit SpineAnimation(const SpineDataPtr& data);
        ~SpineAnimation();
        virtual bool Update(float dt) SB_OVERRIDE;
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
        
        virtual void Clear() SB_OVERRIDE;
        
        Rectf CalcBoundingBox() const;
        void ApplySlotTransform(Transform2d& tr,const sb::string& slot_name) const;
        void ApplySlotTransform(const TransformModificatorPtr& tr,const sb::string& slot_name) const;
    private:
        SpineDataPtr    m_data;
        spSkeleton*     m_skeleton;
        spAnimationState* m_state;
        spAnimation* m_last_animation;
        struct vertices_data {
            struct vertex {
                Vector2f pos;
                Vector2f tex;
            };
            sb::vector<float> world_vertices_pos;
            sb::vector<vertex> vertices;
        };
        sb::vector<vertices_data> m_vertices;

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
    
    class SpineSceneObject;
    
    
    class SpineSceneObject : public Container {
        SB_META_OBJECT
    public:
        explicit SpineSceneObject( const SpineAnimationPtr& animation );
        /// self drawing implementation
        void Draw(Graphics& g) const SB_OVERRIDE;
        void SetAttachement(const sb::string& slot_name,const SceneObjectPtr& object);
        void RemoveAttachement(const sb::string& slot_name);
        ContainerTransformPtr GetAttachment(const sb::string& slot_name);
        void ApplySlotTransform(Transform2d& tr,const sb::string& slot_name) const;
        bool CheckHit(const Vector2f& pos, Resources* resources);
        bool CheckSlotHit(const char* slot,const Vector2f& pos, Resources* resources);
    private:
        SpineAnimationPtr   m_animation;
        typedef sb::map<const void*,ContainerTransformPtr> AttachementMap;
        AttachementMap m_attachements;
        bool CheckSlotHitImpl(spSlot* slot,const Vector2f& pos, Resources* resources);
    };
}

#endif /*SB_SPINE_ANIMATION_H_INCLUDED*/
