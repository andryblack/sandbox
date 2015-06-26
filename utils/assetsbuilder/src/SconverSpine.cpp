//
//  SconverSpine.cpp
//  AssetsBuilder
//
//  Created by AndryBlack on 24/06/15.
//
//

#include "SconverSpine.h"
#include <ghl_data_stream.h>
#include <ghl_data.h>
#include <fstream>
#include "skelet/sb_skelet_data.h"
#include <sb_data.h>
#include <sb_base64.h>
#include "application.h"


extern "C" void _spAtlasPage_createTexture (spAtlasPage* self, const char* path){
    Sandbox::FileProvider* res = static_cast<Sandbox::FileProvider*>(self->atlas->rendererObject);
    if (!res) return;
    /*
    Sandbox::TexturePtr tex = res->GetTexture(path, false);
    if (!tex) return;
    tex->add_ref();
    self->rendererObject = tex.get();
    tex->SetFiltered(true);
    self->width = tex->GetWidth();
    self->height = tex->GetHeight();
     */
}

extern "C" void _spAtlasPage_disposeTexture (spAtlasPage* self){
    //static_cast<Sandbox::Texture*>(self->rendererObject)->remove_ref();
}

extern "C" char* _spUtil_readFile (const char* path, int* length){
    //sb_assert(false && "invalid call");
    return 0;
}

ConverterSpine::ConverterSpine() : m_atlas(0),m_skeleton(0),m_state(0) {
    
}

ConverterSpine::~ConverterSpine() {
    if (m_atlas) spAtlas_dispose(m_atlas);
    if (m_state) spAnimationStateData_dispose(m_state);
    if (m_skeleton) spSkeletonData_dispose(m_skeleton);
}

bool ConverterSpine::Load(const char *atlas_file,
                          const char *skeleton_file,
                          Sandbox::FileProvider *file_provider) {
    
    const char* atlas_name = ::strrchr(atlas_file, '/');
    if (!atlas_name)
        atlas_name = atlas_file;
    const char* atlas_dot = ::strrchr(atlas_name, '.');
    if (atlas_dot) {
        m_atlas_name = sb::string(atlas_name,atlas_dot);
    } else {
        m_atlas_name = atlas_name;
    }
    GHL::DataStream* ds = file_provider->OpenFile(atlas_file);
    if (!ds) return false;
    GHL::Data* data = GHL_ReadAllData(ds);
    ds->Release();
    if (!data)
        return false;
    m_dir = atlas_file;
    size_t spos = m_dir.rfind('/');
    if (spos!=m_dir.npos) {
        m_dir = sb::string(m_dir.c_str(),m_dir.c_str()+spos);
    } else {
        m_dir.clear();
    }
    m_atlas = spAtlas_create(reinterpret_cast<const char*>(data->GetData()), data->GetSize(), m_dir.c_str(), file_provider);
    data->Release();
    if (!m_atlas)
        return false;
    
    spSkeletonJson* json = spSkeletonJson_create(m_atlas);
    ds = file_provider->OpenFile(skeleton_file);
    if (!ds)
        return false;
    data = GHL_ReadAllData(ds);
    ds->Release();
    if (!data)
        return false;
    m_skeleton = spSkeletonJson_readSkeletonData(json, reinterpret_cast<const char*>(data->GetData()));
    spSkeletonJson_dispose(json);
    data->Release();
    if (!m_skeleton)
        return false;
    m_state = spAnimationStateData_create(m_skeleton);
    return true;
}
void ConverterSpine::ExportAtlas(Application* app) {
    pugi::xml_node textures = m_doc.document_element().append_child("textures");
    spAtlasPage* page = m_atlas->pages;
    int index = 0;
    while (page) {
        pugi::xml_node texture = textures.append_child("texture");
        texture.append_attribute("file").set_value(page->name);
        texture.append_attribute("premultiplied").set_value(true);
        spAtlasRegion* region = m_atlas->regions;
        while (region) {
            if (region->page == page) {
                ++index;
                pugi::xml_node image = texture.append_child("image");
                image.append_attribute("x").set_value(region->x);
                image.append_attribute("y").set_value(region->y);
                image.append_attribute("w").set_value(region->rotate ? region->height : region->width);
                image.append_attribute("h").set_value(region->rotate ? region->width : region->height);
                image.append_attribute("r").set_value(bool(region->rotate));
                image.append_attribute("index").set_value(index);
                image.append_attribute("name").set_value(region->name);
                m_image_map[region->name]=index;
            }
            region = region->next;
        }
        app->premultiply_image(m_dir + "/" + page->name, m_out_dir + "/" + page->name);
        page = page->next;
    }
}

static const float export_fps = 24.0f;

