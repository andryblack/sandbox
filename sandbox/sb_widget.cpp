//
//  sb_widget.cpp
//  YinYang
//
//  Created by Андрей Куницын on 2/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_widget.h"
#include "sb_touch_info.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::Widget, Sandbox::Container)
SB_META_DECLARE_OBJECT(Sandbox::TouchButtonWidget, Sandbox::Widget)

namespace Sandbox {
    
    Widget::Widget() {
        m_inside = false;
        m_active = true;
    }
    
    void Widget::Draw(Graphics& g) const {
        Transform2d tr = g.GetTransform();
        g.SetTransform(tr.translated(m_rect.GetTopLeft()));
        Container::Draw(g);
        g.SetTransform(tr);
    }
    
    bool Widget::HandleTouch( const TouchInfo& touch ) {
        if (!m_active)
            return false;
        if ( m_rect.PointInside( touch.GetPosition() ) ) {
            if (!m_inside) {
                OnTouchEnter();
                m_inside = true;
            } else if ( touch.GetType()==TouchInfo::END ) {
                OnTouchLeave();
                m_inside = false;
            }
            TouchInfo touchNew( touch.GetType(), touch.GetPosition()-m_rect.GetTopLeft() );
            if (!HandleTouchInside( touchNew )) 
                HandleChilds(touchNew,false);
            return true;
        }
        if (m_inside) {
            OnTouchLeave();
            m_inside = false;
        }
        return false;
    }
    
    
    void TouchButtonWidget::OnTouchEnter() {
        if (m_activate_event)
            m_activate_event->Emmit();
    }
    void TouchButtonWidget::OnTouchLeave() {
        if (m_deactivate_event)
            m_deactivate_event->Emmit();
    }
    bool TouchButtonWidget::HandleTouchInside(const TouchInfo& touch) {
        if (touch.GetType()==TouchInfo::END) {
            if (m_touch_event)
                m_touch_event->Emmit();
        }
        return true;
    }
}
