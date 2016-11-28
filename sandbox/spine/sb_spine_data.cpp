#include "sb_spine_data.h"
#include <spine/spine.h>
#include "sb_resources.h"
#include <ghl_data_stream.h>
#include <ghl_data.h>
#include "sb_texture.h"
#include "sb_log.h"
#include <sbstd/sb_assert.h>

SB_META_DECLARE_OBJECT(Sandbox::SpineData, Sandbox::meta::object)


extern "C" void _spAtlasPage_createTexture (spAtlasPage* self, const char* path){
    Sandbox::TextureProvider* res = static_cast<Sandbox::TextureProvider*>(self->atlas->rendererObject);
    if (!res) return;
    Sandbox::TexturePtr tex = res->GetTexture(path);
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
    
    Sizef SpineData::GetSize() const {
        if (m_skeleton) {
            return Sizef(m_skeleton->width,m_skeleton->height);
        }
        return Sizef();
    }
    
    size_t SpineData::GetSlotsCount() const {
        return m_skeleton->slotsCount;
    }
    
    sb::string SpineData::GetSlotName(size_t idx) const {
        if (idx < m_skeleton->slotsCount) {
            return m_skeleton->slots[idx]->name;
        }
        return "";
    }
    
    void SpineData::SetSlotAttribute(size_t idx, const DrawAttributesPtr& attribute) {
        if (idx < m_skeleton->slotsCount) {
            m_attributes[m_skeleton->slots[idx]] = attribute;
        }
    }
    static const DrawAttributesPtr empty;
    const DrawAttributesPtr& SpineData::GetSlotAttribute(const void* idx) const {
        sb::map<const void*,DrawAttributesPtr>::const_iterator it = m_attributes.find(idx);
        if (it!=m_attributes.end())
            return it->second;
        return empty;
    }
    static const EventPtr empty_event;
    const EventPtr& SpineData::GetEvent(const void* idx) const {
        sb::map<const void*,EventPtr>::const_iterator it = m_events.find(idx);
        if (it!=m_events.end())
            return it->second;
        return empty_event;
    }
    
    SpineDataPtr SpineData::LoadI(const char* atlas_file,
                                  const char* skeleton_file,
                                  FileProvider* files,
                                  TextureProvider* textures) {
        spBone_setYDown(1);
        GHL::DataStream* ds = files->OpenFile(atlas_file);
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
        
        res->m_atlas = spAtlas_create(reinterpret_cast<const char*>(data->GetData()), data->GetSize(), dir.c_str(), textures);
        data->Release();
        if (!res->m_atlas)
            return SpineDataPtr();
        
        spSkeletonJson* json = spSkeletonJson_create(res->m_atlas);
        ds = files->OpenFile(skeleton_file);
        if (!ds)
            return SpineDataPtr();
        data = GHL_ReadAllData(ds);
        ds->Release();
        if (!data)
            return SpineDataPtr();
        res->m_skeleton = spSkeletonJson_readSkeletonData(json, reinterpret_cast<const char*>(data->GetData()));
        if (!res->m_skeleton && json->error) {
            LogError() << "[spine] " << json->error;
        }
        spSkeletonJson_dispose(json);
        data->Release();
        if (!res->m_skeleton)
            return SpineDataPtr();
        res->m_state = spAnimationStateData_create(res->m_skeleton);
        
        for (int i=0;i<res->m_skeleton->eventsCount;++i) {
            EventPtr e(new Event());
            const spEventData* event(res->m_skeleton->events[i]);
            e->SetString("name", event->name);
            e->SetString("value", event->stringValue);
            e->SetInt("value", event->intValue);
            res->m_events[event] = e;
        }
        
        return res;
    }
    
    SpineDataPtr SpineData::Load(const char* atlas_file,
                     const char* skeleton_file,
                     Resources* resources) {
        if (!resources) {
            return SpineDataPtr();
        }
        return LoadI(atlas_file, skeleton_file, resources, resources);
    }
    
    void SpineData::SetDefaultMix(float duration) {
        m_state->defaultMix = duration;
    }
    
    void SpineData::SetMix(const char* from, const char* to, float duration) {
        spAnimationStateData_setMixByName(m_state, from, to, duration);
    }
    
}