void ConverterSpine::ExportAnimation() {
    
    pugi::xml_node nodes = m_doc.document_element().append_child("nodes");
    nodes.append_attribute("count").set_value(m_skeleton->slotsCount);
    for (int i=0;i<m_skeleton->slotsCount;++i) {
        pugi::xml_node n = nodes.append_child("node");
        n.append_attribute("name").set_value(m_skeleton->slots[i]->name);
        n.append_attribute("index").set_value(static_cast<unsigned int>(i));
    }
    
    spAnimationState* state = spAnimationState_create(m_state);
    spSkeleton* skeleton = spSkeleton_create(m_skeleton);
    
    pugi::xml_node animations = m_doc.document_element().append_child("animations");
    
    for (int i=0;i<m_skeleton->animationsCount;++i) {
        spAnimation* anim = m_skeleton->animations[i];
        size_t frames = anim->duration * export_fps;
        if (frames<1) {
            frames = 1;
        }
        
        pugi::xml_node a = animations.append_child("animation");
        a.append_attribute("name").set_value(anim->name);
        a.append_attribute("fps").set_value(export_fps);
        a.append_attribute("frames").set_value(static_cast<unsigned int>(frames));
        a.append_attribute("compression").set_value("zlib");
        a.append_attribute("encoding").set_value("base64");
       
        Sandbox::VectorData<float>* data = new Sandbox::VectorData<float>();
        data->vector().resize((4+2+1+1)*frames*m_skeleton->slotsCount);
        
        
        float delta = 1.0f / export_fps;
        spAnimationState_setAnimation(state, 0, anim, false);
        float* dst = data->vector().data();
        for (size_t f = 0;f<frames;++f ) {
            spAnimationState_update(state, delta);
            spAnimationState_apply(state, skeleton);
            spSkeleton_updateWorldTransform(skeleton);
            for (int s=0;s<skeleton->slotsCount;s++) {
                spSlot* slot = skeleton->drawOrder[s];
                spBone* bone = slot->bone;
                
                //ox = v.x+m.matrix[0*2+0]*x + m.matrix[1*2+0]*y;
                //oy = v.y+m.matrix[0*2+1]*x + m.matrix[1*2+1]*y;
                //vertices[SP_VERTEX_X1] = offset[SP_VERTEX_X1] * bone->m00 + offset[SP_VERTEX_Y1] * bone->m01 + x;
                //vertices[SP_VERTEX_Y1] = offset[SP_VERTEX_X1] * bone->m10 + offset[SP_VERTEX_Y1] * bone->m11 + y;
                
                

                Sandbox::Transform2d tr;
                tr.m.matrix[0*2+0] = bone->m00;
                tr.m.matrix[0*2+1] = bone->m10;
                tr.m.matrix[1*2+0] = bone->m01;
                tr.m.matrix[1*2+1] = bone->m11;
                tr.v.x = skeleton->x + bone->worldX;
                tr.v.y = skeleton->y + bone->worldY;
                
                
                size_t img = 0;
                if (slot->attachment) {
                    sb::map<sb::string, size_t>::const_iterator it = m_image_map.find(slot->attachment->name);
                    if (it!=m_image_map.end()) {
                        img = it->second;
                    }
                    if (slot->attachment->type == SP_ATTACHMENT_REGION) {
                        spRegionAttachment* ra = (spRegionAttachment*)slot->attachment;
                        
                        float regionScaleX = ra->width / ra->regionOriginalWidth * ra->scaleX;
                        float regionScaleY = ra->height / ra->regionOriginalHeight * ra->scaleY;
                        float localX = -ra->width / 2 * ra->scaleX + ra->regionOffsetX * regionScaleX;
                        float localY = -ra->height / 2 * ra->scaleY + ra->regionOffsetY * regionScaleY;
                        float radians = ra->rotation * M_PI / 180.0f;
                        Sandbox::Transform2d trAttachment;
                        trAttachment.translate(ra->x,ra->y);
                        trAttachment.rotate(radians);
                        trAttachment.translate(localX, -localY);
                        trAttachment.scale(regionScaleX,-regionScaleY);
                        tr =  tr * trAttachment;
                        
                    }
                }
                
                *dst++ = skeleton->a * slot->a;
                *dst++ = tr.m.matrix[0];
                *dst++ = tr.m.matrix[1];
                *dst++ = tr.m.matrix[2];
                *dst++ = tr.m.matrix[3];
                *dst++ = tr.v.x;
                *dst++ = tr.v.y;
                
                *reinterpret_cast<GHL::UInt32*>(dst) = img;
                ++dst;
            }
        }
        GHL::Data* compressed = GHL_PackZlib(data);
        a.append_child(pugi::node_pcdata).set_value(Sandbox::Base64Encode(compressed->GetData(), compressed->GetSize()).c_str());
        compressed->Release();
        data->Release();
    }
    spAnimationState_dispose(state);
    spSkeleton_dispose(skeleton);
}

struct xml_writer : public pugi::xml_writer {
    Sandbox::VectorData<GHL::Byte>* sdata;
    virtual void write(const void* data, size_t size) {
        size_t idx = sdata->vector().size();
        sdata->vector().resize(idx+size);
        ::memcpy(sdata->vector().data()+idx, data, size);
    }
};


void ConverterSpine::Export(const char *file,Application* app) {
    const char* last_slash = ::strrchr(file, '/');
    if (last_slash) {
        m_out_dir.assign(file,last_slash);
    }
    m_doc.root().append_child("skeleton");
    ExportAtlas(app);
    ExportAnimation();
    xml_writer writer;
    writer.sdata = new Sandbox::VectorData<GHL::Byte>();;
    m_doc.save(writer);
    app->store_file(file, writer.sdata);
    writer.sdata->Release();
}