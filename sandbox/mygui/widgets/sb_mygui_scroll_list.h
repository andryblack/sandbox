#ifndef _SB_MYGUI_SCROLL_LIST_H_INCLUDED_
#define _SB_MYGUI_SCROLL_LIST_H_INCLUDED_

#include "MyGUI_ItemBox.h"
#include "meta/sb_meta.h"
#include "sb_vector2.h"
#include "sb_mygui_scroll_list_base.h"

namespace Sandbox {
    
    namespace mygui {
        
        class ScrollList;
               
        class ScrollList : public ScrollListBase {
            MYGUI_RTTI_DERIVED( ScrollList )
        public:
            ScrollList();
            ~ScrollList();
            
            
            void setVisibleCount(size_t count);
            size_t getVisibleCount() const { return m_visible_count; }
            
            void setItemSize(int size);
            int getItemSize() const;
            
            void setSubItems(size_t count);
            size_t getSubItems() const { return m_num_subitems; }
            
           
            
            void moveNext();
            void movePrev();
            void moveToPage(int idx);
            
            void setPage(int page);
            int getPage() const;
            
            
            int getPageScroll(int page);
            
           
        protected:
        private:
            
            int     m_item_size;
            size_t  m_num_subitems;
            size_t m_visible_count;
            
            virtual void setPropertyOverride(const std::string& _key, const std::string& _value);
            virtual void alignWidget(MyGUI::Widget* w,size_t idx) const;
            virtual void updateContent();
            virtual void getVisibleItems(int& first, int& last) const;
            virtual Vector2f Normalize(const Vector2f& v,bool soft) const;
            
            
            
            int     getScrollContentSize() const;
            
            
            
            
            MyGUI::IntSize   m_item_widget_size;
            
            int  m_centered_offset;
        };
        
        
    }
    
}

#endif /* _SB_MYGUI_SCROLL_LIST_H_INCLUDED_ */
