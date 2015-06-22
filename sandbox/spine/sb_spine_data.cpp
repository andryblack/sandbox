#include "sb_spine_data.h"
#include <spine/spine.h>
#include "sb_resources.h"
#include <ghl_data_stream.h>
#include <ghl_data.h>
#include "sb_texture.h"
#include <sbstd/sb_assert.h>

SB_META_DECLARE_OBJECT(Sandbox::SpineData, Sandbox::meta::object)


extern "C" void _spAtlasPage_createTexture (spAtlasPage* self, const char* path){
    Sandbox::Resources* res = static_cast<Sandbox::Resources*>(self->atlas->rendererObject);
    if (!res) return;
    Sandbox::TexturePtr tex = res->GetTexture(path, false);
    if (!tex) return;
    tex->add_ref();
    self->rendererObject = tex.get();
    tex->SetFiltered(true);
    self->width = tex->GetWidth();
    self->height = tex->GetHeight();
}

extern "C" void _spAtlasPage_disposeTexture (spAtlasPage* self){
    static_cast<Sandbox::Texture*>(self->rendererObject)->remove_ref();
}

extern "C" char* _spUtil_readFile (const char* path, int* length){
    sb_assert(false && "invalid call");
    return 0;
}


namespace Sandbox {
    
    
    SpineData::SpineData() : m_atlas(0), m_skeleton(0), m_state(0) {
        
    }
    
    SpineData::~SpineData() {
        if (m_atlas) spAtlas_dispose(m_atlas);
        if (m_state) spAnimationStateData_dispose(m_state);
        if (m_skeleton) spSkeletonData_dispose(m_skeleton);
    }
    
    SpineDataPtr SpineData::Load(const char* atlas_file,
                                 const char* skeleton_file,Resources* resources) {
        GHL::DataStream* ds = resources->OpenFile(atlas_file);
        if (!ds) return SpineDataPtr();
        GHL::Data* data = GHL_ReadAllData(ds);
        ds->Release();
        if (!data)
            return SpineDataPtr();
        sb::string dir = atlas_file;
        size_t spos = dir.rfind('/');
        if (spos!=dir.npos) {
            dir = sb::string(dir.c_str(),dir.c_str()+spos);
        } else {
            dir.clear();
        }
        SpineDataPtr res(new SpineData());
        res->m_atlas = spAtlas_create(reinterpret_cast<const char*>(data->GetData()), data->GetSize(), dir.c_str(), resources);
        data->Release();
        if (!res->m_atlas)
            return SpineDataPtr();
        
        spSkeletonJson* json = spSkeletonJson_create(res->m_atlas);
        ds = resources->OpenFile(skeleton_file);
        if (!ds)
            return SpineDataPtr();
        data = GHL_ReadAllData(ds);
        ds->Release();
        if (!data)
            return SpineDataPtr();
        res->m_skeleton = spSkeletonJson_readSkeletonData(json, reinterpret_cast<const char*>(data->GetData()));
        spSkeletonJson_dispose(json);
        data->Release();
        if (!res->m_skeleton)
            return SpineDataPtr();
        res->m_state = spAnimationStateData_create(res->m_skeleton);
        return res;
    }
    
}