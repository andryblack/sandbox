//
//  sb_mygui_scroll_list.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 20/01/14.
//
//

#ifndef __Sandbox__sb_mygui_scroll_list__
#define __Sandbox__sb_mygui_scroll_list__

#include "MyGUI_ItemBox.h"
#include "meta/sb_meta.h"
#include "sb_vector2.h"
#include "MyGUI_Timer.h"

namespace Sandbox {
    
    namespace mygui {
        
        class ScrollList;
        
        class ScrollListDelegate : public meta::object {
            SB_META_OBJECT
        protected:
            ScrollList* m_list;
        public:
            ScrollListDelegate() : m_list(0) {}
            virtual void setScrollList(ScrollList* sl);
            virtual size_t getItemsCount() = 0;
            virtual MyGUI::Any getItemData(size_t idx) = 0;
            virtual void createWidget(MyGUI::Widget* w) = 0;
            virtual void updateWidget(MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di) = 0;
        };
        typedef sb::shared_ptr<ScrollListDelegate> ScrollListDelegatePtr;
        
        class ScrollList : public MyGUI::ItemBox {
            MYGUI_RTTI_DERIVED( ScrollList )
        public:
            ScrollList();
            ~ScrollList();
            
            void setDelegate(const ScrollListDelegatePtr& delegate);
            
            void setVisibleCount(size_t count);
            size_t getVisibleCount() const { return m_visible_count; }
            
            void setScrollBounds(int b);
            
            void moveNext();
            void movePrev();
            
            virtual MyGUI::ILayerItem* getLayerItemByPoint(int _left, int _top) const;
        protected:
            void initialiseOverride();
            void shutdownOverride();
            
            void frameEntered(float dt);
        private:
            ScrollListDelegatePtr   m_delegate;
            size_t  m_visible_count;
            
            virtual void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            void handleCreateWidgetItem(MyGUI::ItemBox*, MyGUI::Widget* w);
            void handleCoordItem(MyGUI::ItemBox*, MyGUI::IntCoord& coords, bool drag);
            void handleDrawItem(MyGUI::ItemBox*, MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di);
            
            void handleGlobalMouseMove(int x,int y);
            void handleGlobalMousePressed(int x,int y, MyGUI::MouseButton _id);
            void handleGlobalMouseReleased(int x,int y, MyGUI::MouseButton _id);
            
            virtual void onMouseDrag(int _left, int _top, MyGUI::MouseButton _id);
            virtual void onMouseButtonPressed(int _left, int _top, MyGUI::MouseButton _id);
            virtual void onMouseButtonReleased(int _left, int _top, MyGUI::MouseButton _id);
            
            float       m_move_speed;
            float       m_move_accum;
            
            int         m_scroll_target;
            
            void    setScroll(int pos);
            int     getScroll() const;
            int     getItemSize() const;
            int     getScrollAreaSize() const;
            int     getScrollContentSize() const;
            
            enum State {
                state_none,
                state_wait_scroll,
                state_manual_scroll,
                state_free_scroll
            } m_state;
            MyGUI::IntPoint m_scroll_prev_pos;
            int m_scroll_begin;
            
            MyGUI::Timer    m_scroll_timer;
            int normalizeScrollValue(int val) const;
            
            int m_border_dempth;
        };
        
        void register_widgets();
        void unregister_widgets();
    }
    
}

#endif /* defined(__Sandbox__sb_mygui_scroll_list__) */
