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
#include "sb_scene.h"
#include "sb_mygui_cached_widget.h"

namespace Sandbox {
    
    namespace mygui {
        
        class SceneWidget : public CachedWidget {
            MYGUI_RTTI_DERIVED( SceneWidget )
        public:
            SceneWidget();
            ~SceneWidget();
            
            const ScenePtr& getScene() const { return m_scene; }
            void setScene( const ScenePtr& scene ) { m_scene = scene; }
        protected:
            virtual void doRenderToTarget(MyGUI::IRenderTarget* rt);
        private:
            ScenePtr    m_scene;
        };
    }
}

#endif /* defined(__Sandbox__sb_mygui_scene_widget__) */
