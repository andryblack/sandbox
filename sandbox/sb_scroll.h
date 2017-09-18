#ifndef SB_SCROLL_H_INCLUDED
#define SB_SCROLL_H_INCLUDED

#include "sb_vector2.h"
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
        Vector2f   m_prev_pos;
        Time    m_last_time;
        Vector2f    m_last_speed;
        Vector2f    m_scroll_target;
        enum ScrollState {
            scroll_none,
            scroll_wait,
            scroll_move,
            scroll_free,
            scroll_target
        } m_state;
        Sizef   m_bounds;
        float m_fade;
        float   m_normalization_time;
        float m_scroll_start_length;
    protected:
        virtual Vector2f Normalize(const Vector2f& offset,bool soft) const;
        Vector2f Limit(const Vector2f& v) const;
        void Move(const Vector2f& offset,bool fire=true);
        Vector2f GetTarget() const;
        
        virtual void OnScrollBegin() {}
        virtual void OnScrollEnd() {}
        virtual void OnScrollMove() {}
        
        virtual Vector2f GetOffset() const = 0;
        virtual void SetOffset(const Vector2f& offset) = 0;
        bool IsFreeMove() const { return m_state == scroll_free; }
        bool IsTargetMove() const { return m_state == scroll_target; }
        
        const Vector2f& GetPrevPos() const { return m_prev_pos; }
    public:
        Scroll();
        
        bool IsMove() const { return m_state == scroll_move || m_state == scroll_wait;  }
        bool IsActive() const { return m_state != scroll_none; }
        
        bool GetVEnabled() const { return m_v_enabled; }
        void SetVEnabled( bool e ) { m_v_enabled = e; }
        bool GetHEnabled() const { return m_h_enabled; }
        void SetHEnabled( bool e ) { m_h_enabled = e; }
        void SetFade(float f) { m_fade = f; }
        float GetFade() const { return m_fade; }

        const Sizef& GetContentSize() const { return m_content_size; }
        void SetContentSize(const Sizef& r) { m_content_size = r; }
        const Sizef& GetViewSize() const { return m_view_size; }
        void SetViewSize(const Sizef& r);
        const Sizef& GetBounds() const { return m_bounds; }
        void SetBounds(const Sizef& s) { m_bounds = s; }
        void SetViewPos(const Vector2f& v) { SetOffset(Normalize(v,false)); }
        Vector2f GetViewPos() const { return GetOffset(); }
        float GetNormalizationTime() const { return m_normalization_time; }
        void SetNormalizationTime(float t) { m_normalization_time = t; }
        float GetScrollStartLength() const { return m_scroll_start_length; }
        void SetScrollStartLength(float s) { m_scroll_start_length = s; }
        
        void ScrollBegin( const Vector2f& pos );
        void ScrollMove( const Vector2f& pos );
        void ScrollEnd( const Vector2f& pos );
        
        void ScrollTo( const Vector2f& targer );
        
        void Reset();
        void Cancel();
        
        void Update(float dt);
        
    };
    
}

#endif /*SB_SCROLL_H_INCLUDED*/
