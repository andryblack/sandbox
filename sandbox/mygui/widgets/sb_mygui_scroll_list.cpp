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

#include "sb_log.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::ScrollListDelegate, Sandbox::meta::object)
SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ScrollListDelegate)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    namespace mygui {
        
        const float min_speed = 5.0f;
        const int min_scroll_distance = 10;
        const float max_speed = 2048.0f*5;
        
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
                return m_obj.call_self<size_t>("getItemsCount");
            }
            virtual MyGUI::Any getItemData(size_t idx) {
                LuaContextPtr data = m_obj.call_self<LuaContextPtr>("getItemData",idx);
                return MyGUI::Any(data);
            }
            virtual void createWidget(MyGUI::Widget* w) {
                m_obj.call_self("createWidget",w);
            }
            virtual void updateWidget(MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di) {
                LuaContextPtr* data_ptr = m_list->getItemDataAt<LuaContextPtr>(di.index,false);
                if (data_ptr) {
                    m_obj.call_self("updateWidget",w,*data_ptr,di);
                }
            }
            virtual void onItemClick(size_t idx) {
                LuaContextPtr* data_ptr = m_list->getItemDataAt<LuaContextPtr>(idx,false);
                if (data_ptr) {
                    m_obj.call_self("onItemClick",*data_ptr,idx);
                }
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

SB_META_DECLARE_OBJECT(Sandbox::mygui::ScrollList, MyGUI::ItemBox)
SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ScrollList)
bind( property_wo( "delegate", &setDelegateImpl ) );
SB_META_METHOD(moveNext)
SB_META_METHOD(movePrev)
SB_META_METHOD(moveToPage)
SB_META_PROPERTY_RW(page, getPage, setPage)
SB_META_PROPERTY_RW(targetPage, getTargetPage, moveToPage)
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
            requestCreateWidgetItem = MyGUI::newDelegate(this,&ScrollList::handleCreateWidgetItem);
            requestCoordItem = MyGUI::newDelegate(this,&ScrollList::handleCoordItem);
            requestDrawItem = MyGUI::newDelegate(this,&ScrollList::handleDrawItem);
            //eventMouseItemActivate += MyGUI::newDelegate(this,&ScrollList::handleItemActivate);
            setNeedDragDrop(false);
            m_visible_count = 1;
            m_num_subitems = 1;
            m_move_accum  = 0.0f;
            m_move_speed = 0.0f;
            m_scroll_target = 0;
            m_state = state_none;
            m_border_dempth = 100;
            m_centered = false;
        }
        
        ScrollList::~ScrollList() {
        }
        
        void ScrollList::initialiseOverride() {
            Base::initialiseOverride();
            MyGUI::Gui::getInstance().eventFrameStart += MyGUI::newDelegate( this, &ScrollList::frameEntered );
            MyGUI::InputManager::getInstance().eventMouseMoved += MyGUI::newDelegate(this,&ScrollList::handleGlobalMouseMove);
            MyGUI::InputManager::getInstance().eventMousePressed += MyGUI::newDelegate(this,&ScrollList::handleGlobalMousePressed);
            MyGUI::InputManager::getInstance().eventMouseReleased += MyGUI::newDelegate(this,&ScrollList::handleGlobalMouseReleased);
        }
        void ScrollList::shutdownOverride() {
            MyGUI::InputManager::getInstance().eventMouseMoved -= MyGUI::newDelegate(this,&ScrollList::handleGlobalMouseMove);
            MyGUI::InputManager::getInstance().eventMousePressed -= MyGUI::newDelegate(this,&ScrollList::handleGlobalMousePressed);
            MyGUI::InputManager::getInstance().eventMouseReleased -= MyGUI::newDelegate(this,&ScrollList::handleGlobalMouseReleased);

            MyGUI::Gui::getInstance().eventFrameStart -= MyGUI::newDelegate( this, &ScrollList::frameEntered );
            Base::shutdownOverride();
            if (m_delegate) {
                m_delegate->setScrollList(0);
            }
            m_delegate.reset();
        }
        
        void ScrollList::setPropertyOverride(const std::string& _key, const std::string& _value) {
            /// @wproperty{ItemBox, VisibleCount, size_t} visible cells count.
            if (_key == "VisibleCount")
                setVisibleCount(MyGUI::utility::parseValue<size_t>(_value));
            else if (_key == "SubItemsCount")
                setSubItems(MyGUI::utility::parseValue<size_t>(_value));
            /// @wproperty{ItemBox, VerticalAlignment, bool} bounds.
            else if (_key == "ScrollBounds")
                setScrollBounds(MyGUI::utility::parseValue<int>(_value));
            else if (_key == "Centered")
                setCentered(MyGUI::utility::parseValue<bool>(_value));
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        void ScrollList::setVisibleCount(size_t count) {
            if (count<1)
                count = 1;
            if (m_visible_count == count)
                return;
            m_visible_count = count;
            //mCountItemInLine = -1;
            updateFromResize();
        }
        
        void ScrollList::setSubItems(size_t count) {
            if (count<1)
                count = 1;
            if (m_num_subitems == count)
                return;
            m_num_subitems = count;
            //mCountItemInLine = -1;
            updateFromResize();
        }
        
        void ScrollList::setScrollBounds(int b) {
            m_border_dempth = b;
        }
        
        void ScrollList::setCentered(bool c) {
            if (m_centered != c) {
                m_centered = c;
                updateFromResize();
            }
        }
        
        void ScrollList::setDelegate(const ScrollListDelegatePtr &delegate) {
            removeAllItems();
            m_delegate = delegate;
            if (!m_delegate) return;
            m_delegate->setScrollList(this);
            size_t count = m_delegate->getItemsCount();
            for (size_t i=0;i<count;++i) {
                addItem(m_delegate->getItemData(i));
            }
            setScroll(normalizeScrollValue(0));
        }
        
        void ScrollList::handleCreateWidgetItem(MyGUI::ItemBox*, MyGUI::Widget* w) {
            if (m_delegate) {
                m_delegate->createWidget(w);
                w->eventMouseButtonClick += MyGUI::newDelegate(this,&ScrollList::handleItemClick);
            }
        }
        void ScrollList::handleCoordItem(MyGUI::ItemBox*, MyGUI::IntCoord& coords, bool drag) {
            if (m_visible_count && getClientWidget()) {
                if (getVerticalAlignment()) {
                    coords.width = getClientWidget()->getSize().width / m_num_subitems;
                    coords.height = getClientWidget()->getSize().height / m_visible_count;
                } else {
                    coords.width = getClientWidget()->getSize().width / m_visible_count;
                    coords.height = getClientWidget()->getSize().height / m_num_subitems;
                }
            }
            
        }
        void ScrollList::handleDrawItem(MyGUI::ItemBox*, MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di) {
            if (m_delegate && di.update) {
                m_delegate->updateWidget(w, di);
            }
        }
        
        void ScrollList::handleItemClick(MyGUI::Widget* _sender) {
            if (m_delegate) {
                m_delegate->onItemClick( getIndexByWidget(_sender) );
            }
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
            MyGUI::IntPoint idiff(0,0);
//            int norm = normalizeScrollValue(pos);
//            int err = norm - pos;
//            
//            err = err * (1.0f -fabs(err)/m_border_dempth);
//            
//            pos = norm - err;
//            
            if (getVerticalAlignment()) {
                idiff.top = pos;
             } else {
                idiff.left = pos;
            }
            resetCurrentActiveItem();
            setContentPosition(idiff);
            //setViewOffset(idiff);
        }
        
        int     ScrollList::getScroll() const {
            if (getVerticalAlignment())
                return getViewOffset().top;
            return getViewOffset().left;
        }
        
        int     ScrollList::getItemSize() const {
            return getScrollAreaSize() / m_visible_count;
        }
        
        int     ScrollList::getScrollContentSize() const {
            if (getVerticalAlignment())
                return getContentSize().height;
            return getContentSize().width;
        }
        
        int     ScrollList::getScrollAreaSize() const {
            if (getVerticalAlignment()) {
                return getClientWidget()->getSize().height;
            }
            return getClientWidget()->getSize().width;
        }
        
        int ScrollList::normalizeScrollValue(int val) const {
            int max_scroll = getScrollContentSize() - getScrollAreaSize();
            if (val>max_scroll)
                val = max_scroll;
            if (val<0) {
                val = 0;
            }
            if (m_centered) {
                if (getItemCount() < m_visible_count) {
                    val = -(getScrollAreaSize() - getItemSize() * getItemCount())/2;
                }
            }
            return val;
        }
        
        void ScrollList::moveNext() {
            m_scroll_target = normalizeScrollValue(m_scroll_target+getItemSize());
            if (m_state==state_none || m_state == state_free_scroll) {
                m_move_speed += getItemSize() * 2.0f;
                startFreeScroll();
            }
        }
        
        void ScrollList::moveToPage(int idx) {
            m_scroll_target = normalizeScrollValue(getItemSize()*idx);
            if (m_state==state_none || m_state == state_free_scroll) {
                m_move_speed += getItemSize() * 2.0f;
                startFreeScroll();
            }
        }
        
        void ScrollList::startFreeScroll() {
            if (m_state!=state_free_scroll) {
                m_state = state_free_scroll;
                if (m_delegate) {
                    m_delegate->onFreeScroll();
                }
            }
        }
        
        int ScrollList::getTargetPage() const {
            return (m_scroll_target + getItemSize() / 2) / getItemSize();
        }
        
        void ScrollList::setPage(int page) {
            setScroll(normalizeScrollValue(getItemSize()*page));
        }
        
        int ScrollList::getPage() const {
            return (getScroll() + getItemSize() / 2) / getItemSize();
        }
        
        void ScrollList::movePrev() {
            m_scroll_target = normalizeScrollValue(m_scroll_target-getItemSize());
            if (m_state==state_none || m_state == state_free_scroll) {
                m_move_speed += getItemSize() * 2.0f;
                startFreeScroll();
            }
        }
        
        void ScrollList::frameEntered(float dt) {
            if (!getVisible())
                return;
            if (m_state!=state_free_scroll) {
                return;
            }
            int crnt_pos = getScroll();
            if (crnt_pos!=m_scroll_target) {
                
                
                
                float speed = m_move_speed;
                
                float dist = std::abs(m_scroll_target-crnt_pos);
                speed = dist * 5;
                if (speed<min_speed)
                    speed=min_speed;
                if (speed>max_speed)
                    speed=max_speed;
                
                m_move_accum += dt * speed;
                int delta = int(m_move_accum);
                if (!delta)
                    return;
                m_move_accum-=delta;
                
                if (crnt_pos<m_scroll_target) {
                    crnt_pos += delta;
                    if (crnt_pos > m_scroll_target) {
                        crnt_pos = m_scroll_target;
                        //m_move_speed = 0;
                    }
                } else {
                    crnt_pos -= delta;
                    if (crnt_pos < m_scroll_target) {
                        crnt_pos = m_scroll_target;
                        //m_move_speed = 0;
                    }
                }
                setScroll(crnt_pos);
            } else {
                m_move_speed = 0;
                m_move_accum = 0;
                if (m_state != state_none) {
                    m_state = state_none;
                    if (m_delegate) {
                        m_delegate->onEndScroll();
                    }
                }
               
            }
        }
        
        MyGUI::ILayerItem* ScrollList::getLayerItemByPoint(int _left, int _top) const {
            if (!getInheritedVisible())
                return 0;
            if (m_state == state_manual_scroll && _checkPoint(_left, _top)) return const_cast<ScrollList*>(this);
            return Base::getLayerItemByPoint(_left, _top);
        }
        
        void ScrollList::handleGlobalMouseMove(int x,int y) {
            if (!getInheritedVisible())
                return;
            if (m_state == state_wait_scroll || m_state == state_manual_scroll) {
                MyGUI::ILayer* layer = getLayer();
                MyGUI::Widget* parent = getParent();
                while (!layer && parent) {
                    layer = parent->getLayer();
                    parent = parent->getParent();
                }
                if (!layer) return;
                MyGUI::IntPoint pos_in_layer = layer->getPosition(x, y);
                pos_in_layer -= getAbsolutePosition();
                MyGUI::IntPoint move = pos_in_layer - m_scroll_prev_pos;
                
                int delta_scroll = 0;
                if (getVerticalAlignment()) {
                    delta_scroll = -move.top;
                } else {
                    delta_scroll = -move.left;
                }
                int crnt_scroll = getScroll();
                int new_scroll = crnt_scroll + delta_scroll;
                if (m_state == state_wait_scroll) {
                    if (std::abs((new_scroll-m_scroll_begin))>min_scroll_distance) {
                        m_state = state_manual_scroll;
                        if (m_delegate) {
                            m_delegate->onBeginScroll();
                        }
                        MyGUI::InputManager::getInstance()._resetMouseFocusWidget();
                        //getClientWidget()->setEnabled(false);
                        MyGUI::Widget* w = MyGUI::InputManager::getInstance().getMouseFocusWidget();
                        if (w) {
                            //w->_riseMouseButtonReleased(0, 0, MyGUI::MouseButton::Left);
                        }
                        LogInfo() << "set manual scroll";
                    }
                }
                
                
                
                int norm = normalizeScrollValue(new_scroll);
                if (m_border_dempth > 0) {
                    int err = norm - new_scroll;
                    
                    float derr = std::abs(err)/m_border_dempth;
                    if (derr>1.0f) {
                        derr = 1.0f;
                    } else if (derr<0.0f) {
                        derr = 0.0f;
                    }

                    new_scroll = crnt_scroll + delta_scroll * (1.0f - derr);
                } else {
                    new_scroll = norm;
                }
                
                setScroll(new_scroll);
                
                new_scroll = getScroll();
                m_scroll_prev_pos = pos_in_layer;
                
                delta_scroll = new_scroll - crnt_scroll;
                if (true) {
                    unsigned long dt = m_scroll_timer.getMilliseconds();
                    if (dt) {
                        m_move_speed = (m_move_speed*0.9f)+(float(delta_scroll) / (dt*0.001f))*0.1f;
                        m_scroll_timer.reset();
                    }
                }
            }
        }
        
        void ScrollList::handleGlobalMousePressed(int x,int y, MyGUI::MouseButton _id) {
            if (!getVisible())
                return;
            if (_id == MyGUI::MouseButton::Left) {
                MyGUI::ILayer* layer = getLayer();
                MyGUI::Widget* parent = getParent();
                while (!layer && parent) {
                    layer = parent->getLayer();
                    parent = parent->getParent();
                }
                if (!layer) return;
                MyGUI::IntPoint pos_in_layer = layer->getPosition(x, y);
                pos_in_layer -= getAbsolutePosition();
                if (m_state == state_none || m_state == state_free_scroll) {
                    if (pos_in_layer.left > 0 && pos_in_layer.top > 0 &&
                        pos_in_layer.left < getWidth() && pos_in_layer.top < getHeight()) {
                        m_state = state_wait_scroll;
                        //LogInfo() << "set wait scroll";
                        m_scroll_prev_pos = pos_in_layer;
                        m_scroll_begin = getScroll();
                        m_scroll_timer.reset();
                        m_move_speed = 0.0f;
                    }
                }
            }
        }
        
        void ScrollList::handleGlobalMouseReleased(int x,int y, MyGUI::MouseButton _id) {
            
            if (_id == MyGUI::MouseButton::Left) {
                if (m_state == state_manual_scroll || m_state == state_wait_scroll) {
                    if (m_state == state_manual_scroll) {
                        //getClientWidget()->setEnabled(true);
                    }
                    float speed_dir = m_move_speed < 0 ? -1.0f : 1.0f;
                    m_move_speed = fabs(m_move_speed);
                    if (m_move_speed>max_speed)
                        m_move_speed = max_speed;
                    
                    int scroll_distance = m_move_speed * ( 0.2f ) * speed_dir;
                    m_scroll_target = roundf(float(getScroll()+scroll_distance)/getItemSize()) * getItemSize();
                    
                    m_scroll_target = normalizeScrollValue(m_scroll_target);
                    
                    startFreeScroll();
                    //LogInfo() << "set animate scroll";
                } else {
                    //m_state = state_none;
                }
            }
        }
        
        
               
        
        

    }
}
