//
//  sb_mygui_scroll_area.cpp
//  Sandbox
//
//
//

#include "sb_mygui_scroll_area.h"
#include "luabind/sb_luabind.h"
#include "sb_lua_context.h"
#include "MyGUI_Gui.h"
#include "MyGUI_ILayer.h"
#include "MyGUI_InputManager.h"
#include "MyGUI_ScrollBar.h"
#include "MyGUI_LanguageManager.h"

#include "sb_log.h"
#include "sb_mygui_cached_widget.h"
#include "../sb_mygui_gui.h"

namespace Sandbox {
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(ScrollArea)
                
        ScrollArea::ScrollArea() {
            m_manual_scroll = true;
            m_small_scroll_enabled = false;
            m_wheel_scroll_speed = 5.0f;
            m_scroll_cursor = GHL::SYSTEM_CURSOR_MOVE;
        }
        
        ScrollArea::~ScrollArea() {
        }
        
        void ScrollArea::initialiseOverride() {
            Base::initialiseOverride();
            MyGUI::Gui::getInstance().eventFrameStart += MyGUI::newDelegate( this, &ScrollArea::frameEntered );
            MyGUI::InputManager::getInstance().eventMouseMoved += MyGUI::newDelegate(this,&ScrollArea::handleGlobalMouseMove);
            MyGUI::InputManager::getInstance().eventMousePressed += MyGUI::newDelegate(this,&ScrollArea::handleGlobalMousePressed);
            MyGUI::InputManager::getInstance().eventMouseReleased += MyGUI::newDelegate(this,&ScrollArea::handleGlobalMouseReleased);
        }
        void ScrollArea::shutdownOverride() {
            MyGUI::InputManager::getInstance().eventMouseMoved -= MyGUI::newDelegate(this,&ScrollArea::handleGlobalMouseMove);
            MyGUI::InputManager::getInstance().eventMousePressed -= MyGUI::newDelegate(this,&ScrollArea::handleGlobalMousePressed);
            MyGUI::InputManager::getInstance().eventMouseReleased -= MyGUI::newDelegate(this,&ScrollArea::handleGlobalMouseReleased);
            
            MyGUI::Gui::getInstance().eventFrameStart -= MyGUI::newDelegate( this, &ScrollArea::frameEntered );
            Base::shutdownOverride();
        }
        
