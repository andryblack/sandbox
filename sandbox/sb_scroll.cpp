#include "sb_scroll.h"

//SB_META_DECLARE_KLASS(Sandbox::Scroll,void)

namespace Sandbox {
    
    Scroll::Scroll() : m_v_enabled(true) , m_h_enabled(true),m_state(scroll_none) {
        m_bounds = Sizef(0,0);
        m_fade = 0.9f;
        m_normalization_time = 1.0f / 8.0f;
    }
    
    void Scroll::SetViewSize(const Sizef& r) {
        m_view_size = r;
    }
    
    void Scroll::ScrollBegin( const Vector2f& pos ) {
        if (m_state == scroll_free) {
            OnScrollEnd();
        }
        m_prev_pos = pos;
        m_last_time = Time::Now();
        m_last_speed = Sandbox::Vector2f(0,0);
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
        Sandbox::Vector2f prev_pos = GetOffset();
        Move(Limit(delta),false);
        
        Time  now = Time::Now();
        float time = (now-m_last_time).ToFloat();
        
        if (delta.length() > 2.0f && (time > (1.0f / 120.0f)) ) {
            m_last_speed = (GetOffset() - prev_pos) / time;
            //LogDebug() << "set speed: " << m_last_speed.x << "," << m_last_speed.y << " with time: " << time;
        } else if (time > 1.0f/10.0f) {
            m_last_speed = Sandbox::Vector2f();
        }
        if (m_last_speed.length() < 10.0f) {
            m_last_speed = Sandbox::Vector2f();
        }
        m_last_time = now;
        m_prev_pos = pos;
        OnScrollMove();
    }
    void Scroll::ScrollEnd( const Vector2f& pos ) {
        if (m_state == scroll_move) {
            ScrollMove(pos);
            m_state = scroll_free;
        } else if (m_state == scroll_wait) {
            m_state = scroll_free;
        } else {
            m_last_speed = Sandbox::Vector2f(0,0);
            m_state = scroll_none;
        }
        
    }
    
    void Scroll::Move(const Vector2f& delta,bool fire) {
        SetOffset(Normalize(GetOffset() + delta,true));
        if (fire)
            OnScrollMove();
    }
    
    static inline float limit(float v,float l,float r) {
        if (v < l) return l;
        if (v > r) return r;
        return v;
    }
    
    Vector2f Scroll::Normalize(const Vector2f& offset,bool soft) const {
        Vector2f res = offset;
        if (!m_v_enabled) {
            res.y = 0;
        } else if (m_content_size.h <= m_view_size.h) {
            if (soft && m_bounds.h > 0.0f) {
                res.y = limit(res.y,-m_bounds.h,m_bounds.h);
            } else {
                res.y = 0;
            }
        } else {
            if (soft && m_bounds.h > 0.0f) {
                res.y = limit(res.y,-m_bounds.h,m_content_size.h - m_view_size.h + m_bounds.h);
            } else {
                res.y = limit(res.y,0,m_content_size.h - m_view_size.h);
            }
        }
        if (!m_h_enabled) {
            res.x = 0;
        } else if(m_content_size.w <= m_view_size.w) {
            if (soft && m_bounds.w > 0.0f) {
                res.x = limit(res.x,-m_bounds.w,m_bounds.w);
            } else {
                res.x = 0;
            }
        } else {
            if (soft && m_bounds.w > 0.0f) {
                res.x = limit(res.x,-m_bounds.w,m_content_size.w - m_view_size.w + m_bounds.w);
            } else {
                res.x = limit(res.x,0,m_content_size.w - m_view_size.w);
            }
        }
        return res;
    }
    
    static inline float soft_limit(float v,float r,float w,float k) {
        if (w > 0.0f) {
            if (r < 0.0f)
                r = 0.0f;
            if (v < 0.0f) {
                return k * (1.0f - (-v / w));
            } else if (v > r) {
                return k * (1.0f - ( v - r ) / w);
            }
        }
        return k;
    }
    
    Vector2f Scroll::Limit(const Vector2f& v) const {
        Sandbox::Vector2f offset = GetOffset();
        return Vector2f(
                        soft_limit(offset.x, m_content_size.w-m_view_size.w, m_bounds.w, v.x),
                        soft_limit(offset.y, m_content_size.h-m_view_size.h, m_bounds.h, v.y));
    }
    
    void Scroll::ScrollTo( const Vector2f& targer ) {
        m_state = scroll_none;
        m_scroll_target = Normalize(targer,false);
        m_state = scroll_target;
    }
    
    Sandbox::Vector2f Scroll::GetTarget() const {
        if (m_state == scroll_target)
            return m_scroll_target;
        return Normalize(GetOffset(), false);
    }
    
    void Scroll::Update(float dt) {
        if (m_state == scroll_free || m_state == scroll_target) {
            Sandbox::Vector2f offset = GetOffset();
            Vector2f nmove = (GetTarget()-offset);
            float nlen = nmove.length();
            float len = m_last_speed.length() + nlen;
            if (len < 5.0f && nlen < 0.5f) {
                if (nlen != 0.0f) {
                    Move(nmove,true);
                }
                m_last_speed = Vector2f(0,0);
                m_state = scroll_none;
                OnScrollEnd();
                return;
            }
            Vector2f prev_pos = offset;
            Vector2f delta = Limit(m_last_speed * dt * m_fade);
            Move(delta,false);
            m_last_speed = (GetOffset() - prev_pos) / dt;
            Vector2f delta_n = nmove * dt / m_normalization_time;
            if (delta_n.length() > delta.length())
                Move(delta_n,false);
            OnScrollMove();
        }
    }
    
    

    void Scroll::Reset() {
        m_state = scroll_none;
        m_last_speed = Sandbox::Vector2f(0,0);
    }
    
}
