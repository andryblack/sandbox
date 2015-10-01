//
//  SconverSpine.h
//  AssetsBuilder
//
//  Created by AndryBlack on 24/06/15.
//
//

#ifndef __AssetsBuilder__spine_convert__
#define __AssetsBuilder__spine_convert__

#include "skeleton_convert.h"

#include <stdio.h>
#include <sb_file_provider.h>
#include <spine/spine.h>
#include <skelet/sb_skelet_data.h>
#include <pugixml.hpp>
#include <sbstd/sb_map.h>

class Application;
class SpineConvert : public SkeletonConvert {
    SB_META_OBJECT
private:
    spAtlas*   m_atlas;
    spSkeletonData* m_skeleton;
    spAnimationStateData* m_state;
    sb::string  m_atlas_name;
    void ExportAtlas(Application* app);
    void ExportAnimation();
public:
    SpineConvert();
    ~SpineConvert();
    bool Load(const char* atlas, const char* skelet,
              Sandbox::FileProvider* file_provider);
    void Export(const char* file, Application* app);
    
    
};

#endif /* defined(__AssetsBuilder__spine_convert__) */
