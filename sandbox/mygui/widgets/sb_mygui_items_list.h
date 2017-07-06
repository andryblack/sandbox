#ifndef _SB_MYGUI_ITEMS_LIST_H_INCLUDED_
#define _SB_MYGUI_ITEMS_LIST_H_INCLUDED_

#include "MyGUI_ItemBox.h"
#include "meta/sb_meta.h"
#include "sb_vector2.h"
#include "sb_mygui_scroll_list_base.h"

namespace Sandbox {
    
    namespace mygui {
        
        class ItemsList;
        
        class ItemsListDelegate : public ScrollListDelegate {
            SB_META_OBJECT
        public:
            virtual int getAllItemsSize();
            virtual int getItemSize(size_t idx) = 0;
            virtual void alignItem(size_t idx,int& offset,int& size) = 0;
            virtual void getVisibleItems(int from,int to,int& first,int& last) = 0;
        };
        typedef sb::intrusive_ptr<ItemsListDelegate> ItemsListDelegatePtr;
        
        class ItemsList : public ScrollListBase {
            MYGUI_RTTI_DERIVED( ItemsList )
        public:
            ItemsList();
            ~ItemsList();
            
            void setDelegate(const ItemsListDelegatePtr& d);
        protected:
            ItemsListDelegate* getDelegate() {
                return static_cast<ItemsListDelegate*>(m_delegate.get());
            }
            ItemsListDelegate* getDelegate() const {
                return static_cast<ItemsListDelegate*>(m_delegate.get());
            }
        private:
            virtual void alignWidget(MyGUI::Widget* w,size_t idx) const;
            virtual void updateContent();
            virtual void getVisibleItems(int& first, int& last) const;
            virtual Vector2f Normalize(const Vector2f& v,bool soft) const;
        };
        
        
    }
    
}

#endif /* _SB_MYGUI_ITEMS_LIST_H_INCLUDED_ */