        void ScrollArea::setPropertyOverride(const std::string& _key, const std::string& _value) {
            /// @wproperty{ItemBox, VisibleCount, size_t} visible cells count.
            if (_key == "ScrollBounds")
                setScrollBounds(MyGUI::utility::parseValue<int>(_value));
            else if (_key == "ManualScroll")
                setManualScroll(MyGUI::utility::parseValue<bool>(_value));
            else if (_key == "SmallScrollEnabled")
                setSmallScrollEnabled(MyGUI::utility::parseValue<bool>(_value));
            else if (_key == "ScrollStartLength")
                SetScrollStartLength(MyGUI::utility::parseFloat(MyGUI::LanguageManager::getInstance().replaceTags(_value)));
            else if (_key == "WheelScrollSpeed")
                setWheelScrollSpeed(MyGUI::utility::parseFloat(MyGUI::LanguageManager::getInstance().replaceTags(_value)));
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        void ScrollArea::setScrollBounds(int b) {
            //m_border_dempth = b;
        }
        
        void ScrollArea::setManualScroll(bool s) {
            m_manual_scroll = s;
            Scroll::Reset();
            if (mVScroll) {
                mVScroll->setEnabled(s);
            }
            if (mHScroll) {
                mHScroll->setEnabled(s);
            }
        }
        
        void ScrollArea::setWheelScrollSpeed(float v) {
            m_wheel_scroll_speed = v;
        }
        
        
        void ScrollArea::frameEntered(float dt) {
            if (!getVisible())
                return;
            update(dt);
        }
        
        void ScrollArea::update(float dt) {
            Scroll::Update(dt);
        }
        
        MyGUI::ILayerItem* ScrollArea::getLayerItemByPoint(int _left, int _top) const {
            if (!getInheritedVisible())
                return 0;
            if (Scroll::IsMove() && _checkPoint(_left, _top)) return const_cast<ScrollArea*>(this);
            return Base::getLayerItemByPoint(_left, _top);
        }
        
        void ScrollArea::notifyScrollChangePosition(MyGUI::ScrollBar* _sender, size_t _position) {
            if (!m_manual_scroll)
                return;
            Scroll::Reset();
            MyGUI::ScrollView::notifyScrollChangePosition(_sender, _position);
            SetOffset(Vector2f(-getViewOffset().left,-getViewOffset().top));
            OnScrollMove();
        }
        
        void ScrollArea::notifyMouseWheel(MyGUI::Widget* _sender, float _rel) {
            if (!m_manual_scroll)
                return;
            Vector2f pos(getWidth()/2,getHeight()/2);
            Scroll::ScrollBegin(pos);
            if (GetVEnabled()) {
                pos += Vector2f(0,_rel * m_wheel_scroll_speed);
            } else if (GetHEnabled()) {
                pos += Vector2f(_rel * m_wheel_scroll_speed,0);
            }
            Scroll::ScrollMove(pos);
            Scroll::ScrollEnd(pos);
        }
        
        void ScrollArea::cancelScroll() {
            Scroll::Cancel();
        }
        
        void ScrollArea::handleGlobalMouseMove(float x,float y) {
            if (!getInheritedVisible())
                return;
            if (!m_manual_scroll)
                return;
            if (Scroll::IsMove()) {
                Scroll::ScrollMove(Vector2f(x,y));
            }
        }
        
        void ScrollArea::handleGlobalMousePressed(float x,float y, MyGUI::MouseButton _id) {
            if (!getVisible())
                return;
            if (!m_manual_scroll)
                return;
            if (_id == MyGUI::MouseButton::Left) {
                if (!MyGUI::LayerManager::getInstance().checkItemAccessibleAtPoint(this, x, y))
                    return;
                MyGUI::ILayer* layer = getLayer();
                MyGUI::Widget* parent = getParent();
                while (!layer && parent) {
                    layer = parent->getLayer();
                    parent = parent->getParent();
                }
                if (!layer) return;
                
                MyGUI::FloatPoint pos_in_layer = layer->getPosition(x, y);
                pos_in_layer -= MyGUI::FloatPoint(mClient ? mClient->getAbsolutePosition() : getAbsolutePosition());
                MyGUI::IntRect client_rect = MyGUI::IntRect(0,0,getWidth(),getHeight());
                if (mClient) {
                    client_rect = MyGUI::IntRect(mClient->getLeft(),mClient->getTop(),mClient->getWidth(),mClient->getHeight());
                }
                if (client_rect.inside(pos_in_layer)) {
                    Scroll::ScrollBegin(Vector2f(x,y));
                }
            }
        }
        
        void ScrollArea::handleGlobalMouseReleased(float x,float y, MyGUI::MouseButton _id) {
            if (!m_manual_scroll)
                return;
            if (_id == MyGUI::MouseButton::Left) {
                Scroll::ScrollEnd(Vector2f(x,y));
                GUI::getInstancePtr()->setCursor(GHL::SYSTEM_CURSOR_DEFAULT);
            }
        }
        
        void ScrollArea::scrollToWidget(MyGUI::Widget* w) {
            if (!w)
                return;
            if (w->getParent()!=mRealClient)
                return;
            MyGUI::IntCoord srect = w->getCoord();
            MyGUI::IntSize psize = getViewSize();
            MyGUI::IntPoint offset = getViewOffset();
            srect.left += offset.left;
            srect.top += offset.top;
            MyGUI::IntPoint move;
            if (GetVEnabled()) {
                if (srect.top < 0) {
                    move.top = -srect.top;
                } else if (srect.bottom() > psize.height) {
                    move.top = psize.height - srect.bottom();
                }
            }
            if (GetHEnabled()) {
                if (srect.left < 0) {
                    move.left = -srect.left;
                } else if (srect.right() > psize.width) {
                    move.left = psize.width - srect.right();
                }
            }
            if (!move.empty()) {
                ScrollTo(GetOffset()-Sandbox::Vector2f(move.left,move.top));
            }
        }
        
        bool ScrollArea::isWidgetFullVisible(MyGUI::Widget* w) {
            if (!w)
                return true;
            if (w->getParent()!=mRealClient)
                return true;
            MyGUI::IntCoord srect = w->getCoord();
            MyGUI::IntSize psize = getViewSize();
            MyGUI::IntPoint offset = getViewOffset();
            srect.left += offset.left;
            srect.top += offset.top;
            MyGUI::IntPoint move;
            if (GetVEnabled()) {
                if (srect.top < 0) {
                    return  false;
                } else if (srect.bottom() > psize.height) {
                    return  false;
                }
            }
            if (GetHEnabled()) {
                if (srect.left < 0) {
                    return false;
                } else if (srect.right() > psize.width) {
                    return false;
                }
            }
            return true;
        }
        
        void ScrollArea::updateView() {
            Base::updateView();
            Scroll::SetContentSize(Sizef(getCanvasSize()));
            Scroll::SetViewSize(Sizef(getViewSize()));
            Scroll::SetBounds(Scroll::GetViewSize()*0.25);
            updateScrollEnable();
            if (mRealClient) {
                Scroll::SetViewPos(Vector2f(-getViewOffset().left,-getViewOffset().top));
            }
        }
        
        void ScrollArea::setScrollPos(const MyGUI::IntPoint& p) {
            Scroll::SetViewPos(Sizef(-p.left,-p.top));
            OnScrollMove();
        }
        
        void ScrollArea::setScrollPosF(const Sandbox::Vector2f &p) {
            Scroll::SetViewPos(-p);
            OnScrollMove();
        }
        
        void ScrollArea::OnScrollBegin() {
            MyGUI::InputManager::getInstance().setMouseFocusWidget(this);
            scrollBegin(this, getViewOffset());
            GUI::getInstancePtr()->setCursor(m_scroll_cursor);
        }
        
        void ScrollArea::OnScrollEnd() {
            scrollComplete(this,getViewOffset());
        }
        
        
        Vector2f ScrollArea::GetOffset() const {
            return m_real_offset;
        }
        void ScrollArea::updateScrollBars() {
            if (mHScroll != nullptr)
                mHScroll->setScrollPosition(m_real_offset.x < 0 ? 0 : m_real_offset.x);
            
            if (mVScroll != nullptr)
                mVScroll->setScrollPosition(m_real_offset.y < 0 ? 0 : m_real_offset.y);
        }
        void ScrollArea::SetOffset(const Vector2f& offset) {
            m_real_offset = offset;
            
            updateScrollBars();
            
            
            
            MyGUI::IntPoint int_pos(-offset.x,-offset.y);
            mRealClient->setPosition(int_pos);
            if (mClient && mClient->isType<CachedWidget>()) {
                static_cast<CachedWidget*>(mClient)->setSubOffset(Sandbox::Vector2f(-m_real_offset.x-int_pos.left,
                                                                                    -m_real_offset.y-int_pos.top));
            } else if (mClient) {
                MyGUI::Widget* parent = mClient->getParent();
                if (parent && parent->isType<CachedWidget>()) {
                    static_cast<CachedWidget*>(parent)->setSubOffset(Sandbox::Vector2f(-m_real_offset.x-int_pos.left,
                                                                                        -m_real_offset.y-int_pos.top));
                }
            }
        }
        
        void ScrollArea::updateRealSize(int w,int h) {
            if (mRealClient) {
                mRealClient->setSize(w,h);
            }
        }
        
        void ScrollArea::setSmallScrollEnabled(bool enabled) {
            m_small_scroll_enabled = enabled;
            updateScrollEnable();
        }
        
        void ScrollArea::updateScrollEnable() {
            Scroll::SetHEnabled(m_small_scroll_enabled || Scroll::GetContentSize().w>Scroll::GetViewSize().w);
            Scroll::SetVEnabled(m_small_scroll_enabled || Scroll::GetContentSize().h>Scroll::GetViewSize().h);
        }
    }
}
