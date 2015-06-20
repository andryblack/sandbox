#ifndef SB_SPINE_DATA_H_INCLUDED
#define SB_SPINE_DATA_H_INCLUDED

#include "meta/sb_meta.h"

struct spAtlas;
struct spSkeletonData;
struct spAnimationStateData;

namespace Sandbox {
    
    class Resources;
    class SpineSkeleton;
    class SpineData;
    typedef sb::intrusive_ptr<SpineData> SpineDataPtr;
    
    class SpineData : public meta::object {
        SB_META_OBJECT
    public:
        static SpineDataPtr Load(const char* atlas_file,
                                 const char* skeleton_file,
                                 Resources* resources);
        ~SpineData();
    private:
        SpineData();
        spAtlas*   m_atlas;
        spSkeletonData* m_skeleton;
        spAnimationStateData* m_state;
        friend class SpineAnimation;
    };
    
}

#endif /*SB_SPINE_DATA_H_INCLUDED*/
