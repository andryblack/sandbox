#ifndef _SB_MYGUI_SCROLL_LIST_BASE_H_INCLUDED_
#define _SB_MYGUI_SCROLL_LIST_BASE_H_INCLUDED_


#include "MyGUI_ItemBox.h"
#include "meta/sb_meta.h"
#include "sb_vector2.h"
#include "sb_mygui_scroll_area.h"

namespace Sandbox {
    
    namespace mygui {
        
        class ScrollListBase;
        
        class ScrollListDelegate : public meta::object {
            SB_META_OBJECT
        public:
            ScrollListDelegate()  {}
            virtual size_t getItemsCount() = 0;
            virtual void createWidget(MyGUI::Widget* w,size_t idx) = 0;
            virtual bool canReuseWidget(MyGUI::Widget* w,size_t idx) = 0;
            virtual void updateWidget(MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di,bool changed) = 0;
            virtual void onItemClick(size_t idx) = 0;
            virtual void onItemPressed(size_t idx) = 0;
            virtual void onSelectionChanged(size_t idx) {};
            virtual void onBeginScroll() = 0;
            virtual void onEndScroll() = 0;
            virtual void onFreeScroll() = 0;
            
        };
        typedef sb::intrusive_ptr<ScrollListDelegate> ScrollListDelegatePtr;
        
        class ScrollListBase : public ScrollArea {
            MYGUI_RTTI_DERIVED( ScrollListBase )
        public:
            ScrollListBase();
            ~ScrollListBase();
            
            void setDelegate(const ScrollListDelegatePtr& delegate);
            void updateData();
          
            //! Get index of selected item (ITEM_NONE if none selected)
            size_t getIndexSelected() const { return m_selected_index; }
            void setIndexSelected(size_t _index);
            void clearIndexSelected();
            
            size_t getIndexTop() const { return m_top_index; }
            void setIndexTop(size_t idx);
            
            MyGUI::Widget* getSelectionWidget() const { return m_selection_widget; }
            
            void setScrollBounds(int b);
            bool getCentered() const { return m_centered; }
            void setCentered(bool c);
            
            void    setScroll(float pos);
            float     getScroll() const;
            
            void itemAdded();
            
            
            void setVerticalAlignment(bool _value);
            bool getVerticalAlignment() const { return m_vertical; }
            void setContentMargins(const MyGUI::IntRect& _value);
            const MyGUI::IntRect& getContentMargins() const {
                return m_content_margins;
            }
            
            MyGUI::Widget* getWidgetByIndex(size_t _index);
            size_t getIndexByWidget(MyGUI::Widget* w);
            
            void redrawAllItems();
            void redrawItemAt(size_t idx);
            
            void setAlignOnCell(bool _value);
            bool getAlignOnCell() const { return m_align_on_cell; }
            
            MyGUI::Widget* updateWidget(size_t idx, MyGUI::VectorWidgetPtr& pool);
            void upWidget(MyGUI::Widget* w);
        protected:
            
            void initialiseOverride();
            void shutdownOverride();
            
            virtual void setPropertyOverride(const std::string& _key, const std::string& _value);
            
            
            virtual void setContentPosition(const MyGUI::IntPoint& pos);
            int     getScrollAreaSize() const;
            int getScrollMargin() const;
            
            virtual void updateView();
            
            
            void updateWidgets();
            
            virtual void OnScrollBegin();
            virtual void OnScrollMove();
            virtual void OnScrollEnd();
            
            void updateSelectionWidget(MyGUI::Widget* w);
            
            virtual void onMouseDrag(int _left, int _top, MyGUI::MouseButton _id);
            virtual void onMouseButtonPressed(int _left, int _top, MyGUI::MouseButton _id);
            virtual void onMouseButtonReleased(int _left, int _top, MyGUI::MouseButton _id);
            
            virtual void handleItemClick(MyGUI::Widget* _sender);
            virtual void handleItemPressed(MyGUI::Widget* _sender,float,float,MyGUI::MouseButton _id);

            MyGUI::Widget* createWidgetItem(size_t idx);
            void drawItem(MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di);
            
            void resetWidgets();
            
            virtual void alignWidget(MyGUI::Widget* w,size_t idx) const = 0;
            virtual void updateContent() = 0;
            virtual void getVisibleItems(int& first, int& last) const = 0;
        protected:
            ScrollListDelegatePtr   m_delegate;
            size_t  m_selected_index;
            MyGUI::Widget* m_selection_widget;
            MyGUI::IntPoint m_selection_offset;
            size_t  m_top_index;
            
            bool   m_centered;
            bool   m_vertical;
            
            
            bool m_align_on_cell;
            MyGUI::VectorWidgetPtr m_all_items;
        private:
            MyGUI::IntRect m_content_margins;
        };
    }
    
}

#endif /*_SB_MYGUI_SCROLL_LIST_BASE_H_INCLUDED_*/
