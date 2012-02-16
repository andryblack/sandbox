//
//  sb_widget.h
//  YinYang
//
//  Created by Андрей Куницын on 2/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_widget_h
#define YinYang_sb_widget_h

#include "sb_container.h"
#include "sb_rect.h"
#include "sb_event.h"

namespace Sandbox {
    
    class Widget : public Container {
    public:
        Widget();
        bool HandleTouch( const TouchInfo& touch );
        const Rectf& GetRect() const { return m_rect; }
        void SetRect( const Rectf& rect) { m_rect = rect; }
        void SetActive( bool a ) { m_active = a;}
        bool GetActive() const { return m_active; }
        void Draw(Graphics& g) const;
    protected:
        virtual void OnTouchEnter() {}
        virtual void OnTouchLeave() {}
        virtual bool HandleTouchInside(const TouchInfo& touch) { return false; }
    private:
        bool    m_active;
        Rectf   m_rect;
        bool    m_inside;
    };
    
    class TouchButtonWidget : public Widget {
    public:
        void SetActivateEvent( const EventPtr& e) { m_activate_event = e; }
        void SetDeactivateEvent( const EventPtr& e) { m_deactivate_event = e;}
        void SetTouchUpInsideEvent( const EventPtr& e) {m_touch_event = e;}
    protected:
        virtual void OnTouchEnter();
        virtual void OnTouchLeave();
        virtual bool HandleTouchInside(const TouchInfo& touch);
    private:
        EventPtr    m_activate_event;
        EventPtr    m_deactivate_event;
        EventPtr    m_touch_event;
    };
}

#endif
