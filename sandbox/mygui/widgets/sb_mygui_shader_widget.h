#ifndef _SB_MYGUI_SHADER_WIDGET_H_INCLUDED_
#define _SB_MYGUI_SHADER_WIDGET_H_INCLUDED_

#include "sb_mygui_animated_widget.h"
#include "sb_shader.h"

namespace Sandbox {
    
    namespace mygui {
        
        class ShaderWidget : public AnimatedWidget {
            MYGUI_RTTI_DERIVED( ShaderWidget )
        public:
            virtual void DrawContent(MyGUI::IRenderTarget* rt,MyGUI::LayerNode* node,bool update);
            void setPropertyOverride(const std::string& _key, const std::string& _value);
            virtual void setShader( const ShaderPtr& sh ) { m_shader = sh; }
            const ShaderPtr& getShader() const { return m_shader; }
        protected:
            ShaderPtr   m_shader;
        };
    }
    
}

#endif /*_SB_MYGUI_SHADER_WIDGET_H_INCLUDED_*/
