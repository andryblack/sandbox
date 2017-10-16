

#include "sb_mygui_animated_widget.h"
#include "MyGUI_ITexture.h"
#include "MyGUI_RenderManager.h"
#include "sb_math.h"
#include "MyGUI_Gui.h"
#include "MyGUI_LayerNode.h"
#include "../sb_mygui_render.h"
#include <sbstd/sb_platform.h>
#include "sb_graphics.h"
#include "../sb_mygui_animated_layer.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedWidget, MyGUI::Widget)

namespace Sandbox {
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(AnimatedWidget)
        
        AnimatedWidget::AnimatedWidget()  {
            m_thread.reset(new ThreadsMgr());
            setUserString("animated", "true");
        }
        
        AnimatedWidget::~AnimatedWidget() {
        }
        
        const TransformModificatorPtr& AnimatedWidget::GetTransform() const {
            if (!m_transform) {
                m_transform.reset(new TransformModificator());
            }
            return m_transform;
        }
        
        const ColorModificatorPtr& AnimatedWidget::GetColor() const {
            if (!m_color) {
                m_color.reset(new ColorModificator());
            }
            return m_color;
        }
        
        void AnimatedWidget::SetOrigin(const Sandbox::Vector2f &o) {
            m_origin = o;
            if (m_transform) {
                m_transform->SetOrigin(m_origin);
            }
        }
        void AnimatedWidget::initialiseOverride() {
            Base::initialiseOverride();
            MyGUI::Gui::getInstance().eventFrameStart += MyGUI::newDelegate( this, &AnimatedWidget::Update );
        }
        
        void AnimatedWidget::shutdownOverride() {
            Base::shutdownOverride();
            MyGUI::Gui::getInstance().eventFrameStart -= MyGUI::newDelegate( this, &AnimatedWidget::Update );
        }
        
        void AnimatedWidget::detachFromLayer() {
            Base::detachFromLayer();
        }

        
        bool AnimatedWidget::renderNodeToTarget(MyGUI::IRenderTarget* rt,MyGUI::LayerNode* node,bool update) {
            Graphics& g = *static_cast<RenderTargetImpl*>(rt)->graphics();
            Transform2d tr = g.GetTransform();
            Color clr = g.GetColor();
            if (m_transform) {
                Vector2f origin = Vector2f(getAbsoluteLeft(),getAbsoluteTop())+m_origin;
                m_transform->SetOrigin(origin);
                m_transform->Apply(g);
            }
            if (m_color) {
                m_color->Apply(g);
            }
            DrawContent(rt,node,update);

            g.SetColor(clr);
            g.SetTransform(tr);
            return true;
        }
   
        void AnimatedWidget::DrawContent(MyGUI::IRenderTarget* rt,MyGUI::LayerNode* node,bool update) {
            node->LayerNode::renderToTarget(rt, update);
        }
        
        void AnimatedWidget::Update(float dt) {
            if (!m_thread->Update(dt)) {
                setNodeOutOfDate();
            }
        }
        
        MyGUI::ILayerNode* AnimatedWidget::createChildItemNode(MyGUI::ILayerNode* _node) {
            AnimatedLayerNode* node = new AnimatedLayerNode(_node,this);
            _node->addChildItemNode(node);
            return node;
        }
        
        void AnimatedWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "Origin") {
                MyGUI::IntPoint p = MyGUI::utility::parseValue<MyGUI::IntPoint>(_value);
                SetOrigin(Vector2f(p.left,p.top));
            } else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
                
    }
    
}
