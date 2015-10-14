//
//  sb_mygui_scene_object.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 02/02/14.
//
//

#ifndef __Sandbox__sb_mygui_scene_object__
#define __Sandbox__sb_mygui_scene_object__

#include "meta/sb_meta.h"
#include "sb_scene_object.h"
#include "MyGUI_Widget.h"
#include "MyGUI_SubSkin.h"

namespace Sandbox {
    
    namespace mygui {
        
        class ObjectSubSkin : public MyGUI::SubSkin {
            MYGUI_RTTI_DERIVED( ObjectSubSkin )
        public:
            ObjectSubSkin();
            virtual void doManualRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count);
        private:
        };
        
        class SceneObjectWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( SceneObjectWidget )
        public:
            SceneObjectWidget();
            ~SceneObjectWidget();
            
            void setObject(const SceneObjectPtr& object) { m_object = object; }
            const SceneObjectPtr& getObject() const { return m_object; }
        private:
            SceneObjectPtr  m_object;
        };
    }
}

#endif /* defined(__Sandbox__sb_mygui_scene_object__) */
