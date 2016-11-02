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

SB_META_DECLARE_OBJECT(Sandbox::mygui::ScrollListDelegate, Sandbox::meta::object)
SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ScrollListDelegate)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    namespace mygui {
        
        
        class LuaScrollListDelegate : public ScrollListDelegate {
        private:
            LuaContext m_obj;
        public:
            LuaScrollListDelegate(lua_State* L, int idx) {
                lua_pushvalue(L, idx);
                m_obj.SetObject(L);
            }
            virtual void setScrollList(ScrollList* sl) {
                ScrollListDelegate::setScrollList(sl);
            }
            virtual size_t getItemsCount() {
                if (!m_obj.Valid())
                    return 0;
                return m_obj.call_self<size_t>("getItemsCount");
            }
            LuaContextPtr getItemData(size_t idx) {
                LuaContextPtr data = m_obj.call_self<LuaContextPtr>("getItemData",idx);
                return data;
            }
            virtual void createWidget(MyGUI::Widget* w) {
                m_obj.call_self("createWidget",w);
            }
            virtual void updateWidget(MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di,bool changed) {
                LuaContextPtr data_ptr = getItemData(di.index);
                m_obj.call_self("updateWidget",w,data_ptr,di,changed);
            }
            virtual void onItemClick(size_t idx) {
                LuaContextPtr data_ptr = getItemData(idx);
                if (data_ptr && m_obj.GetValueRaw<bool>("onItemClick")) {
                    m_obj.call_self("onItemClick",data_ptr,idx);
                }
            }
            virtual void onSelectionChanged(size_t idx) {
                if (!m_obj.GetValueRaw<bool>("onSelectionChanged")) {
                    return;
                }
                LuaContextPtr data;
                if (idx != MyGUI::ITEM_NONE) {
                    data = getItemData(idx);
                }
                m_obj.call_self("onSelectionChanged",data,idx);
            }
            virtual void onBeginScroll() {
                if (m_obj.GetValue<bool>("onBeginScroll"))
                    m_obj.call_self("onBeginScroll");
            }
            virtual void onEndScroll() {
                if (m_obj.GetValue<bool>("onEndScroll"))
                    m_obj.call_self("onEndScroll");
            }
            virtual void onFreeScroll() {
                if (m_obj.GetValue<bool>("onFreeScroll"))
                    m_obj.call_self("onFreeScroll");
            }
        };
    }
}

static int setDelegateImpl(lua_State* L) {
    using namespace Sandbox::mygui;
    ScrollList* sl = Sandbox::luabind::stack<ScrollList*>::get(L, 1);
    if (!sl) return 0;
    if (lua_istable(L, 2)) {
        LuaScrollListDelegate* delegate = new LuaScrollListDelegate(L,2);
        sl->setDelegate(ScrollListDelegatePtr(delegate));
    } else {
        sl->setDelegate(Sandbox::luabind::stack<ScrollListDelegatePtr>::get(L, 2));
    }
    return 0;
}

