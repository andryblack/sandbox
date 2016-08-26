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

#include "sb_log.h"

namespace Sandbox {
    
    namespace mygui {
        
        const float min_speed = 5.0f;
        const int min_scroll_distance = 10;
        const float max_speed = 2048.0f*5;
        
    }
}



namespace Sandbox {
    
    namespace mygui {
                
        ScrollArea::ScrollArea() {
            
            m_move_accum.left  = 0.0f;
            m_move_accum.top  = 0.0f;
            m_move_speed.left = 0.0f;
            m_move_speed.top  = 0.0f;
            m_scroll_target.left = 0;
            m_scroll_target.top = 0;
            m_state = state_none;
            m_border_dempth = 100;
            m_manual_scroll = true;
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
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        void ScrollArea::setScrollBounds(int b) {
            m_border_dempth = b;
        }
        
        void ScrollArea::setManualScroll(bool s) {
            m_manual_scroll = s;
            m_state = state_none;
            if (mVScroll) {
                mVScroll->setEnabled(s);
            }
            if (mHScroll) {
                mHScroll->setEnabled(s);
            }
        }
        
        
        void ScrollArea::frameEntered(float dt) {
            if (!getVisible())
                return;
            if (m_state!=state_free_scroll) {
                return;
            }
            MyGUI::IntPoint crnt_pos = getViewOffset();
            if (crnt_pos!=m_scroll_target) {
                
                
                
                MyGUI::FloatPoint speed = m_move_speed;
                
                MyGUI::FloatPoint dist = MyGUI::FloatPoint( std::abs(m_scroll_target.left - crnt_pos.left),
                                          std::abs(m_scroll_target.top - crnt_pos.top) );
                
                speed.left = dist.left * 5;
                speed.top = dist.top * 5;
                
                if (speed.left<min_speed)
                    speed.left=min_speed;
                if (speed.left>max_speed)
                    speed.left=max_speed;
                
                if (speed.top<min_speed)
                    speed.top=min_speed;
                if (speed.top>max_speed)
                    speed.top=max_speed;
                
                m_move_accum.left += dt * speed.left;
                m_move_accum.top += dt * speed.top;
                
                MyGUI::IntPoint delta = MyGUI::IntPoint(m_move_accum.left,m_move_accum.top);
                if (!delta.left && !delta.top)
                    return;
                
                m_move_accum.left-=delta.left;
                m_move_accum.top-=delta.top;
                
                if (crnt_pos.left<m_scroll_target.left) {
                    crnt_pos.left += delta.left;
                    if (crnt_pos.left > m_scroll_target.left) {
                        crnt_pos.left = m_scroll_target.left;
                        //m_move_speed = 0;
                    }
                } else {
                    crnt_pos.left -= delta.left;
                    if (crnt_pos.left < m_scroll_target.left) {
                        crnt_pos.left = m_scroll_target.left;
                        //m_move_speed = 0;
                    }
                }
                
                if (crnt_pos.top<m_scroll_target.top) {
                    crnt_pos.top += delta.top;
                    if (crnt_pos.top > m_scroll_target.top) {
                        crnt_pos.top = m_scroll_target.top;
                        //m_move_speed = 0;
                    }
                } else {
                    crnt_pos.top -= delta.top;
                    if (crnt_pos.top < m_scroll_target.top) {
                        crnt_pos.top = m_scroll_target.top;
                        //m_move_speed = 0;
                    }
                }
                setViewOffset(crnt_pos);
            } else {
                m_move_speed = MyGUI::FloatPoint(0,0);
                m_move_accum = MyGUI::FloatPoint(0,0);
                m_state = state_none;
                scrollComplete(this,m_scroll_target);
            }
        }
        
        MyGUI::ILayerItem* ScrollArea::getLayerItemByPoint(int _left, int _top) const {
            if (!getInheritedVisible())
                return 0;
            if (m_state == state_manual_scroll && _checkPoint(_left, _top)) return const_cast<ScrollArea*>(this);
            return Base::getLayerItemByPoint(_left, _top);
        }
        
        void ScrollArea::notifyScrollChangePosition(MyGUI::ScrollBar* _sender, size_t _position) {
            if (!m_manual_scroll)
                return;
            m_state = state_none;
            MyGUI::ScrollView::notifyScrollChangePosition(_sender, _position);
        }
        
        void ScrollArea::handleGlobalMouseMove(int x,int y) {
            if (!getInheritedVisible())
                return;
            if (!m_manual_scroll)
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
                
                MyGUI::IntPoint delta_scroll = MyGUI::IntPoint(0,0);
                
                delta_scroll.left = move.left;
                delta_scroll.top = move.top;
                
                MyGUI::IntPoint crnt_scroll = getViewOffset();
                
                MyGUI::IntPoint new_scroll = crnt_scroll + delta_scroll;
                if (m_state == state_wait_scroll) {
                    if ((std::abs((new_scroll.left-m_scroll_begin.left))>min_scroll_distance)||
                        (std::abs((new_scroll.top-m_scroll_begin.top))>min_scroll_distance)) {
                        m_state = state_manual_scroll;
//                        MyGUI::InputManager::getInstance()._resetMouseFocusWidget();
//                        //getClientWidget()->setEnabled(false);
//                        MyGUI::Widget* w = MyGUI::InputManager::getInstance().getMouseFocusWidget();
//                        if (w) {
//                            //w->_riseMouseButtonReleased(0, 0, MyGUI::MouseButton::Left);
//                        }
//                        LogInfo() << "set manual scroll";
                        MyGUI::InputManager::getInstance().setMouseFocusWidget(this);
                    }
                }
                
                
                
                MyGUI::IntPoint norm = normalizeScrollValue(new_scroll);
                if (m_border_dempth > 0 && mHRange) {
                    int err = norm.left - new_scroll.left;
                    
                    float derr = std::abs(err)/m_border_dempth;
                    if (derr>1.0f) {
                        derr = 1.0f;
                    } else if (derr<0.0f) {
                        derr = 0.0f;
                    }
                    
                    new_scroll.left = crnt_scroll.left + delta_scroll.left * (1.0f - derr);
                    
                } else {
                    new_scroll.left = norm.left;
                }
                
                if (m_border_dempth > 0 && mVRange) {
                    int err = norm.top - new_scroll.top;
                    
                    float derr = std::abs(err)/m_border_dempth;
                    if (derr>1.0f) {
                        derr = 1.0f;
                    } else if (derr<0.0f) {
                        derr = 0.0f;
                    }
                    
                    new_scroll.top = crnt_scroll.top + delta_scroll.top * (1.0f - derr);
                    
                } else {
                    new_scroll.top = norm.top;
                }
                
                setViewOffset(new_scroll);
                
                new_scroll = getViewOffset();
                m_scroll_prev_pos = pos_in_layer;
                
                delta_scroll = new_scroll - crnt_scroll;
                if (true) {
                    unsigned long dt = m_scroll_timer.getMilliseconds();
                    if (dt) {
                        m_move_speed.left = (m_move_speed.left*0.9f)+(float(delta_scroll.left) / (dt*0.001f))*0.1f;
                        m_move_speed.top = (m_move_speed.top*0.9f)+(float(delta_scroll.top) / (dt*0.001f))*0.1f;
                        m_scroll_timer.reset();
                    }
                }
            }
        }
        
