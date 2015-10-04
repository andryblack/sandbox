//
//  sb_mygui_scene_widget.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 02/02/14.
//
//

#ifndef __Sandbox__sb_mygui_scene_widget__
#define __Sandbox__sb_mygui_scene_widget__

#include "meta/sb_meta.h"
#include "sb_rt_scene.h"
#include "sb_mygui_cached_widget.h"

namespace Sandbox {
    
    namespace mygui {
        
        class SceneWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( SceneWidget )
        public:
            SceneWidget();
            ~SceneWidget();
            
            void initialiseOverride();
            
            const RTScenePtr& getScene() const { return m_scene; }
            void setScene( const RTScenePtr& scene );
        private:
            MyGUI::ITexture*    m_texture;
            RTScenePtr    m_scene;
        };
    }
}

#endif /* defined(__Sandbox__sb_mygui_scene_widget__) */
