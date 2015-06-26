//
//  SconverSpine.h
//  AssetsBuilder
//
//  Created by AndryBlack on 24/06/15.
//
//

#ifndef __AssetsBuilder__SconverSpine__
#define __AssetsBuilder__SconverSpine__

#include <stdio.h>
#include <sb_file_provider.h>
#include <spine/spine.h>
#include <skelet/sb_skelet_data.h>
#include <sbstd/sb_string.h>
#include <sbstd/sb_map.h>
#include <pugixml.hpp>

class Application;
class ConverterSpine {
private:
    spAtlas*   m_atlas;
    spSkeletonData* m_skeleton;
    spAnimationStateData* m_state;
    sb::string  m_dir;
    sb::string  m_out_dir;
    sb::string  m_atlas_name;
    void ExportAtlas(Application* app);
    void ExportAnimation();
    pugi::xml_document   m_doc;
    sb::map<sb::string, size_t> m_image_map;
public:
    ConverterSpine();
    ~ConverterSpine();
    bool Load(const char* atlas, const char* skelet,
              Sandbox::FileProvider* file_provider);
    void Export(const char* file, Application* app);
};

#endif /* defined(__AssetsBuilder__SconverSpine__) */