        void ScrollArea::handleGlobalMousePressed(int x,int y, MyGUI::MouseButton _id) {
            if (!getVisible())
                return;
            if (!m_manual_scroll)
                return;
            if (_id == MyGUI::MouseButton::Left) {
                MyGUI::Widget* top_modal = MyGUI::InputManager::getInstance().getTopModalWidget();
                if (!top_modal)
                    return;
                bool on_top_modal = top_modal == this;
                MyGUI::ILayer* layer = getLayer();
                MyGUI::Widget* parent = getParent();
                while (!layer && parent) {
                    if (parent == top_modal)
                        on_top_modal = true;
                    layer = parent->getLayer();
                    parent = parent->getParent();
                }
                if (!on_top_modal) return;
                if (!layer) return;
                MyGUI::IntPoint pos_in_layer = layer->getPosition(x, y);
                pos_in_layer -= mClient ? mClient->getAbsolutePosition() : getAbsolutePosition();
                if (m_state == state_none || m_state == state_free_scroll) {
                    MyGUI::IntRect client_rect = MyGUI::IntRect(0,0,getWidth(),getHeight());
                    if (mClient) {
                        client_rect = MyGUI::IntRect(mClient->getLeft(),mClient->getTop(),mClient->getWidth(),mClient->getHeight());
                    }
                    if (client_rect.inside(pos_in_layer)) {
                        m_state = state_wait_scroll;
                        //LogInfo() << "set wait scroll";
                        m_scroll_prev_pos = pos_in_layer;
                        m_scroll_begin = getViewOffset();
                        m_scroll_timer.reset();
                        m_move_speed.left = 0.0f;
                        m_move_speed.top = 0.0f;
                    }
                }
            }
        }
        
        void ScrollArea::handleGlobalMouseReleased(int x,int y, MyGUI::MouseButton _id) {
            if (!m_manual_scroll)
                return;
            if (_id == MyGUI::MouseButton::Left) {
                if (m_state == state_manual_scroll || m_state == state_wait_scroll) {
                    if (m_state == state_manual_scroll) {
                        //getClientWidget()->setEnabled(true);
                    }
                    float speed_dir_h = m_move_speed.left < 0 ? -1.0f : 1.0f;
                    m_move_speed.left = fabs(m_move_speed.left);
                    if (m_move_speed.left>max_speed)
                        m_move_speed.left = max_speed;
                    
                    float speed_dir_v = m_move_speed.top < 0 ? -1.0f : 1.0f;
                    m_move_speed.top = fabs(m_move_speed.top);
                    if (m_move_speed.top>max_speed)
                        m_move_speed.top = max_speed;
                    
                    int scroll_distance_h = m_move_speed.left * ( 0.2f ) * speed_dir_h;
                    int scroll_distance_v = m_move_speed.top * ( 0.2f ) * speed_dir_v;
                    
                    m_scroll_target = getViewOffset() + MyGUI::IntPoint(scroll_distance_h,scroll_distance_v);
                    m_scroll_target = normalizeScrollValue(m_scroll_target);
                    
                    m_state = state_free_scroll;
                    //LogInfo() << "set animate scroll";
                } else {
                    //m_state = state_none;
                }
            }
        }
        
         MyGUI::IntPoint ScrollArea::normalizeScrollValue(const MyGUI::IntPoint& val_) const {
            
            MyGUI::IntPoint val(val_);
            
            MyGUI::IntSize contentSize = getContentSize();
            MyGUI::IntSize viewSize = getViewSize();
             
            MyGUI::IntSize max_scroll = contentSize - viewSize;
            if (val.left>0)
                val.left = 0;
            if (val.left<-max_scroll.width) {
                val.left = -max_scroll.width;
            }
             
             if (val.top>0)
                 val.top = 0;
             if (val.top<-max_scroll.height) {
                 val.top = -max_scroll.height;
             }
            return val;
        }
        
        
        void ScrollArea::setScrollPos(const MyGUI::IntPoint& p) {
            m_scroll_target = normalizeScrollValue(p);
            setViewOffset(m_scroll_target);
        }
        
        
        
    }
}
