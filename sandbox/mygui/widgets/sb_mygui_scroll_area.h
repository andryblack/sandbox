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
            void setScrollPosF(const Sandbox::Vector2f& p);
            
            EventHandle_ScrollAreaPtrIntPoint scrollBegin;
            EventHandle_ScrollAreaPtrIntPoint scrollComplete;
            virtual MyGUI::ILayerItem* getLayerItemByPoint(int _left, int _top) const;
            
            bool scrollActive() const { return Scroll::IsActive(); }
            void scrollToWidget(MyGUI::Widget* w);
            bool isWidgetFullVisible(MyGUI::Widget* w);
            
            void setSmallScrollEnabled(bool enabled);
            bool getSmallScrollEnabled() const { return m_small_scroll_enabled; }
            void cancelScroll();
        protected:
            virtual void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            void initialiseOverride();
            void shutdownOverride();
            
            virtual void updateView();
            
            void frameEntered(float dt);
            virtual void updateScrollBars();
            virtual void notifyScrollChangePosition(MyGUI::ScrollBar* _sender, size_t _position);
            
            virtual Vector2f GetOffset() const;
            virtual void SetOffset(const Vector2f& offset);
            
            virtual void OnScrollBegin();
            virtual void OnScrollEnd();
            
            virtual void update(float dt);
        protected:
            void updateRealSize(int w,int h);
            virtual void updateScrollEnable();
        private:
            
            void handleGlobalMouseMove(float x,float y);
            void handleGlobalMousePressed(float x,float y, MyGUI::MouseButton _id);
            void handleGlobalMouseReleased(float x,float y, MyGUI::MouseButton _id);
            
            Sandbox::Vector2f   m_real_offset;
            bool    m_manual_scroll;
            bool    m_small_scroll_enabled;
        };
        
        
    }
    
}

#endif /* defined(__Sandbox__sb_mygui_scroll_area__) */
