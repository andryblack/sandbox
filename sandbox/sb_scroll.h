#ifndef SB_SCROLL_H_INCLUDED
#define SB_SCROLL_H_INCLUDED

#include "sb_vector.h"
#include "sb_rect.h"
#include "sb_point.h"
#include "sb_time.h"

namespace Sandbox {
    
    class Scroll {
    private:
        bool m_v_enabled;
        bool m_h_enabled;
        Sizef   m_content_size;
        Sizef   m_view_size;
        Vector2f    m_offset;
        Vector2f   m_prev_pos;
        Time    m_last_time;
        Vector2f    m_last_speed;
        enum ScrollState {
            scroll_none,
            scroll_wait,
            scroll_move,
            scroll_free
        } m_state;
        Sizef   m_bounds;
    protected:
        virtual Vector2f Normalize(const Vector2f& offset,bool soft);
        Vector2f Limit(const Vector2f& v) const;
        void Move(const Vector2f& offset);
        
        virtual void OnScrollBegin() {}
        virtual void OnScrollEnd() {}
        virtual void OnScrollMove() {}
    public:
        Scroll();
        
        bool IsMove() const { return m_state == scroll_move || m_state == scroll_wait;  }
        bool IsActive() const { return m_state != scroll_none; }
        
        const Sizef& GetContentSize() const { return m_content_size; }
        void SetContentSize(const Sizef& r) { m_content_size = r; }
        const Sizef& GetViewSize() const { return m_view_size; }
        void SetViewSize(const Sizef& r);
        void SetViewPos(const Vector2f& v) { m_offset = Normalize(v,false); }
        const Vector2f& GetViewPos() const { return m_offset; }
        
        void ScrollBegin( const Vector2f& pos );
        void ScrollMove( const Vector2f& pos );
        void ScrollEnd( const Vector2f& pos );
        
        void Reset();
        
        void Update(float dt);
        
    };
    
}

#endif /*SB_SCROLL_H_INCLUDED*/
