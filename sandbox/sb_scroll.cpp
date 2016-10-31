#include "sb_scroll.h"

namespace Sandbox {
    
    Scroll::Scroll() : m_v_enabled(true) , m_h_enabled(true),m_state(scroll_none) {
        m_bounds = Sizef(100,100);
    }
    
    void Scroll::SetViewSize(const Sizef& r) {
        m_view_size = r;
        m_bounds = m_view_size * 0.25;
    }
    
    void Scroll::ScrollBegin( const Vector2f& pos ) {
        m_prev_pos = pos;
        m_last_time = Time::Now();
        m_state = scroll_wait;
    }
    void Scroll::ScrollMove( const Vector2f& pos ) {
        if (!IsMove())
            return;
        Vector2f delta = -( pos - m_prev_pos );
        if (m_state == scroll_wait) {
            if (delta.length() >= 2.0f){
                m_last_time = Time::Now();
                OnScrollBegin();
                m_state = scroll_move;
            } else {
                return;
            }
        }
        Sandbox::Vector2f prev_pos = m_offset;
        Move(Limit(delta));
        
        Time  now = Time::Now();
        if (delta.length() > 1.0) {
            m_last_speed = (m_offset - prev_pos) / (now-m_last_time).ToFloat();
        }
        m_last_time = now;
        m_prev_pos = pos;
    }
    void Scroll::ScrollEnd( const Vector2f& pos ) {
        if (m_state == scroll_move) {
            ScrollMove(pos);
        }
        m_state = scroll_free;
    }
    
    void Scroll::Move(const Vector2f& delta) {
        m_offset = Normalize(m_offset + delta,true);
        OnScrollMove();
    }
    
    static inline float limit(float v,float l,float r) {
        if (v < l) return l;
        if (v > r) return r;
        return v;
    }
    
    Vector2f Scroll::Normalize(const Vector2f& offset,bool soft) {
        Vector2f res = offset;
        if (!m_v_enabled || m_content_size.h <= m_view_size.h) {
            res.y = (m_content_size.h - m_view_size.h) / 2;
        } else {
            if (soft) {
                res.y = limit(res.y,-m_bounds.h,m_content_size.h - m_view_size.h + m_bounds.h);
            } else {
                res.y = limit(res.y,0,m_content_size.h - m_view_size.h);
            }
        }
        if (!m_h_enabled || m_content_size.w <= m_view_size.w) {
            res.x = (m_content_size.w - m_view_size.w) / 2;
        } else {
            if (soft) {
                res.x = limit(res.x,-m_bounds.h,m_content_size.w - m_view_size.w + m_bounds.w);
            } else {
                res.x = limit(res.x,0,m_content_size.w - m_view_size.w);
            }
        }
        return res;
    }
    
    static inline float soft_limit(float v,float r,float w,float k) {
        if (v < 0.0f) {
            return k * (1.0f - (-v / w));
        } else if (v > r) {
            return k * (1.0f - ( v - r ) / w);
        }
        return k;
    }
    
    Vector2f Scroll::Limit(const Vector2f& v) const {
        return Vector2f(
                        soft_limit(m_offset.x, m_content_size.w-m_view_size.w, m_bounds.w, v.x),
                        soft_limit(m_offset.y, m_content_size.h-m_view_size.h, m_bounds.h, v.y));
    }
    
    void Scroll::Update(float dt) {
        if (m_state == scroll_free) {
            Vector2f nmove = (Normalize(m_offset,false)-m_offset);
            float len = m_last_speed.length() + nmove.length();
            if (len < 5.0f) {
                m_last_speed = Vector2f(0,0);
                m_state = scroll_none;
                OnScrollEnd();
                return;
            }
            Vector2f prev_pos = m_offset;
            Vector2f delta = Limit(m_last_speed * dt * 0.95);
            Move(delta);
            m_last_speed = (m_offset - prev_pos) / dt;
            Move(nmove * dt * 4);
        }
        
    }

    void Scroll::Reset() {
        m_state = scroll_none;
    }
    
}
