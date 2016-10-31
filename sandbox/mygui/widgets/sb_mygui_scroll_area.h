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
#include "sb_scroll.h"

namespace Sandbox {
    
    namespace mygui {
        
        class ScrollArea;
        
        typedef MyGUI::delegates::CDelegate2<ScrollArea*, MyGUI::IntPoint> EventHandle_ScrollAreaPtrIntPoint;
        
                
        class ScrollArea : public MyGUI::ScrollView , protected Scroll {
            MYGUI_RTTI_DERIVED( ScrollArea )
        public:
            ScrollArea();
            ~ScrollArea();
            
            void setScrollBounds(int b);
            
            void setManualScroll(bool s);
            bool manualScroll() const { return m_manual_scroll; }
            
            void setScrollPos(const MyGUI::IntPoint& p);
          
            EventHandle_ScrollAreaPtrIntPoint scrollComplete;
            virtual MyGUI::ILayerItem* getLayerItemByPoint(int _left, int _top) const;
            
            bool scrollActive() const { return Scroll::IsActive(); }
        protected:
            void initialiseOverride();
            void shutdownOverride();
            
            virtual void updateView();
            
            void frameEntered(float dt);
            virtual void notifyScrollChangePosition(MyGUI::ScrollBar* _sender, size_t _position);
            
            virtual void OnScrollBegin();
            virtual void OnScrollEnd();
            virtual void OnScrollMove();
        private:
            virtual void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            void handleGlobalMouseMove(int x,int y);
            void handleGlobalMousePressed(int x,int y, MyGUI::MouseButton _id);
            void handleGlobalMouseReleased(int x,int y, MyGUI::MouseButton _id);
            
            bool    m_manual_scroll;
        };
        
        
    }
    
}

#endif /* defined(__Sandbox__sb_mygui_scroll_area__) */
