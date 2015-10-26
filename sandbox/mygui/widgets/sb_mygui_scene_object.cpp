#include "sb_mygui_scene_object.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "MyGUI_RenderItem.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::ObjectSubSkin,MyGUI::SubSkin)
SB_META_DECLARE_OBJECT(Sandbox::mygui::SceneObjectWidget,MyGUI::Widget)


namespace Sandbox {
    namespace mygui {
        
        
        ObjectSubSkin::ObjectSubSkin() {
            mSeparate = true;
        }
        
        
        void ObjectSubSkin::doManualRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count) {
            if (mRenderItem && mCroppedParent) {
                MyGUI::IRenderTarget* _target = mRenderItem->getRenderTarget();
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
                    
                    target->startRenderObject();
                    target->graphics()->SetColor(Sandbox::Color(GHL::UInt32(mCurrentColour)));
                    target->graphics()->SetTransform(Sandbox::Transform2d().translated(x, y));
                    object->Draw(*target->graphics());
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