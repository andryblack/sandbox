#include "sb_skelet_controller.h"
#include "sb_skelet_data.h"
#include "sb_skelet_object.h"

SB_META_DECLARE_OBJECT(Sandbox::SkeletController, Sandbox::Thread)
SB_META_DECLARE_OBJECT(Sandbox::InterpolatedSkeletController, Sandbox::SkeletController)
namespace Sandbox {
    
    SkeletController::SkeletController(const SkeletonDataPtr& data ) : m_data(data),m_animation(0) {
        m_crnt_frame = 0;
        m_frame_time = 0;
        m_started = false;
        m_loop = 0;
    }
    
    void SkeletController::EmmitEvents(size_t from,size_t to) {
        if (m_event_signal) {
            while (from!=to) {
                const sb::vector<EventPtr>& events = m_animation->GetFrameEvents(from);
                for ( sb::vector<EventPtr>::const_iterator it = events.begin();it!=events.end();++it) {
                    m_event_signal->Emmit(*it);
                }
                ++from;
            }
        }
    }
    
    bool SkeletController::Update(float dt) {
        if (m_animation && m_started) {
            bool stop = false;
            m_frame_time += dt;
            size_t frames_drop = size_t(m_frame_time * m_animation->GetFPS());
            size_t from_frame = m_crnt_frame;
            m_crnt_frame += frames_drop;
            m_frame_time -= float(frames_drop) / m_animation->GetFPS();
            if (m_crnt_frame >= m_animation->GetFrames()){
                if (m_loop != 1) {
                    m_crnt_frame = m_crnt_frame % m_animation->GetFrames();
                    EmmitEvents(from_frame,m_animation->GetFrames());
                    EmmitEvents(0,m_crnt_frame);
                    if (m_loop > 0) {
                        m_loop--;
                    }
                } else {
                    m_crnt_frame = m_animation->GetFrames() - 1;
                    EmmitEvents(from_frame,m_animation->GetFrames());
                    ApplyFrame();
                    stop = true;
                    if (m_end_signal) {
                        m_end_signal->Emmit();
                    }
                    m_started = false;
                    if (!m_added_animations.empty()) {
                        StartAnimation(m_added_animations.front().name.c_str(), m_added_animations.front().loop);
                        m_added_animations.pop_front();
                        stop = false;
                    }
                }
            } else {
                EmmitEvents(from_frame,m_crnt_frame);
            }
            ApplyFrame();
        }
        return false;
    }
    
    void SkeletController::AddAnimation(const char *name, int loop) {
        if (!m_animation || !m_started) {
            StartAnimation(name, loop);
        } else {
            AnimationEntry e = { name , loop };
            m_added_animations.push_back(e);
            if (m_loop <=0 ) {
                m_loop = 1;
            }
        }
        
    }
    
    void SkeletController::SetAnimation(const char *name) {
        m_animation = m_data->GetAnimation(name).get();
        m_crnt_frame = 0;
        m_frame_time = 0;
        ApplyFrame();
    }
    
    void SkeletController::StartAnimation(const char* name,int loop) {
        SetAnimation(name);
        m_loop = loop;
        m_started = true;
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
        InitObjectNodes(obj);
    }
    
    void SkeletController::InitObjectNodes(const SkeletObjectPtr& /*obj*/) {
        
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
        size_t frames = size_t(time * m_animation->GetFPS());
        m_crnt_frame = frames % m_animation->GetFrames();
        m_frame_time = time - frames / m_animation->GetFPS();
    }
    
    float SkeletController::GetCurrentAnimationLength() const {
        return m_animation ? (m_animation->GetFrames() / m_animation->GetFPS()) : 0.0f;
    }
    size_t SkeletController::GetCurrentAnimationFrames() const {
        return m_animation ? m_animation->GetFrames() : 0;
    }
    
    void SkeletController::ApplyFrame() {
        if (!m_animation)
            return;
        for (sb::vector<SkeletObjectPtr>::const_iterator it = m_objects.begin();it!=m_objects.end();++it ) {
            for (size_t i = 0; i < m_data->GetNodesCount();++i ) {
                const SkeletonNodeFrame& frame = m_animation->GetNodeFrame(m_crnt_frame, i);
                const SkeletonNodeData& node = m_data->GetNode(frame.node);
                (*it)->SetSlot(i,node,frame,m_data->GetImage(frame.image));
            }
        }
    }
    
    void SkeletController::Clear() {
        m_data.reset();
        m_animation = 0;
        m_objects.clear();
        if (m_end_signal) {
            m_end_signal->Clear();
            m_end_signal.reset();
        }
        if (m_event_signal) {
            m_event_signal->Clear();
            m_event_signal.reset();
        }
        m_started = false;
        m_added_animations.clear();
    }
    
    InterpolatedSkeletController::InterpolatedSkeletController(const SkeletonDataPtr& data ) : SkeletController(data ) {
        
    }
    static inline SkeletonNodeFrame interpolate_frame(const SkeletonNodeFrame& a, const SkeletonNodeFrame& b, float k) {
        if (a.image!=b.image || a.node != b.node)
            return a;
        float ak = 1.0f - k;
        SkeletonNodeFrame f;
        f.transform.v = a.transform.v * ak + b.transform.v * k;
        f.transform.m = a.transform.m * ak + b.transform.m * k;
        f.color = a.color * ak + b.color * k;
        return f;
    }
    void InterpolatedSkeletController::ApplyFrame() {
        if (!m_animation)
            return;
        float k = m_frame_time * m_animation->GetFPS();
        size_t next_frame = m_crnt_frame + 1;
        if (next_frame >= m_animation->GetFrames()) {
            next_frame = (m_loop!=1) ? 0 : m_crnt_frame;
        }
        
        for (sb::vector<SkeletObjectPtr>::const_iterator it = m_objects.begin();it!=m_objects.end();++it ) {
            for (size_t i = 0; i < m_data->GetNodesCount();++i ) {
                const SkeletonNodeFrame& next = m_animation->GetNodeFrame(next_frame, i);
                const SkeletonNodeFrame& frame = m_animation->GetNodeFrame(m_crnt_frame, i);
                const SkeletonNodeData& node = m_data->GetNode(frame.node);
                (*it)->SetSlot(i,node,interpolate_frame(frame,next,k),m_data->GetImage(frame.image));
            }
        }
    }
}
