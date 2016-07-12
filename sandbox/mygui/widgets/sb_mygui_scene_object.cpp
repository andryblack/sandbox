#include "sb_mygui_scene_object.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "MyGUI_RenderItem.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::ObjectSubSkin,MyGUI::ISubWidgetRect)
SB_META_DECLARE_OBJECT(Sandbox::mygui::SceneObjectWidget,MyGUI::Widget)


namespace Sandbox {
    namespace mygui {
        
        
        ObjectSubSkin::ObjectSubSkin() : mNode(0),mRenderItem(0){
            //LogInfo() << "ObjectSubSkin::ObjectSubSkin";
        }
        
        void ObjectSubSkin::createDrawItem( MyGUI::ITexture* _texture, MyGUI::ILayerNode* _node) {
            //LogInfo() << "ObjectSubSkin::createDrawItem";
            mNode = _node;
            mRenderItem = mNode->addToRenderItem(this, true, true);
            mRenderItem->addDrawItem(this);
        }
        void ObjectSubSkin::destroyDrawItem() {
            //LogInfo() << "ObjectSubSkin::destroyDrawItem";
            mNode = nullptr;
            mRenderItem->removeDrawItem(this);
            mRenderItem = nullptr;
        }
        
        
        void ObjectSubSkin::doRender(MyGUI::IRenderTarget* _target) {
            if (!mVisible)
                return;
            if (mCroppedParent) {
                if (_target) {
                    RenderTargetImpl* target = static_cast<RenderTargetImpl*>(_target);
                    MyGUI::Widget* widget_p = static_cast<MyGUI::Widget*>(mCroppedParent);
                    SceneObjectWidget* widget = widget_p->castType<SceneObjectWidget>(false);
                    if (!widget)
                        return;
                    
                    SceneObjectPtr object = widget->getObject();
                    if (!object)
                        return;
                    int x = mCroppedParent->getAbsoluteLeft();
                    int y = mCroppedParent->getAbsoluteTop();
                    
                    const MyGUI::RenderTargetInfo& info = _target->getInfo();
                    
                    x-=info.leftOffset;
                    y-=info.topOffset;
                  
                    Transform2d tr = target->graphics()->GetTransform();
                    Color c = target->graphics()->GetColor();
                    target->graphics()->SetColor(c*Sandbox::Color(m_colour.red,m_colour.green,m_colour.blue,m_colour.alpha));
                    target->graphics()->SetTransform(tr.translated(x, y));
                    object->Draw(*target->graphics());
                    target->graphics()->SetColor(c);
                    target->graphics()->SetTransform(tr);
                }
            }
        }
        
        
        SceneObjectWidget::SceneObjectWidget() {
            
        }
        
        SceneObjectWidget::~SceneObjectWidget() {
            
        }
        
        void SceneObjectWidget::setObject(const SceneObjectPtr &object) {
            m_object = object;
            _updateView();
        }
        
    }
}