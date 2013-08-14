//
//  sb_gui_widget.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 7/31/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_gui_widget.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::Gui::Widget, Sandbox::meta::object)

namespace Sandbox {
    
    namespace Gui {
        
        Widget::Widget() :
            m_parent(0),
            m_visible(true),
            m_enabled(true) {
            
        }
        
        Widget::~Widget() {
            
        }
        
        void Widget::AddChild(const Ptr& w) {
            if (!w) return;
            Widget* parent = w->m_parent;
            if (parent==this) return;
            if (parent) {
                parent->RemoveChild(w);
            }
            w->m_parent = this;
            m_childs.push_back(w);
        }
        
        void Widget::RemoveChild(const Ptr& w) {
            for (WidgetsList::iterator it = m_childs.begin();it!=m_childs.end();++it) {
                if (w==*it) {
                    m_childs.erase(it);
                    break;
                }
            }
        }
        
        void Widget::DoRender( Graphics& g ) const {
            if (!GetVisible()) return;
            Transform2d tr = g.GetTransform();
            g.SetTransform(tr.translated(GetPos()));
            Render(g);
            g.SetTransform(tr);
        }
        
        void Widget::Render(Graphics &g) const {
            for (WidgetsList::const_iterator it = m_childs.begin();it!=m_childs.end();++it) {
                (*it)->DoRender(g);
            }
        }
    }
    
}
