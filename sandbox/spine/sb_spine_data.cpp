#include "sb_spine_data.h"
#include <spine/spine.h>
#include "sb_resources.h"
#include <ghl_data_stream.h>
#include <ghl_data.h>
#include "sb_texture.h"
#include "sb_log.h"
#include <sbstd/sb_assert.h>

static const char* MODULE = "spine";

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
        if (idx < size_t(m_skeleton->slotsCount)) {
            return m_skeleton->slots[idx]->name;
        }
        return "";
    }
    
    ImagePtr SpineData::GetSlotImage(const char* slot_name) const {
        if (!m_skeleton)
            return ImagePtr();
        int slot_index = spSkeletonData_findSlotIndex(m_skeleton, slot_name);
        if (slot_index < 0 || slot_index>=m_skeleton->slotsCount)
            return ImagePtr();
        spSlotData* slot = m_skeleton->slots[slot_index];
        if (!slot || !slot->attachmentName)
            return ImagePtr();
        spAttachment* attachment = spSkin_getAttachment(m_skeleton->defaultSkin, slot_index, slot->attachmentName);
        if (!attachment || attachment->type != SP_ATTACHMENT_REGION)
            return ImagePtr();
        spRegionAttachment* ra = (spRegionAttachment*)attachment;
        spAtlasRegion* region = (spAtlasRegion*)ra->rendererObject;
        if (!region)
            return ImagePtr();
        TexturePtr tex(static_cast<Texture*>(region->page->rendererObject));
        if (!tex) return ImagePtr();
        
        ImagePtr img(new Image(tex,
				  float(region->x),float(region->y),
                  float(region->rotate ? region->height : region->width),
                  float(region->rotate ? region->width : region->height)));
        if (region->rotate) {
            img->SetRotated(true);
            img->SetSize(float(region->width),float(region->height));
        }
        return img;
    }
    
    void SpineData::SetSlotAttribute(size_t idx, const DrawAttributesPtr& attribute) {
        if (idx < size_t(m_skeleton->slotsCount)) {
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
        
        {
            SB_PROFILE("load skeleton");
            
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

        }
        
        if (!res->m_skeleton)
            return SpineDataPtr();
        res->m_state = spAnimationStateData_create(res->m_skeleton);
        
        res->LoadEvents();
        
        return res;
    }
    
    void SpineData::LoadEvents() {
        for (int i=0;i<m_skeleton->eventsCount;++i) {
            EventPtr e(new Event());
            const spEventData* event(m_skeleton->events[i]);
            e->SetString("name", event->name);
            e->SetString("value", event->stringValue);
            e->SetInt("value", event->intValue);
            m_events[event] = e;
        }
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