SB_META_DECLARE_OBJECT(Sandbox::mygui::ScrollList, Sandbox::mygui::ScrollArea)
SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ScrollList)
bind( property_wo( "delegate", &setDelegateImpl ) );
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
SB_META_PROPERTY_RW(page, getPage, setPage)
SB_META_PROPERTY_RW(manualScroll,manualScroll,setManualScroll)
SB_META_PROPERTY_RO(selectionWidget, getSelectionWidget)
SB_META_PROPERTY_RW(itemSize, getItemSize, setItemSize)
SB_META_PROPERTY_RW(indexSelected, getIndexSelected, setIndexSelected)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    namespace mygui {
        
        void register_ScrollList(lua_State* L) {
            luabind::Class<ScrollListDelegate>(L);
            luabind::ExternClass<ScrollList>(L);
        }
        
        void ScrollListDelegate::setScrollList(ScrollList* sl) {
            m_list = sl;
        }
        
        
        ScrollList::ScrollList() {
            m_selection_widget = 0;
            m_centered = false;
            m_item_size = 0;
            m_visible_count = 0;
            m_num_subitems = 1;
            m_vertical = false;
            m_selected_index = MyGUI::ITEM_NONE;
            Scroll::SetVEnabled(m_vertical);
            Scroll::SetHEnabled(!m_vertical);
        }
        
        ScrollList::~ScrollList() {
        }
        
        void ScrollList::initialiseOverride() {
            Base::initialiseOverride();
            assignWidget(m_selection_widget, "Selection");
            if (m_selection_widget) {
                m_selection_widget->setVisible(false);
                m_selection_offset = m_selection_widget->getPosition();
            }
        }
        void ScrollList::shutdownOverride() {
            Base::shutdownOverride();
            if (m_delegate) {
                m_delegate->setScrollList(0);
            }
            m_delegate.reset();
        }
        
        void ScrollList::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "VerticalAlignment")
                setVerticalAlignment(MyGUI::utility::parseValue<bool>(_value));
            else if (_key == "ContentMargins")
                setContentMargins(MyGUI::utility::parseValue<MyGUI::IntRect>(_value));
            /// @wproperty{ItemBox, VisibleCount, size_t} visible cells count.
            else if (_key == "VisibleCount")
                setVisibleCount(MyGUI::utility::parseValue<size_t>(_value));
            else if (_key == "ItemSize")
                setItemSize(MyGUI::utility::parseValue<int>(_value));
            else if (_key == "SubItemsCount")
                setSubItems(MyGUI::utility::parseValue<size_t>(_value));
            /// @wproperty{ItemBox, VerticalAlignment, bool} bounds.
            else if (_key == "ScrollBounds")
                setScrollBounds(MyGUI::utility::parseValue<int>(_value));
            else if (_key == "Centered")
                setCentered(MyGUI::utility::parseValue<bool>(_value));
            else if (_key == "ManualScroll")
                setManualScroll(MyGUI::utility::parseValue<bool>(_value));
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
        
        void ScrollList::setScrollBounds(int b) {
            if (getVerticalAlignment()) {
                Scroll::SetBounds(Sizef(0,b));
            } else {
                Scroll::SetBounds(Sizef(b,0));
            }
        }
                        
        void ScrollList::setDelegate(const ScrollListDelegatePtr &delegate) {
            m_delegate = delegate;
            if (!m_delegate) return;
            m_delegate->setScrollList(this);
            resetWidgets();
            updateView();
            updateWidgets();
            _updateChilds();
        }
        
        void ScrollList::resetWidgets() {
            for (MyGUI::VectorWidgetPtr::iterator it = m_items.begin();
                 it!=m_items.end();++it) {
                (*it)->setUserData(MyGUI::Any::Null);
            }
        }
        
        void ScrollList::itemAdded() {
            if (!m_delegate) return;
            updateView();
            resetWidgets();
        }
        
        void ScrollList::updateData() {
            if (!m_delegate) return;
            resetWidgets();
            updateView();
            updateWidgets();
        }
        
        MyGUI::Widget* ScrollList::createWidgetItem() {
            MyGUI::Widget* w = mRealClient->createWidget<MyGUI::Widget>("Default",
                                                                        MyGUI::IntCoord(0, 0, m_item_widget_size.width, m_item_widget_size.height), MyGUI::Align::Default);
            if (m_delegate) {
                m_delegate->createWidget(w);
            }
            w->eventMouseButtonClick += MyGUI::newDelegate(this,&ScrollList::handleItemClick);
            return w;
        }
        
        void ScrollList::drawItem(MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di) {
            if (m_delegate && di.update) {
                size_t* old_idx = w->getUserData<size_t>(false);
                m_delegate->updateWidget(w, di,!old_idx || *old_idx!=di.index);
                if (!old_idx) {
                    w->setUserData(MyGUI::Any(di.index));
                } else {
                    *old_idx = di.index;
                }
            }
        }
        
        void ScrollList::handleItemClick(MyGUI::Widget* _sender) {
            size_t index = getIndexByWidget(_sender);
            if (index == MyGUI::ITEM_NONE)
                return;
            if (m_delegate) {
                m_delegate->onItemClick( index );
            }
            setIndexSelected( index);
        }
        
        void ScrollList::onMouseDrag(int _left, int _top, MyGUI::MouseButton _id) {
            Base::onMouseDrag(_left, _top, _id);
        }
        void ScrollList::onMouseButtonPressed(int _left, int _top, MyGUI::MouseButton _id) {
            Base::onMouseButtonPressed(_left, _top, _id);
        }
        void ScrollList::onMouseButtonReleased(int _left, int _top, MyGUI::MouseButton _id) {
            Base::onMouseButtonReleased(_left, _top, _id);
        }
        
        void ScrollList::setScroll(int pos) {
            Vector2f idiff(0,0);
            if (getVerticalAlignment()) {
                idiff.y = pos;
             } else {
                idiff.x = pos;
            }
            ScrollArea::SetOffset(ScrollArea::Normalize(idiff,false));
            updateWidgets();
        }
        
        void ScrollList::setContentPosition(const MyGUI::IntPoint& pos) {
            Base::setContentPosition(pos);
        }
        
        int     ScrollList::getScroll() const {
            if (getVerticalAlignment())
                return -getViewOffset().top;
            return -getViewOffset().left;
        }
        
        int     ScrollList::getItemSize() const {
            return m_item_size;
        }
        
        int     ScrollList::getScrollContentSize() const {
            if (getVerticalAlignment())
                return getContentSize().height;
            return getContentSize().width;
        }
        
        int ScrollList::getScrollMargin() const {
            if (getVerticalAlignment())
                return getContentMargins().top;
            return getContentMargins().left;
        }
        
        int     ScrollList::getScrollAreaSize() const {
            if (getVerticalAlignment()) {
                return Scroll::GetViewSize().h;
            }
            return Scroll::GetViewSize().w;
        }
        
        void ScrollList::moveNext() {
            setPage(getPage()+1);
        }
        
        void ScrollList::moveToPage(int idx) {
            setPage(idx);
        }
                
        void ScrollList::setPage(int page) {
            setScroll(getItemSize()*(page/m_num_subitems) - getScrollMargin());
        }
        
        int ScrollList::getPage() const {
            return ((getScroll()- getScrollMargin()) + getItemSize() / 2) / getItemSize();
        }
        
        void ScrollList::movePrev() {
            setPage(getPage()-1);
        }
        
        
        
        void ScrollList::setIndexSelected(size_t _index) {
            size_t prev = m_selected_index;
            m_selected_index = _index;
            if (prev != _index && prev != MyGUI::ITEM_NONE) {
                Widget* w = getWidgetByIndex(prev);
                if (w) {
                    MyGUI::IBDrawItemInfo di(prev,m_selected_index,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,true,false);
                    drawItem(w, di);
                }
            }
            if (_index != MyGUI::ITEM_NONE) {
                Widget* w = getWidgetByIndex(_index);
                if (w) {
                    MyGUI::IBDrawItemInfo di(_index,m_selected_index,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,true,false);
                    drawItem(w, di);
                    updateSelectionWidget(w);
                }
            } else if (m_selection_widget) {
                m_selection_widget->setVisible(false);
            }
            if (m_delegate) {
                m_delegate->onSelectionChanged(_index);
            }
        }
       
        void  ScrollList::clearIndexSelected() {
            setIndexSelected(MyGUI::ITEM_NONE);
        }
        
        void ScrollList::updateView() {
            ScrollView::updateView();
            updateContent();
            Base::updateView();
            ScrollArea::SetOffset(Normalize(GetOffset(), true));
            updateWidgets();
        }
        
        void ScrollList::updateContent() {
            if (m_visible_count) {
                if (getVerticalAlignment()) {
                    m_item_size =
                    (getViewSize().height - m_content_margins.top - m_content_margins.bottom) / m_visible_count;
                } else {
                    m_item_size = (getViewSize().width - m_content_margins.left - m_content_margins.right) / m_visible_count;
                }
            }
            
            if (m_delegate) {
                size_t count = m_delegate->getItemsCount();
                size_t full_lines = (count + m_num_subitems -1)/m_num_subitems;
                if (m_vertical) {
                    m_item_widget_size.width = (getViewSize().width-m_content_margins.left-m_content_margins.right) / m_num_subitems;
                    m_item_widget_size.height = m_item_size;
                    mRealClient->setSize(getViewSize().width,
                                  full_lines*m_item_size +
                                  m_content_margins.top +
                                  m_content_margins.bottom);
                } else {
                    m_item_widget_size.width = m_item_size;
                    m_item_widget_size.height = (getViewSize().height-m_content_margins.top-m_content_margins.bottom) / m_num_subitems;
                    mRealClient->setSize(full_lines*m_item_size +
                                  m_content_margins.left +
                                  m_content_margins.top,getViewSize().height);
                }
            }
        }

        void ScrollList::setVerticalAlignment(bool _value) {
            m_vertical = _value;
            Scroll::SetVEnabled(m_vertical);
            Scroll::SetHEnabled(!m_vertical);
            updateView();
        }
        
        void ScrollList::setContentMargins(const MyGUI::IntRect& _value) {
            m_content_margins = _value;
            updateView();
        }
        
        void ScrollList::setCentered(bool c) {
            m_centered = c;
            updateView();
        }
        
        MyGUI::Widget* ScrollList::getWidgetByIndex(size_t _index) {
            for (MyGUI::VectorWidgetPtr::iterator it = m_items.begin();
                 it!=m_items.end();++it) {
                size_t* idx_ptr = (*it)->getUserData<size_t>(false);
                if (idx_ptr && *idx_ptr == _index)
                    return *it;
            }
            return 0;
        }
        
        size_t ScrollList::getIndexByWidget(MyGUI::Widget* w) {
            for (MyGUI::VectorWidgetPtr::iterator it = m_items.begin();
                 it!=m_items.end();++it) {
                if (w == *it) {
                    size_t* idx_ptr = (*it)->getUserData<size_t>(false);
                    if (idx_ptr)
                        return *idx_ptr;
                    return MyGUI::ITEM_NONE;
                }
            }
            return MyGUI::ITEM_NONE;
        }
        
        void ScrollList::redrawAllItems() {
            for (MyGUI::VectorWidgetPtr::iterator it = m_items.begin();
                 it!=m_items.end();++it) {
                size_t* idx_ptr = (*it)->getUserData<size_t>(false);
                if (idx_ptr) {
                    MyGUI::IBDrawItemInfo di(*idx_ptr,m_selected_index,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,true,false);
                    drawItem(*it, di);
                }
            }
        }
        
        void ScrollList::redrawItemAt(size_t idx) {
            MyGUI::Widget* w = getWidgetByIndex(idx);
            if (w) {
                MyGUI::IBDrawItemInfo di(idx,m_selected_index,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,true,false);
                drawItem(w, di);
            }
        }
        
        void ScrollList::OnScrollMove() {
            updateWidgets();
        }
        
        void ScrollList::OnScrollBegin() {
            Base::OnScrollBegin();
            if (m_delegate) {
                m_delegate->onBeginScroll();
            }
        }
        
        void ScrollList::OnScrollEnd() {
            Base::OnScrollEnd();
            if (m_delegate) {
                m_delegate->onEndScroll();
            }
        }
        
        Vector2f ScrollList::Normalize(const Vector2f& v,bool soft) const {
            Vector2f res = Scroll::Normalize(v, soft);
            if (!soft) {
                if (getVerticalAlignment()) {
                    float max = Scroll::GetContentSize().h - Scroll::GetViewSize().h;
                    if (res.y >= ( max - m_item_size/4)) {
                        res.y = max;
                    } else {
                        res.y = m_content_margins.top + m_item_size * int((res.y - m_content_margins.top + m_item_size/2) / m_item_size);
                    }
                } else {
                    float max = Scroll::GetContentSize().w - Scroll::GetViewSize().w;
                    if (res.x >= ( max - m_item_size/4)) {
                        res.x = max;
                    } else {
                        res.x = m_content_margins.left + m_item_size * int((res.x - m_content_margins.left + m_item_size/2) / m_item_size);
                    }
                }
            }
            return res;
        }
        
        void ScrollList::updateSelectionWidget(MyGUI::Widget *w) {
            if (!m_selection_widget)
                return;
            m_selection_widget->setVisible(true);
            m_selection_widget->setPosition(w->getPosition()
                                            +m_selection_offset
                                            +getViewOffset());
        }
        
        void ScrollList::updateWidgets() {
            if (!m_delegate)
                return;
            int count = m_delegate->getItemsCount();
            if (count == 0) {
                for (MyGUI::VectorWidgetPtr::iterator it = m_items.begin();
                          it!=m_items.end();++it) {
                    (*it)->setUserData(MyGUI::Any::Null);
                    (*it)->setVisible(false);
                }
                return;
            }
            int lines = (count + m_num_subitems - 1) / m_num_subitems;
            int item_size = getItemSize();
            int first = (getScroll() - item_size) / item_size;
            int last = (getScroll() + getScrollAreaSize() + item_size) / item_size;
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
            if (last>=count) {
                last = count-1;
            }
            if (last < first) {
                last = first;
            }
            bool hide_selection = true;
            MyGUI::VectorWidgetPtr pool;
            //sb::vector<size_t> create;
            for (MyGUI::VectorWidgetPtr::iterator it = m_items.begin();
                 it!=m_items.end();++it) {
                size_t* idx_ptr = (*it)->getUserData<size_t>(false);
                if (idx_ptr) {
                    if (*idx_ptr >= first && *idx_ptr <= last) {
                        // ok, reuse
                        continue;
                    }
                }
                (*it)->setUserData(MyGUI::Any::Null);
                (*it)->setVisible(false);
                pool.push_back(*it);
            }
            for (int i = first; i<=last; ++i) {
                MyGUI::Widget* w = getWidgetByIndex(i);
                if (w) {
                    if (i == m_selected_index && m_selection_widget) {
                        updateSelectionWidget(w);
                        hide_selection = false;
                    }
                    continue;
                }
                int l = (i/m_num_subitems);
                int x = (i%m_num_subitems);
                
                if (pool.empty()) {
                    w = createWidgetItem();
                    m_items.push_back(w);
                } else {
                    w = pool.back();
                    pool.pop_back();
                    w->setVisible(true);
                }
                if (getVerticalAlignment()) {
                    w->setPosition(m_content_margins.left
                                               +x*m_item_widget_size.width,
                                                   m_content_margins.top+l*m_item_size);
                } else {
                    w->setPosition(m_content_margins.left
                                   +l*m_item_size,
                                   m_content_margins.top+x*m_item_widget_size.height);
                }
                w->setSize(m_item_widget_size);
                MyGUI::IBDrawItemInfo di(i,m_selected_index,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,true,false);
                drawItem(w, di);
                if (di.select && m_selection_widget) {
                    hide_selection = false;
                    updateSelectionWidget(w);
                }
            }
            
            if (hide_selection && m_selection_widget) {
                m_selection_widget->setVisible(false);
            }
        }
    }
}
