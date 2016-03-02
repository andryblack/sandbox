//
//  sb_mygui_scroll_area.h
//  Sandbox
//
//
//

#ifndef __Sandbox__sb_mygui_scroll_area__
#define __Sandbox__sb_mygui_scroll_area__

#include "MyGUI_ScrollView.h"
#include "meta/sb_meta.h"
#include "sb_vector2.h"
#include "MyGUI_Timer.h"

namespace Sandbox {
    
    namespace mygui {
        
        class ScrollArea;
                
        class ScrollArea : public MyGUI::ScrollView {
            MYGUI_RTTI_DERIVED( ScrollArea )
        public:
            ScrollArea();
            ~ScrollArea();
            
            void setScrollBounds(int b);
            
            void setManualScroll(bool s);
            bool manualScroll() const { return m_manual_scroll; }
            
            void setScrollPos(const MyGUI::IntPoint& p);
          
            virtual MyGUI::ILayerItem* getLayerItemByPoint(int _left, int _top) const;
        protected:
            void initialiseOverride();
            void shutdownOverride();
            
            void frameEntered(float dt);
        private:
            virtual void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            void handleGlobalMouseMove(int x,int y);
            void handleGlobalMousePressed(int x,int y, MyGUI::MouseButton _id);
            void handleGlobalMouseReleased(int x,int y, MyGUI::MouseButton _id);
            
            MyGUI::FloatPoint       m_move_speed;
            MyGUI::FloatPoint       m_move_accum;
            
            MyGUI::IntPoint         m_scroll_target;
                        
            enum State {
                state_none,
                state_wait_scroll,
                state_manual_scroll,
                state_free_scroll
            } m_state;
            MyGUI::IntPoint m_scroll_prev_pos;
            MyGUI::IntPoint m_scroll_begin;
            
            MyGUI::Timer    m_scroll_timer;
            MyGUI::IntPoint normalizeScrollValue(const MyGUI::IntPoint& val) const;
            
            int m_border_dempth;
            bool    m_manual_scroll;
        };
        
        
    }
    
}

#endif /* defined(__Sandbox__sb_mygui_scroll_area__) */
