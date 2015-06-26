#include "sb_skelet_controller.h"
#include "sb_skelet_data.h"
#include "sb_skelet_object.h"

SB_META_DECLARE_OBJECT(Sandbox::SkeletController, Sandbox::Thread)
namespace Sandbox {
    
    SkeletController::SkeletController(const SkeletonDataPtr& data ) : m_data(data),m_animation(0) {
        m_crnt_frame = 0;
        m_frame_time = 0;
        m_loop = false;
    }
    
    
    bool SkeletController::Update(float dt) {
        if (m_animation) {
            bool stop = false;
            m_frame_time += dt;
            size_t frames_drop = m_frame_time * m_animation->GetFPS();
            m_crnt_frame += frames_drop;
            m_frame_time -= float(frames_drop) / m_animation->GetFPS();
            if (m_crnt_frame >= m_animation->GetFrames()){
                if (m_loop) {
                    m_crnt_frame = m_crnt_frame % m_animation->GetFrames();
                } else {
                    m_crnt_frame = m_animation->GetFrames() - 1;
                    stop = true;
                }
            }
            ApplyFrame();
        }
        return false;
    }
    
    void SkeletController::StartAnimation(const char* name,bool loop) {
        m_animation = m_data->GetAnimation(name).get();
        m_crnt_frame = 0;
        m_frame_time = 0;
        m_loop = loop;
    }
    
    void SkeletController::AddObject(const SkeletObjectPtr& obj) {
        m_objects.push_back(obj);
    }
    
    void SkeletController::ApplyFrame() {
        for (sb::vector<SkeletObjectPtr>::const_iterator it = m_objects.begin();it!=m_objects.end();++it ) {
            for (size_t i = 0; i < m_data->GetNodesCount();++i ) {
                const SkeletonNodeFrame& frame = m_animation->GetNodeFrame(m_crnt_frame, i);
                (*it)->SetNode(i, frame.transform, frame.alpha, m_data->GetImage(frame.image));
            }
        }
    }
}