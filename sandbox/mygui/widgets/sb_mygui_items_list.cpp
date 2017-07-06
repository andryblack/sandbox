
#include "sb_mygui_items_list.h"
#include "luabind/sb_luabind.h"
#include "sb_lua_context.h"
#include "sb_lua_value.h"
#include "sb_log.h"


SB_META_DECLARE_OBJECT(Sandbox::mygui::ItemsListDelegate, Sandbox::mygui::ScrollListDelegate)
SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ItemsListDelegate)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    namespace mygui {
        
        int ItemsListDelegate::getAllItemsSize() {
            int res = 0;
            size_t cnt = getItemsCount();
            for (size_t i=0;i<cnt;++i) {
                res += getItemSize(i);
            }
            return res;
        }
        
        class LuaItemsListDelegate : public ItemsListDelegate {
        private:
            LuaContext m_obj;
        public:
            LuaItemsListDelegate(lua_State* L, int idx) {
                lua_pushvalue(L, idx);
                m_obj.SetObject(L);
            }
            virtual size_t getItemsCount() {
                if (!m_obj.Valid())
                    return 0;
                LuaValuePtr items = m_obj.GetValueRaw<LuaValuePtr>("items");
                if (!items) return 0;
                return items->GetSize();
            }
            LuaContextPtr getItemData(size_t idx) {
                LuaValuePtr items = m_obj.GetValueRaw<LuaValuePtr>("items");
                if (!items) return LuaContextPtr();
                LuaValuePtr v = items->GetAt(idx+1);
                if (!v) return LuaContextPtr();
                return v->GetAs<LuaContextPtr>();
            }
            virtual void createWidget(MyGUI::Widget* w,size_t idx) {
                LuaContextPtr data = getItemData(idx);
                data->call_self("createWidget",w);
                w->_setInternalData(data->GetValueRaw<int>("widgetType"));
            }
            virtual bool canReuseWidget(MyGUI::Widget* w,size_t idx) {
                LuaContextPtr data = getItemData(idx);
                int* original = w->_getInternalData<int>(false);
                return original && (*original) == data->GetValueRaw<int>("widgetType");
            }

            virtual void updateWidget(MyGUI::Widget* w, const MyGUI::IBDrawItemInfo& di,bool changed) {
                LuaContextPtr data_ptr = getItemData(di.index);
                data_ptr->call_self("updateWidget",w,di,changed);
            }
            virtual void onItemClick(size_t idx) {
                LuaContextPtr data_ptr = getItemData(idx);
                if (data_ptr && data_ptr->GetValueRaw<bool>("onItemClick")) {
                    data_ptr->call_self("onItemClick",data_ptr,idx);
                }
            }
            virtual void onItemPressed(size_t idx) {
                LuaContextPtr data_ptr = getItemData(idx);
                if (data_ptr && data_ptr->GetValueRaw<bool>("onItemPressed")) {
                    data_ptr->call_self("onItemPressed",data_ptr,idx);
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
            
            
            virtual int getAllItemsSize() {
                LuaValuePtr items = m_obj.GetValueRaw<LuaValuePtr>("items");
                if (!items) return 0;
                size_t cnt = items->GetSize();
                int res = 0;
                for (size_t i=0;i<cnt;++i) {
                    LuaValuePtr v = items->GetAt(i+1);
                    if (!v) continue;
                    LuaContextPtr item = v->GetAs<LuaContextPtr>();
                    if (!item) continue;
                    int size = item->GetValueRaw<int>("size");
                    item->SetValue("offset", res);
                    res += size;
                }
                return res;
            }
            virtual int getItemSize(size_t idx) {
                LuaContextPtr data = getItemData(idx);
                if (data) {
                    return data->GetValueRaw<int>("size");
                }
                return 0;
            }
            virtual void alignItem(size_t idx,int& offset,int& size) {
                LuaContextPtr data = getItemData(idx);
                offset = data->GetValueRaw<int>("offset");
                if (!offset) {
                    getAllItemsSize();
                    offset = data->GetValueRaw<int>("offset");
                }
                size = data->GetValueRaw<int>("size");
            }
            virtual void getVisibleItems(int from,int to,int& first,int& last) {
                LuaValuePtr items = m_obj.GetValueRaw<LuaValuePtr>("items");
                if (!items) return;
                size_t cnt = items->GetSize();
                first = last = 0;
                int res = 0;
                for (size_t i=0;i<cnt;++i) {
                    LuaValuePtr v = items->GetAt(i+1);
                    if (!v) continue;
                    LuaContextPtr item = v->GetAs<LuaContextPtr>();
                    if (!item) continue;
                    int size = item->GetValueRaw<int>("size");
                    if (res < from)
                        first = i;
                    last = i;
                    item->SetValue("offset", res);
                    if (res>to) {
                        break;
                    }
                    res += size;
                }
            }
            
        };
    }
}

static int setDelegateImpl(lua_State* L) {
    using namespace Sandbox::mygui;
    ItemsList* sl = Sandbox::luabind::stack<ItemsList*>::get(L, 1);
    if (!sl) return 0;
    if (lua_istable(L, 2)) {
        ItemsListDelegate* delegate = new LuaItemsListDelegate(L,2);
        sl->setDelegate(ItemsListDelegatePtr(delegate));
    } else {
        sl->setDelegate(Sandbox::luabind::stack<ItemsListDelegatePtr>::get(L, 2));
    }
    return 0;
}

SB_META_DECLARE_OBJECT(Sandbox::mygui::ItemsList, Sandbox::mygui::ScrollListBase)
SB_META_BEGIN_KLASS_BIND(Sandbox::mygui::ItemsList)
bind( property_wo( "delegate", &setDelegateImpl ) );
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(ItemsList)
        
        void register_ItemsList(lua_State* L) {
            luabind::Class<ItemsListDelegate>(L);
            luabind::ExternClass<ItemsList>(L);
        }
        
        
        
        ItemsList::ItemsList() {
        }
        
        ItemsList::~ItemsList() {
        }
        
        void ItemsList::setDelegate(const ItemsListDelegatePtr& d) {
            Base::setDelegate(d);
        }
        
        
        void ItemsList::alignWidget(MyGUI::Widget* w,size_t idx) const {
            int width;
            int height;
            
            int left, top;
            if (getVerticalAlignment()) {
                left = getContentMargins().left;
                getDelegate()->alignItem(idx,top,height);
                top += getContentMargins().top + getCenteredOffset();
                width = (getViewSize().width-getContentMargins().left-getContentMargins().right);
            } else {
                getDelegate()->alignItem(idx,left,width);
                left += getContentMargins().left + getCenteredOffset();
                top = getContentMargins().top;
                height = (getViewSize().height-getContentMargins().top-getContentMargins().bottom);
            }
            w->setPosition( left, top );
            w->setSize(width,height);
        }
        
        
        void ItemsList::updateContent() {
            
            if (m_delegate) {
                
                if (m_vertical) {
                    int height = getDelegate()->getAllItemsSize();
                    if( m_centered ) {
                        int required_height = getContentMargins().top+getContentMargins().bottom + height;
                        int client_height = getViewSize().height;
                        if( required_height < client_height ) {
                            setCenteredOffset((client_height - required_height)/2);
                        } else {
                            setCenteredOffset(0);
                        }
                    }
                    
                    updateRealSize(getViewSize().width,
                                         getCenteredOffset()+ height +
                                         getContentMargins().top +
                                         getContentMargins().bottom);
                } else {
                    int width = getDelegate()->getAllItemsSize();
                    if( m_centered ) {
                        int required_width = getContentMargins().left+getContentMargins().right + width;
                        int client_width = getViewSize().width;
                        if( required_width < client_width ) {
                            setCenteredOffset((client_width - required_width)/2);
                        } else {
                            setCenteredOffset(0);
                        }
                    }
                    updateRealSize(getCenteredOffset() + width +
                                         getContentMargins().left +
                                         getContentMargins().top,getViewSize().height);
                }
            }
        }
        
        void ItemsList::getVisibleItems(int& first, int& last) const {
            int top = getScroll() + getScrollMargin();
            int bottom = top + getScrollAreaSize();
            getDelegate()->getVisibleItems(top,bottom,first,last);
        }
        
        
        Vector2f ItemsList::Normalize(const Vector2f& v,bool soft) const {
            Vector2f res = Scroll::Normalize(v, soft);
            if (!soft && m_align_on_cell) {
                if (getVerticalAlignment()) {
//                    float max = Scroll::GetContentSize().h - Scroll::GetViewSize().h;
//                    int last_delta = 0;
//                    if (max > m_item_size) {
//                        last_delta = m_item_size / 4;
//                    }
//                    if (res.y >= ( max - last_delta)) {
//                        res.y = max;
//                    } else {
//                        res.y = m_content_margins.top + m_item_size * int((res.y - m_content_margins.top + m_item_size/2) / m_item_size);
//                    }
                } else {
//                    float max = Scroll::GetContentSize().w - Scroll::GetViewSize().w;
//                    int last_delta = 0;
//                    if (max > m_item_size) {
//                        last_delta = m_item_size / 4;
//                    }
//                    if (res.x >= ( max - last_delta)) {
//                        res.x = max;
//                    } else {
//                        res.x = m_content_margins.left + m_item_size * int((res.x - m_content_margins.left + m_item_size/2) / m_item_size);
//                    }
                }
            }
            return res;
        }
        
        
        
        
    }
}
