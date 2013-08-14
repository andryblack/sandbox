//
//  sb_gui_widget.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 7/31/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_gui_widget__
#define __sr_osx__sb_gui_widget__

#include "meta/sb_meta.h"
#include "sb_rect.h"
#include <sbstd/sb_string.h>
#include <sbstd/sb_vector.h>

namespace Sandbox {
	
	class Graphics;
	  
    namespace Gui {
    
        class Widget : public meta::object {
            SB_META_OBJECT
        public:
            Widget();
            virtual ~Widget();
            
            typedef sb::shared_ptr<Widget> Ptr;
            
            
            bool GetVisible() const { return m_visible; }
            void SetVisible(bool v) { m_visible = v; }
            bool GetEnabled() const { return m_enabled; }
            void SetEnabled(bool e) { m_enabled = e; }
            const Rectf& GetRect() const { return m_rect; }
            void SetRect(const Rectf& r) { m_rect = r; }
            Vector2f GetPos() const { return m_rect.GetTopLeft(); }
            void SetPos(const Vector2f& p) { m_rect.SetPos(p); }
            Vector2f GetSize() const { return m_rect.GetSize(); }
            void SetSize(const Vector2f& s) { return m_rect.SetSize(s); }
            const sb::string& GetText() const { return m_text; }
            void SetText(const sb::string& t) { m_text = t; }
        
            void DoRender( Graphics& g ) const;
            
            void AddChild(const Ptr& w);
            void RemoveChild(const Ptr& w);
        protected:
            virtual void Render( Graphics& g ) const;
        private:
            Widget*     m_parent;
            Rectf       m_rect;
            sb::string  m_text;
            typedef sb::vector<Ptr> WidgetsList;
            WidgetsList m_childs;
            bool        m_visible:1;
            bool        m_enabled:1;
        };
        
    }
    
}

#endif /* defined(__sr_osx__sb_gui_widget__) */
