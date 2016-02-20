#include "sb_skelet_controller.h"
#include "sb_skelet_data.h"
#include "sb_skelet_object.h"

SB_META_DECLARE_OBJECT(Sandbox::SkeletController, Sandbox::Thread)
namespace Sandbox {
    
    SkeletController::SkeletController(const SkeletonDataPtr& data ) : m_data(data),m_animation(0) {
        m_crnt_frame = 0;
        m_frame_time = 0;
        m_loop = 0;
    }
    
    
    bool SkeletController::Update(float dt) {
        if (m_animation) {
            bool stop = false;
            m_frame_time += dt;
            size_t frames_drop = m_frame_time * m_animation->GetFPS();
            m_crnt_frame += frames_drop;
            m_frame_time -= float(frames_drop) / m_animation->GetFPS();
            if (m_crnt_frame >= m_animation->GetFrames()){
                if (m_loop != 1) {
                    m_crnt_frame = m_crnt_frame % m_animation->GetFrames();
                    if (m_loop > 0) {
                        m_loop--;
                    }
                } else {
                    m_crnt_frame = m_animation->GetFrames() - 1;
                    ApplyFrame();
                    stop = true;
                    if (m_end_event) {
                        m_end_event->Emmit();
                    }
                    m_animation = 0;
                    if (!m_added_animations.empty()) {
                        StartAnimation(m_added_animations.front().name.c_str(), m_added_animations.front().loop);
                        m_added_animations.pop_front();
                        stop = false;
                    }
                }
            }
            ApplyFrame();
        }
        return false;
    }
    
    void SkeletController::AddAnimation(const char *name, int loop) {
        if (!m_animation) {
            StartAnimation(name, loop);
        } else {
            AnimationEntry e = { name , loop };
            m_added_animations.push_back(e);
            if (m_loop <=0 ) {
                m_loop = 1;
            }
        }
        
    }
    
    void SkeletController::StartAnimation(const char* name,int loop) {
        m_animation = m_data->GetAnimation(name).get();
        m_crnt_frame = 0;
        m_frame_time = 0;
        m_loop = loop;
        ApplyFrame();
    }
    
    void SkeletController::ChangeData( const SkeletonDataPtr& data ) {
        sb_assert(data);
        if (m_data) {
            if (m_animation) {
                m_animation = data->GetAnimation( m_animation->GetName() ).get();
            }
        }
        m_data = data;
        m_crnt_frame = 0;
        m_frame_time = 0;
        ApplyFrame();
    }
    
    bool SkeletController::HasAnimation(const char* name) const {
        if (!m_data) return false;
        return  m_data->HasAnimation(name);
    }
    
    void SkeletController::AddObject(const SkeletObjectPtr& obj) {
        m_objects.push_back(obj);
    }
    
    void SkeletController::SetFrame(size_t frame) {
        if (!m_animation)
            return;
        m_crnt_frame = frame % m_animation->GetFrames();
        ApplyFrame();
    }
    
    void SkeletController::SetTime( float time ) {
        if (!m_animation)
            return;
        size_t frames = time * m_animation->GetFPS();
        m_crnt_frame = frames % m_animation->GetFrames();
        m_frame_time = time - frames / m_animation->GetFPS();
    }
    
    void SkeletController::ApplyFrame() {
        if (!m_animation)
            return;
        for (sb::vector<SkeletObjectPtr>::const_iterator it = m_objects.begin();it!=m_objects.end();++it ) {
            for (size_t i = 0; i < m_data->GetNodesCount();++i ) {
                const SkeletonNodeFrame& frame = m_animation->GetNodeFrame(m_crnt_frame, i);
                (*it)->SetNode(i, frame.transform, frame.alpha, m_data->GetImage(frame.image));
            }
        }
    }
}