#include "sb_mygui_shader_widget.h"
#include "sb_mygui_render.h"
#include "sb_resources.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::ShaderWidget, Sandbox::mygui::AnimatedWidget)

namespace Sandbox {
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(ShaderWidget)
    
        void ShaderWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "Shader") {
                size_t delim_pos = _value.find(';');
                if (delim_pos!=_value.npos) {
                    sb::string vert = _value.substr(0,delim_pos);
                    sb::string frag = _value.substr(delim_pos+1);
                    setShader(RenderManager::getInstance().resources()->GetShader(vert.c_str(), frag.c_str()));
                }
            }
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        
        void ShaderWidget::DrawContent(MyGUI::IRenderTarget* rt,MyGUI::LayerNode* node,bool update) {
            if (m_shader) {
                Graphics& g = *static_cast<RenderTargetImpl*>(rt)->graphics();
                ShaderPtr old = g.GetShader();
                g.SetShader(m_shader);
                Base::DrawContent(rt,node,update);
                g.SetShader(old);
            } else {
                Base::DrawContent(rt,node,update);
            }
            
        }


    }
    
}
