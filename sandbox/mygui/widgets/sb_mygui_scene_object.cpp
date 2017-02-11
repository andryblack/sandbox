#include "sb_mygui_scene_object.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "MyGUI_RenderItem.h"
#include "sb_graphics.h"
#include "MyGUI_Gui.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::ObjectSubSkin,MyGUI::ISubWidgetRect)
SB_META_DECLARE_OBJECT(Sandbox::mygui::SceneObjectWidget,Sandbox::mygui::AnimatedWidget)


namespace Sandbox {
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(ObjectSubSkin)
        
        ObjectSubSkin::ObjectSubSkin() : mNode(0),mRenderItem(0),m_alpha(1.0f){
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
        
        void ObjectSubSkin::_correctView() {
            if (nullptr != mNode)
                mNode->outOfDate(mRenderItem);
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
                    
                    Transform2d tr = target->graphics()->GetTransform();
                    Color c = target->graphics()->GetColor();
                    target->graphics()->SetColor(c*Sandbox::Color(m_colour.red,m_colour.green,m_colour.blue,m_colour.alpha*m_alpha));
                    target->graphics()->SetTransform(tr.translated(x, y));
                    object->Draw(*target->graphics());
                    target->graphics()->SetColor(c);
                    target->graphics()->SetTransform(tr);
                }
            }
        }
        
        MYGUI_IMPL_TYPE_NAME(SceneObjectWidget)
        
        SceneObjectWidget::SceneObjectWidget() {
            
        }
        
        SceneObjectWidget::~SceneObjectWidget() {
            
        }
        
        void SceneObjectWidget::initialiseOverride() {
            Base::initialiseOverride();
            MyGUI::Gui::getInstance().eventFrameStart += MyGUI::newDelegate( this, &SceneObjectWidget::handleFrame );
        }
        
        void SceneObjectWidget::shutdownOverride() {
            Base::shutdownOverride();
            MyGUI::Gui::getInstance().eventFrameStart -= MyGUI::newDelegate( this, &SceneObjectWidget::handleFrame );
        }

        void SceneObjectWidget::handleFrame(float dt) {
            if (m_object) {
                m_object->Update(dt);
            }
        }
        
        void SceneObjectWidget::setObject(const SceneObjectPtr &object) {
            m_object = object;
            _updateView();
        }
        
    }
}
