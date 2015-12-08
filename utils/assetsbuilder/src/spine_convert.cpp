//
//
//  AssetsBuilder
//
//  Created by AndryBlack on 24/06/15.
//
//

#include "spine_convert.h"
#include <ghl_data_stream.h>
#include <ghl_data.h>
#include <fstream>
#include "skelet/sb_skelet_data.h"
#include <sb_data.h>
#include <sb_base64.h>
#include <sb_log.h>
#include "application.h"

SB_META_DECLARE_OBJECT(SpineConvert, SkeletonConvert)


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

SpineConvert::SpineConvert() : m_atlas(0),m_skeleton(0),m_state(0) {
    
}

SpineConvert::~SpineConvert() {
    if (m_atlas) spAtlas_dispose(m_atlas);
    if (m_state) spAnimationStateData_dispose(m_state);
    if (m_skeleton) spSkeletonData_dispose(m_skeleton);
}

bool SpineConvert::Load(const char *atlas_file,
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
    if (!ds) {
        Sandbox::LogError() << "not found " << atlas_file;
        return false;
    }
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
    if (!ds) {
        Sandbox::LogError() << "not found " << skeleton_file;
        return false;
    }
    data = GHL_ReadAllData(ds);
    ds->Release();
    if (!data)
        return false;
    m_skeleton = spSkeletonJson_readSkeletonData(json, reinterpret_cast<const char*>(data->GetData()));
    if (json->error) {
        Sandbox::LogError() << "parsing skeleton error: '" << json->error << "'";
    }
    spSkeletonJson_dispose(json);
    data->Release();
    if (!m_skeleton) {
        return false;
    }
    m_state = spAnimationStateData_create(m_skeleton);
    return true;
}
void SpineConvert::ExportAtlas(Application* app) {
    spAtlasPage* page = m_atlas->pages;
    while (page) {
        atlas& a = add_atlas(page->name, true);
        spAtlasRegion* region = m_atlas->regions;
        while (region) {
            if (region->page == page) {
                image& img = add_image(a, region->name);
                img.x = region->x;
                img.y = region->y;
                img.w = (region->rotate ? region->height : region->width);
                img.h = (region->rotate ? region->width : region->height);
                img.r = (bool(region->rotate));
                float oy = (region->originalHeight-region->height-region->offsetY);
                img.hsx = region->originalWidth / 2 - region->offsetX;//-(region->rotate ? oy : region->offsetX);
                img.hsy = region->originalHeight / 2 - oy;//-(region->rotate ? region->offsetX : oy);
            }
            region = region->next;
        }
        sb::string output_name = m_out_name;
        output_name += "_";
        output_name += page->name;
        a.filename = output_name;
        app->premultiply_image(m_dir + "/" + page->name, m_out_dir + "/" + output_name);
        page = page->next;
    }
    write_atlases();
}

static const float export_fps = 24.0f;

void SpineConvert::ExportAnimation() {
    
    for (int i=0;i<m_skeleton->slotsCount;++i) {
        add_node(m_skeleton->slots[i]->name);
    }
    write_nodes();
    
    spAnimationState* state = spAnimationState_create(m_state);
    spSkeleton* skeleton = spSkeleton_create(m_skeleton);
    
    for (int i=0;i<m_skeleton->animationsCount;++i) {
        spAnimation* anim = m_skeleton->animations[i];
        size_t frames = anim->duration * export_fps;
        if (frames<1) {
            frames = 1;
        }
        animation& a = add_animation(anim->name, export_fps);
        if (a.name == "Idle") {
            int x = 1;
        }
        
        float delta = 1.0f / export_fps;
        
        spSkeleton_setToSetupPose(skeleton);
        
        spAnimationState_setAnimation(state, 0, anim, false);
       
        for (size_t f = 0;f<frames;++f ) {
            frame& fr = add_frame(a);
            spAnimationState_update(state, delta);
            spAnimationState_apply(state, skeleton);
            spSkeleton_updateWorldTransform(skeleton);
            for (int s=0;s<skeleton->slotsCount;s++) {
                spSlot* slot = skeleton->drawOrder[s];
                spBone* bone = slot->bone;
                frame::node& n = add_frame_node(fr);
                
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
                    sb::map<sb::string, int>::const_iterator it = m_image_indexes.find(slot->attachment->name);
                    if (it!=m_image_indexes.end()) {
                        img = it->second;
                    } else {
                        Sandbox::LogWarning() << "not found attachment image " << slot->attachment->name;
                    }
                    if (slot->attachment->type == SP_ATTACHMENT_REGION) {
                        spRegionAttachment* ra = (spRegionAttachment*)slot->attachment;
                        
                        float regionScaleX = ra->width / ra->regionOriginalWidth * ra->scaleX;
                        float regionScaleY = ra->height / ra->regionOriginalHeight * ra->scaleY;
                        //float localX = -ra->width / 2 * ra->scaleX + ra->regionOffsetX * regionScaleX;
                        //float localY = -ra->height / 2 * ra->scaleY + ra->regionOffsetY * regionScaleY;
                        float radians = ra->rotation * M_PI / 180.0f;
                        Sandbox::Transform2d trAttachment;
                        trAttachment.translate(ra->x,ra->y);
                        trAttachment.rotate(radians);
                        //trAttachment.translate(localX, -localY);
                        trAttachment.scale(regionScaleX,-regionScaleY);
                        tr =  tr * trAttachment;
                        
                    } else {
                        Sandbox::LogWarning() << "unexpected attachment type " << slot->attachment->type;
                    }
                }
                
                tr = Sandbox::Transform2d().scale(1.0f,-1.0f) * tr;
                
                n.tr = tr;
                n.a = skeleton->a * slot->a;
                n.image = img;
                
            }
        }
       
    }
    spAnimationState_dispose(state);
    spSkeleton_dispose(skeleton);
    write_animations();
}





void SpineConvert::Export(const char *file,Application* app) {
    const char* last_slash = ::strrchr(file, '/');
    if (last_slash) {
        m_out_name = last_slash + 1;
        size_t dotpos = m_out_name.rfind('.');
        if (dotpos!=m_out_name.npos) {
            m_out_name.resize(dotpos);
        }
        m_out_dir.assign(file,last_slash);
    }
    m_doc.root().append_child("skeleton");
    ExportAtlas(app);
    ExportAnimation();
    store_xml(file, app);
}