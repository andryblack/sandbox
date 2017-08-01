#include "sb_mygui_scroll_list_base.h"
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
            virtual size_t getItemsCount() {
                if (!m_obj.Valid())
                    return 0;
                return m_obj.call_self<size_t>("getItemsCount");
            }
            LuaContextPtr getItemData(size_t idx) {
                LuaContextPtr data = m_obj.call_self<LuaContextPtr>("getItemData",idx);
                return data;
            }
            virtual void createWidget(MyGUI::Widget* w,size_t idx) {
                m_obj.call_self("createWidget",w,idx);
            }
            virtual bool canReuseWidget(MyGUI::Widget* w,size_t idx) {
                LuaContextPtr data_ptr = getItemData(idx);
                if (data_ptr && m_obj.GetValueRaw<bool>("canReuseWidget")) {
                    return m_obj.call_self<bool>("canReuseWidget",w,data_ptr,idx);
                }
                return true;
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
            virtual void onItemPressed(size_t idx) {
                LuaContextPtr data_ptr = getItemData(idx);
                if (data_ptr && m_obj.GetValueRaw<bool>("onItemPressed")) {
                    m_obj.call_self("onItemPressed",data_ptr,idx);
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
    ScrollListBase* sl = Sandbox::luabind::stack<ScrollListBase*>::get(L, 1);
    if (!sl) return 0;
    if (lua_istable(L, 2)) {
        LuaScrollListDelegate* delegate = new LuaScrollListDelegate(L,2);
        sl->setDelegate(ScrollListDelegatePtr(delegate));
    } else {
        sl->setDelegate(Sandbox::luabind::stack<ScrollListDelegatePtr>::get(L, 2));
    }
    return 0;
}

SB_META_DECLARE_OBJECT(Sandbox::mygui::ScrollListBase, Sandbox::mygui::ScrollArea)
SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ScrollListBase)
bind( property_wo( "delegate", &setDelegateImpl ) );
SB_META_METHOD(updateData)
SB_META_METHOD(itemAdded)
SB_META_METHOD(clearIndexSelected)
SB_META_METHOD(redrawAllItems)
SB_META_METHOD(redrawItemAt)
SB_META_METHOD(getWidgetByIndex)
SB_META_METHOD(getIndexByWidget)
SB_META_METHOD(upWidget)
SB_META_PROPERTY_RW(verticalAlignment,getVerticalAlignment,setVerticalAlignment)
SB_META_PROPERTY_RO(selectionWidget, getSelectionWidget)
SB_META_PROPERTY_RW(indexTop, getIndexTop,setIndexTop)
SB_META_PROPERTY_RW(indexSelected, getIndexSelected, setIndexSelected)
SB_META_PROPERTY_RW(alignOnCell, getAlignOnCell, setAlignOnCell)
SB_META_PROPERTY_RW(scroll, getScroll, setScroll)
SB_META_PROPERTY_RW(contentMargins, getContentMargins, setContentMargins)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(ScrollListBase)
        
        void register_ScrollListBase(lua_State* L) {
            luabind::Class<ScrollListDelegate>(L);
            luabind::ExternClass<ScrollListBase>(L);
        }
        
        
        
        ScrollListBase::ScrollListBase() {
            m_centered_offset = 0;
            
            m_selection_widget = 0;
            m_centered = false;
            m_vertical = false;
            m_align_on_cell = false;
            
            m_selected_index = MyGUI::ITEM_NONE;
            m_top_index = MyGUI::ITEM_NONE;
            updateScrollEnable();
            setSmallScrollEnabled(true);
        }
        
        ScrollListBase::~ScrollListBase() {
        }
        
        void ScrollListBase::initialiseOverride() {
            Base::initialiseOverride();
            assignWidget(m_selection_widget, "Selection");
            if (m_selection_widget) {
                m_selection_widget->setVisible(false);
                m_selection_offset = m_selection_widget->getPosition();
            }
        }
        void ScrollListBase::shutdownOverride() {
            Base::shutdownOverride();
            m_delegate.reset();
        }

        void ScrollListBase::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "VerticalAlignment")
                setVerticalAlignment(MyGUI::utility::parseValue<bool>(_value));
            else if (_key == "ContentMargins")
                setContentMargins(MyGUI::utility::parseValue<MyGUI::IntRect>(_value));
            else if (_key == "ScrollBounds")
                setScrollBounds(MyGUI::utility::parseValue<int>(_value));
            else if (_key == "Centered")
                setCentered(MyGUI::utility::parseValue<bool>(_value));
            else if (_key == "ManualScroll")
                setManualScroll(MyGUI::utility::parseValue<bool>(_value));
            else if (_key == "AlignOnCell")
                setAlignOnCell(MyGUI::utility::parseValue<bool>(_value));
            else if (_key == "AlignTime")
                SetNormalizationTime(MyGUI::utility::parseValue<float>(_value));
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        void ScrollListBase::setScrollBounds(int b) {
            if (getVerticalAlignment()) {
                Scroll::SetBounds(Sizef(0,b));
            } else {
                Scroll::SetBounds(Sizef(b,0));
            }
        }
        
        void ScrollListBase::setDelegate(const ScrollListDelegatePtr &delegate) {
            m_delegate = delegate;
            if (!m_delegate) return;
            resetWidgets();
            updateView();
            updateWidgets();
            _updateChilds();
        }

        void ScrollListBase::itemAdded() {
            if (!m_delegate) return;
            updateView();
            resetWidgets();
        }
        
        void ScrollListBase::updateData() {
            if (!m_delegate) return;
            resetWidgets();
            updateView();
            updateWidgets();
        }


        void ScrollListBase::handleItemClick(MyGUI::Widget* _sender) {
            size_t index = getIndexByWidget(_sender);
            if (index == MyGUI::ITEM_NONE)
                return;
            if (m_delegate) {
                m_delegate->onItemClick( index );
            }
            setIndexSelected( index);
        }
        
        void ScrollListBase::handleItemPressed(MyGUI::Widget* _sender,float x,float y,MyGUI::MouseButton btn) {
            size_t index = getIndexByWidget(_sender);
            if (index == MyGUI::ITEM_NONE)
                return;
            if (m_delegate) {
                m_delegate->onItemPressed( index  );
            }
        }
        
        void ScrollListBase::onMouseDrag(int _left, int _top, MyGUI::MouseButton _id) {
            Base::onMouseDrag(_left, _top, _id);
        }
        void ScrollListBase::onMouseButtonPressed(int _left, int _top, MyGUI::MouseButton _id) {
            Base::onMouseButtonPressed(_left, _top, _id);
        }
        void ScrollListBase::onMouseButtonReleased(int _left, int _top, MyGUI::MouseButton _id) {
            Base::onMouseButtonReleased(_left, _top, _id);
        }
        
        void ScrollListBase::setScroll(float pos) {
            Vector2f idiff(0,0);
            if (getVerticalAlignment()) {
                idiff.y = pos;
            } else {
                idiff.x = pos;
            }
            ScrollArea::SetOffset(ScrollArea::Normalize(idiff,false));
            updateWidgets();
        }
        
        void ScrollListBase::setContentPosition(const MyGUI::IntPoint& pos) {
            Base::setContentPosition(pos);
        }
                
        float     ScrollListBase::getScroll() const {
            if (getVerticalAlignment())
                return ScrollArea::GetOffset().y;
            return ScrollArea::GetOffset().x;
        }
        
        
        void ScrollListBase::setIndexSelected(size_t _index) {
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
        
        void  ScrollListBase::clearIndexSelected() {
            setIndexSelected(MyGUI::ITEM_NONE);
        }
        
        void ScrollListBase::updateView() {
            ScrollView::updateView();
            updateContent();
            Base::updateView();
            ScrollArea::SetOffset(Normalize(GetOffset(), true));
            updateWidgets();
        }

        
        void ScrollListBase::setVerticalAlignment(bool _value) {
            m_vertical = _value;
            updateView();
        }
        
        void ScrollListBase::setAlignOnCell(bool _value) {
            m_align_on_cell = _value;
            updateView();
        }
        
        void ScrollListBase::setContentMargins(const MyGUI::IntRect& _value) {
            m_content_margins = _value;
            updateView();
        }
        
        void ScrollListBase::setCentered(bool c) {
            m_centered = c;
            if (!m_centered) {
                setCenteredOffset(0);
            }
            updateView();
        }
        
                
        void ScrollListBase::redrawItemAt(size_t idx) {
            MyGUI::Widget* w = getWidgetByIndex(idx);
            if (w) {
                MyGUI::IBDrawItemInfo di(idx,m_selected_index,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,true,false);
                drawItem(w, di);
            }
        }
        
        void ScrollListBase::OnScrollMove() {
            updateWidgets();
        }
        
        void ScrollListBase::OnScrollBegin() {
            Base::OnScrollBegin();
            if (m_delegate) {
                m_delegate->onBeginScroll();
            }
        }
        
        void ScrollListBase::OnScrollEnd() {
            Base::OnScrollEnd();
            if (m_delegate) {
                m_delegate->onEndScroll();
            }
        }
        
        
        
        int ScrollListBase::getScrollMargin() const {
            if (getVerticalAlignment())
                return getContentMargins().top;
            return getContentMargins().left;
        }
        
        
        int     ScrollListBase::getScrollAreaSize() const {
            if (getVerticalAlignment()) {
                return Scroll::GetViewSize().h;
            }
            return Scroll::GetViewSize().w;
        }

        
        
        
        void ScrollListBase::resetWidgets() {
            for (MyGUI::VectorWidgetPtr::iterator it = m_all_items.begin();
                 it!=m_all_items.end();++it) {
                (*it)->setUserData(MyGUI::Any::Null);
                (*it)->setVisible(false);
            }
        }

        MyGUI::Widget* ScrollListBase::getWidgetByIndex(size_t _index) {
            for (MyGUI::VectorWidgetPtr::iterator it = m_all_items.begin();
                 it!=m_all_items.end();++it) {
                size_t* idx_ptr = (*it)->getUserData<size_t>(false);
                if (idx_ptr && *idx_ptr == _index)
                    return *it;
            }
            return 0;
        }
        
        size_t ScrollListBase::getIndexByWidget(MyGUI::Widget* w) {
            for (MyGUI::VectorWidgetPtr::iterator it = m_all_items.begin();
                 it!=m_all_items.end();++it) {
                if (w == *it) {
                    size_t* idx_ptr = (*it)->getUserData<size_t>(false);
                    if (idx_ptr)
                        return *idx_ptr;
                    return MyGUI::ITEM_NONE;
                }
            }
            return MyGUI::ITEM_NONE;
        }
        
        void ScrollListBase::redrawAllItems() {
            for (MyGUI::VectorWidgetPtr::iterator it = m_all_items.begin();
                 it!=m_all_items.end();++it) {
                size_t* idx_ptr = (*it)->getUserData<size_t>(false);
                if (idx_ptr) {
                    MyGUI::IBDrawItemInfo di(*idx_ptr,m_selected_index,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,true,false);
                    drawItem(*it, di);
                }
            }
        }
        
        void ScrollListBase::updateSelectionWidget(MyGUI::Widget *w) {
            if (!m_selection_widget)
                return;
            m_selection_widget->setVisible(true);
            m_selection_widget->setPosition(w->getPosition()
                                            +m_selection_offset
                                            +getViewOffset());
        }
        
        void ScrollListBase::upWidget(MyGUI::Widget* w) {
            if (w == m_all_items.back())
                return;
            MyGUI::VectorWidgetPtr::iterator it = std::find(m_all_items.begin(),m_all_items.end(),w);
            if (it!=m_all_items.end()) {
                m_all_items.erase(it);
                w->detachFromWidget();
                w->attachToWidget(mRealClient);
                m_all_items.push_back(w);
            }
        }
        
        void ScrollListBase::setIndexTop(size_t idx) {
            m_top_index = idx;
            MyGUI::Widget* w = getWidgetByIndex(idx);
            if (w) {
                upWidget(w);
            }
        }
        
        MyGUI::Widget* ScrollListBase::createWidgetItem(size_t idx) {
            MyGUI::Widget* w = mRealClient->createWidget<MyGUI::Widget>("Default",
                                                                        MyGUI::IntCoord(0, 0, 10, 10), MyGUI::Align::Default);
            w->eventMouseButtonClick += MyGUI::newDelegate(this,&ScrollListBase::handleItemClick);
            w->eventMouseButtonPressed += MyGUI::newDelegate(this,&ScrollListBase::handleItemPressed);
            return w;
        }

        void ScrollListBase::drawItem(MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di) {
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
        
        MyGUI::Widget* ScrollListBase::updateWidget(size_t i, MyGUI::VectorWidgetPtr& pool) {
            MyGUI::Widget* w = 0;
            bool is_new = false;
            for (MyGUI::VectorWidgetPtr::iterator it = pool.begin();it!=pool.end();++it) {
                if (!m_delegate || m_delegate->canReuseWidget(*it, i)) {
                    w = *it;
                    pool.erase(it);
                    break;
                }
            }
            if (!w) {
                w = createWidgetItem(i);
                m_all_items.push_back(w);
                is_new = true;
            }
            w->setVisible(true);
            alignWidget(w,i);
            if (is_new && m_delegate) {
                m_delegate->createWidget(w, i);
                w->setSize(w->getSize());
            }
            
            MyGUI::IBDrawItemInfo di(i,m_selected_index,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,MyGUI::ITEM_NONE,true,false);
            drawItem(w, di);
            return w;
        }

        void ScrollListBase::updateWidgets() {
            if (!m_delegate)
                return;
            int count = int(m_delegate->getItemsCount());
            if (count == 0) {
                resetWidgets();
                return;
            }
            
            int first;
            int last;
            getVisibleItems(first,last);
            
            if (last>=count) {
                last = count-1;
            }
            if (last < first) {
                last = first;
            }
            
            MyGUI::Widget* selected_widget = 0;
            MyGUI::Widget* top_widget = 0;
            MyGUI::VectorWidgetPtr pool;
            //sb::vector<size_t> create;
            for (MyGUI::VectorWidgetPtr::iterator it = m_all_items.begin();
                 it!=m_all_items.end();++it) {
                size_t* idx_ptr = (*it)->getUserData<size_t>(false);
                if (idx_ptr) {
                    if (*idx_ptr >= first && *idx_ptr <= last) {
                        // ok, reuse
                        if (*idx_ptr == m_selected_index) {
                            selected_widget = *it;
                        }
                        if (*idx_ptr == m_top_index) {
                            top_widget = *it;
                        }
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
                    continue;
                }
                w = updateWidget(i, pool);
                if ( i == m_selected_index ) {
                    selected_widget = w;
                }
                if ( i == m_top_index ) {
                    top_widget = w;
                }
            }
            
            if (!selected_widget) {
                if (m_selection_widget)
                    m_selection_widget->setVisible(false);
            } else {
                if (m_selection_widget) {
                    updateSelectionWidget(selected_widget);
                    m_selection_widget->setVisible(true);
                }
            }
            if (top_widget) {
                upWidget(top_widget);
            }
        }

        void ScrollListBase::updateScrollEnable() {
            if (m_vertical) {
                Scroll::SetVEnabled(getSmallScrollEnabled() || Scroll::GetContentSize().h>Scroll::GetViewSize().h);
                Scroll::SetHEnabled(false);
            } else {
                Scroll::SetVEnabled(false);
                Scroll::SetHEnabled(getSmallScrollEnabled() || Scroll::GetContentSize().w>Scroll::GetViewSize().w);
            }
            
        }


    }
}
