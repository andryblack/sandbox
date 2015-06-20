#include "sb_spine_animation.h"
#include "sb_spine_data.h"
#include "sb_graphics.h"
#include <spine/spine.h>

SB_META_DECLARE_OBJECT(Sandbox::SpineAnimation, Sandbox::Thread)
SB_META_DECLARE_OBJECT(Sandbox::SpineSceneObject, Sandbox::SceneObjectWithPosition)

namespace Sandbox {
    
    
    SpineAnimation::SpineAnimation(const SpineDataPtr& data) : m_data(data) {
        m_skeleton = spSkeleton_create(m_data->m_skeleton);
        m_state = spAnimationState_create(m_data->m_state);
    }
    SpineAnimation::~SpineAnimation() {
        if (m_skeleton) {
            spSkeleton_dispose(m_skeleton);
        }
        if (m_state) {
            spAnimationState_dispose(m_state);
        }
    }
    
    void SpineAnimation::StartAnimation(const char* name,bool loop) {
        spAnimationState_setAnimationByName(m_state, 0, name, loop);
    }
    
    bool SpineAnimation::Update(float dt) {
        spSkeleton_update(m_skeleton, dt);
        spAnimationState_update(m_state, dt);
        spAnimationState_apply(m_state, m_skeleton);
        spSkeleton_updateWorldTransform(m_skeleton);
        return false;
    }
 
    
    SpineSceneObject::SpineSceneObject(const SpineAnimationPtr& animation) : m_animation(animation) {
        
    }
    
    void SpineSceneObject::Draw(Graphics &gr) const {
        if (!m_animation)
            return;
        Transform2d tr = gr.GetTransform();
        gr.SetTransform(tr.scaled(1.0f, -1.0f));
        
        float worldVertices[8];
        spSkeleton* skeleton = m_animation->m_skeleton;
        for (int i = 0; i < skeleton->slotsCount; ++i) {
            spSlot* slot = skeleton->drawOrder[i];
            spAttachment* attachment = slot->attachment;
            if (!attachment) continue;
            
            switch (slot->data->blendMode) {
                case SP_BLEND_MODE_ADDITIVE:
                    gr.SetBlendMode(BLEND_MODE_ADDITIVE);
                    break;
                default:
                    gr.SetBlendMode(BLEND_MODE_ALPHABLEND);
                    break;
            }
            
            if (attachment->type == SP_ATTACHMENT_REGION) {
                spRegionAttachment* regionAttachment = (spRegionAttachment*)attachment;
                TexturePtr tex(static_cast<Texture*>(((spAtlasRegion*)regionAttachment->rendererObject)->page->rendererObject));
                if (!tex) continue;
                m_data.texture = tex;
                
                spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, worldVertices);
                m_data.primitives = GHL::PRIMITIVE_TYPE_TRIANGLES;
                m_data.vertexes.resize(4);
                GHL::Vertex* vertices = &m_data.vertexes[0];
                
                GHL::Byte r = static_cast<GHL::Byte>(skeleton->r * slot->r * 255);
                GHL::Byte g = static_cast<GHL::Byte>(skeleton->g * slot->g * 255);
                GHL::Byte b = static_cast<GHL::Byte>(skeleton->b * slot->b * 255);
                GHL::Byte a = static_cast<GHL::Byte>(skeleton->a * slot->a * 255);
                
                //Vector2u size = texture->getSize();
                vertices[0].color[0] = r;
                vertices[0].color[1] = g;
                vertices[0].color[2] = b;
                vertices[0].color[3] = a;
                vertices[0].x = worldVertices[SP_VERTEX_X1];
                vertices[0].y = worldVertices[SP_VERTEX_Y1];
                vertices[0].z = 0;
                vertices[0].tx = regionAttachment->uvs[SP_VERTEX_X1];
                vertices[0].ty = regionAttachment->uvs[SP_VERTEX_Y1];
                
                vertices[1].color[0] = r;
                vertices[1].color[1] = g;
                vertices[1].color[2] = b;
                vertices[1].color[3] = a;
                vertices[1].x = worldVertices[SP_VERTEX_X2];
                vertices[1].y = worldVertices[SP_VERTEX_Y2];
                vertices[1].z = 0;
                vertices[1].tx = regionAttachment->uvs[SP_VERTEX_X2];
                vertices[1].ty = regionAttachment->uvs[SP_VERTEX_Y2];
                
                vertices[2].color[0] = r;
                vertices[2].color[1] = g;
                vertices[2].color[2] = b;
                vertices[2].color[3] = a;
                vertices[2].x = worldVertices[SP_VERTEX_X3];
                vertices[2].y = worldVertices[SP_VERTEX_Y3];
                vertices[3].z = 0;
                vertices[2].tx = regionAttachment->uvs[SP_VERTEX_X3];
                vertices[2].ty = regionAttachment->uvs[SP_VERTEX_Y3];
                
                vertices[3].color[0] = r;
                vertices[3].color[1] = g;
                vertices[3].color[2] = b;
                vertices[3].color[3] = a;
                vertices[3].x = worldVertices[SP_VERTEX_X4];
                vertices[3].y = worldVertices[SP_VERTEX_Y4];
                vertices[3].z = 0;
                vertices[3].tx = regionAttachment->uvs[SP_VERTEX_X4];
                vertices[3].ty = regionAttachment->uvs[SP_VERTEX_Y4];
                
                m_data.indexes.clear();
                m_data.indexes.push_back(0);
                m_data.indexes.push_back(1);
                m_data.indexes.push_back(2);
                m_data.indexes.push_back(0);
                m_data.indexes.push_back(2);
                m_data.indexes.push_back(3);
                
                gr.DrawGeometry(m_data,true);
            }
        }
        gr.SetTransform(tr);
    }
}