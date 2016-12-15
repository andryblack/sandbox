#ifndef _SB_MYGUI_SCROLL_LIST_H_INCLUDED_
#define _SB_MYGUI_SCROLL_LIST_H_INCLUDED_

#include "MyGUI_ItemBox.h"
#include "meta/sb_meta.h"
#include "sb_vector2.h"
#include "sb_mygui_scroll_area.h"

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
            virtual void createWidget(MyGUI::Widget* w) = 0;
            virtual void updateWidget(MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di,bool changed) = 0;
            virtual void onItemClick(size_t idx) = 0;
            virtual void onSelectionChanged(size_t idx) {};
            virtual void onBeginScroll() = 0;
            virtual void onEndScroll() = 0;
            virtual void onFreeScroll() = 0;
        };
        typedef sb::shared_ptr<ScrollListDelegate> ScrollListDelegatePtr;
        
        class ScrollList : public ScrollArea {
            MYGUI_RTTI_DERIVED( ScrollList )
        public:
            ScrollList();
            ~ScrollList();
            
            void setDelegate(const ScrollListDelegatePtr& delegate);
            void updateData();
            
            void setVisibleCount(size_t count);
            size_t getVisibleCount() const { return m_visible_count; }
            
            //! Get index of selected item (ITEM_NONE if none selected)
            size_t getIndexSelected() const { return m_selected_index; }
            void setIndexSelected(size_t _index);
            void clearIndexSelected();

            size_t getIndexTop() const { return m_top_index; }
            void setIndexTop(size_t idx);
            
            void setItemSize(int size);
            int getItemSize() const;
            
            void setSubItems(size_t count);
            size_t getSubItems() const { return m_num_subitems; }
            
            MyGUI::Widget* getSelectionWidget() const { return m_selection_widget; }
            
            void setScrollBounds(int b);
            bool getCentered() const { return m_centered; }
            void setCentered(bool c);
            
            void moveNext();
            void movePrev();
            void moveToPage(int idx);
            
            void setPage(int page);
            int getPage() const;
            
            void    setScroll(int pos);
            int     getScroll() const;
            
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
            
            virtual void setContentPosition(const MyGUI::IntPoint& pos);
            
            virtual void updateView();
            void updateContent();
            
            void updateWidgets();
            
            virtual void OnScrollBegin();
            virtual void OnScrollMove();
            virtual void OnScrollEnd();
            
            virtual Vector2f Normalize(const Vector2f& v,bool soft) const;
            void updateSelectionWidget(MyGUI::Widget* w);
            
        private:
            ScrollListDelegatePtr   m_delegate;
            int     m_item_size;
            size_t  m_num_subitems;
            size_t m_visible_count;
            
            virtual void setPropertyOverride(const std::string& _key, const std::string& _value);
            
           
            MyGUI::Widget* createWidgetItem();
            void drawItem(MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di);
            void handleItemClick(MyGUI::Widget* _sender);
            
            
            virtual void onMouseDrag(int _left, int _top, MyGUI::MouseButton _id);
            virtual void onMouseButtonPressed(int _left, int _top, MyGUI::MouseButton _id);
            virtual void onMouseButtonReleased(int _left, int _top, MyGUI::MouseButton _id);
            
            
            
            int     getScrollAreaSize() const;
            int     getScrollContentSize() const;
            int getScrollMargin() const;
            
            
            size_t  m_selected_index;
            MyGUI::Widget* m_selection_widget;
            MyGUI::IntPoint m_selection_offset;
            size_t  m_top_index;
            
            bool   m_centered;
            bool   m_vertical;
            MyGUI::IntRect m_content_margins;
            
            MyGUI::VectorWidgetPtr m_items;
            MyGUI::IntSize   m_item_widget_size;
            
            void resetWidgets();
            
            bool m_align_on_cell;
        };
        
        
    }
    
}

#endif /* _SB_MYGUI_SCROLL_LIST_H_INCLUDED_ */
