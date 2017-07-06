//
//  sb_mygui_scroll_list.cpp
//  Sandbox
//
//  Created by Andrey Kunitsyn on 20/01/14.
//
//

#include "sb_mygui_scroll_list.h"
#include "luabind/sb_luabind.h"
#include "sb_lua_context.h"
#include "MyGUI_Gui.h"
#include "MyGUI_ILayer.h"
#include "MyGUI_InputManager.h"
#include "MyGUI_ScrollBar.h"

#include "sb_log.h"


SB_META_DECLARE_OBJECT(Sandbox::mygui::ScrollList, Sandbox::mygui::ScrollListBase)
SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ScrollList)
SB_META_METHOD(moveNext)
SB_META_METHOD(movePrev)
SB_META_METHOD(moveToPage)
SB_META_METHOD(updateData)
SB_META_METHOD(itemAdded)
SB_META_METHOD(clearIndexSelected)
SB_META_METHOD(redrawAllItems)
SB_META_METHOD(redrawItemAt)
SB_META_METHOD(getWidgetByIndex)
SB_META_METHOD(getIndexByWidget)
SB_META_METHOD(upWidget)
SB_META_METHOD(getPageScroll)
SB_META_PROPERTY_RW(page, getPage, setPage)
SB_META_PROPERTY_RW(manualScroll,manualScroll,setManualScroll)
SB_META_PROPERTY_RW(itemSize, getItemSize, setItemSize)
SB_META_PROPERTY_RW(indexSelected, getIndexSelected, setIndexSelected)
SB_META_PROPERTY_RW(scroll, getScroll, setScroll)
SB_META_PROPERTY_RW(contentMargins, getContentMargins, setContentMargins)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(ScrollList)
        
        void register_ScrollList(lua_State* L) {
            luabind::ExternClass<ScrollList>(L);
        }
        
                
        
        ScrollList::ScrollList() {
            m_item_size = 0;
            m_visible_count = 0;
            m_num_subitems = 1;
        }
        
        ScrollList::~ScrollList() {
        }
        
        
        void ScrollList::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "VisibleCount")
                setVisibleCount(MyGUI::utility::parseValue<size_t>(_value));
            else if (_key == "ItemSize")
                setItemSize(MyGUI::utility::parseValue<int>(_value));
            else if (_key == "SubItemsCount")
                setSubItems(MyGUI::utility::parseValue<size_t>(_value));
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        void ScrollList::setVisibleCount(size_t count) {
            if (m_visible_count == count)
                return;
            m_visible_count = count;
            updateView();
        }
        
        void ScrollList::setItemSize(int size) {
            m_item_size = size;
            if (size > 0) {
                m_visible_count = 0;
            }
            updateView();
        }
        
        void ScrollList::setSubItems(size_t count) {
            if (count<1)
                count = 1;
            if (m_num_subitems == count)
                return;
            m_num_subitems = count;
            updateView();
        }
                        
        void ScrollList::alignWidget(MyGUI::Widget* w,size_t idx) const {
            int l = (idx/m_num_subitems);
            int x = (idx%m_num_subitems);
            
            int left, top;
            if (getVerticalAlignment()) {
                left = getContentMargins().left+x*m_item_widget_size.width;
                top = getContentMargins().top+l*m_item_size+getCenteredOffset();
            } else {
                left = getContentMargins().left+l*m_item_size+getCenteredOffset();
                top = getContentMargins().top+x*m_item_widget_size.height;
            }
            w->setPosition( left, top );
            w->setSize(m_item_widget_size);
        }
                
        int     ScrollList::getItemSize() const {
            return m_item_size;
        }
        
        int     ScrollList::getScrollContentSize() const {
            if (getVerticalAlignment())
                return getContentSize().height;
            return getContentSize().width;
        }
        
        
        
        
        
        void ScrollList::moveNext() {
            setPage(getPage()+1);
        }
        
        void ScrollList::moveToPage(int idx) {
            setPage(idx);
        }
                
        void ScrollList::setPage(int page) {
            setScroll(getItemSize()*(page/int(m_num_subitems)) - getScrollMargin());
        }
        
        int ScrollList::getPageScroll(int page) {
            int scroll = getItemSize()*(page/int(m_num_subitems)) - getScrollMargin();
            Vector2f idiff(0,0);
            if (getVerticalAlignment()) {
                idiff.y = scroll;
            } else {
                idiff.x = scroll;
            }
            Vector2f pos = ScrollArea::Normalize(idiff,false);
            return getVerticalAlignment() ? pos.y : pos.x;
        }
        
        int ScrollList::getPage() const {
            return ((getScroll()- getScrollMargin()) + getItemSize() / 2) / getItemSize();
        }
        
        void ScrollList::movePrev() {
            setPage(getPage()-1);
        }
        
        
        
                
        void ScrollList::updateContent() {
            if (m_visible_count) {
                if (getVerticalAlignment()) {
                    m_item_size =
                    (getViewSize().height - getContentMargins().top - getContentMargins().bottom) / m_visible_count;
                } else {
                    m_item_size = (getViewSize().width - getContentMargins().left - getContentMargins().right) / m_visible_count;
                }
            }
            
            if (m_delegate) {
                size_t count = m_delegate->getItemsCount();
                size_t full_lines = (count + m_num_subitems -1)/m_num_subitems;
                
                
                int client_width = getViewSize().width;
                int client_height = getViewSize().height;
                
                if (m_vertical) {
                    m_item_widget_size.width = (client_width-getContentMargins().left-getContentMargins().right) / m_num_subitems;
                    m_item_widget_size.height = m_item_size;
                    
                    if( m_centered ) {
                        int required_height = getContentMargins().top+getContentMargins().bottom + count * m_item_size;
                        int client_height = getViewSize().height;
                        if( required_height < client_height ) {
                            setCenteredOffset((client_height - required_height)/2);
                        } else {
                            setCenteredOffset(0);
                        }
                    }
                    updateRealSize(getViewSize().width,
                                   getCenteredOffset()+ full_lines*m_item_size +
                                   getContentMargins().top +
                                   getContentMargins().bottom);
                } else {
                    m_item_widget_size.width = m_item_size;
                    m_item_widget_size.height = (client_height-getContentMargins().top-getContentMargins().bottom) / m_num_subitems;
                    
                    if( m_centered ) {
                        int required_width = getContentMargins().left+getContentMargins().right + count * m_item_size;
                        int client_width = getViewSize().width;
                        if( required_width < client_width ) {
                            setCenteredOffset((client_width - required_width)/2);
                        } else {
                            setCenteredOffset(0);
                        }
                    }
                    updateRealSize(getCenteredOffset() + full_lines*m_item_size +
                                  getContentMargins().left +
                                  getContentMargins().top,getViewSize().height);
                }
            }
        }
        
        void ScrollList::getVisibleItems(int& first, int& last) const {
            int count = m_delegate->getItemsCount();
            int lines = (count + m_num_subitems - 1) / m_num_subitems;
            int item_size = getItemSize();
            first = (getScroll() - item_size) / item_size;
            last = (getScroll() + getScrollAreaSize() + item_size) / item_size;
            if (first >= lines) {
                first = lines - 1;
            }
            if (first < 0) {
                first = 0;
            }
            if (last >= lines) {
                last = lines-1;
            }
            if (last < 0) {
                last = 0;
            }
            first*=m_num_subitems;
            last*=m_num_subitems;
            last+=m_num_subitems-1;
        }

                
        Vector2f ScrollList::Normalize(const Vector2f& v,bool soft) const {
            Vector2f res = Scroll::Normalize(v, soft);
            if (!soft && m_item_size > 0 && m_align_on_cell) {
                if (getVerticalAlignment()) {
                    float max = Scroll::GetContentSize().h - Scroll::GetViewSize().h;
                    int last_delta = 0;
                    if (max > m_item_size) {
                        last_delta = m_item_size / 4;
                    }
                    if (res.y >= ( max - last_delta)) {
                        res.y = max;
                    } else {
                        res.y = getContentMargins().top + m_item_size * int((res.y - getContentMargins().top + m_item_size/2) / m_item_size);
                    }
                } else {
                    float max = Scroll::GetContentSize().w - Scroll::GetViewSize().w;
                    int last_delta = 0;
                    if (max > m_item_size) {
                        last_delta = m_item_size / 4;
                    }
                    if (res.x >= ( max - last_delta)) {
                        res.x = max;
                    } else {
                        res.x = getContentMargins().left + m_item_size * int((res.x - getContentMargins().left + m_item_size/2) / m_item_size);
                    }
                }
            }
            return res;
        }
        
                
                
        
    }
}
