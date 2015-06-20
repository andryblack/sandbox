#ifndef SB_SPINE_ANIMATION_H_INCLUDED
#define SB_SPINE_ANIMATION_H_INCLUDED

#include "sb_thread.h"
#include "sb_scene_object.h"
#include "sb_geometry.h"

struct spSkeleton;
struct spAnimationState;

namespace Sandbox {
    
    class SpineData;
    typedef sb::intrusive_ptr<SpineData> SpineDataPtr;
    
    class SpineAnimation : public Thread {
        SB_META_OBJECT
    public:
        explicit SpineAnimation(const SpineDataPtr& data);
        ~SpineAnimation();
        virtual bool Update(float dt);
        void StartAnimation(const char* name,bool loop);
    private:
        SpineDataPtr    m_data;
        spSkeleton*     m_skeleton;
        spAnimationState* m_state;
        friend class SpineSceneObject;
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
        mutable GeometryData        m_data;
    };
    
}

#endif /*SB_SPINE_ANIMATION_H_INCLUDED*/
