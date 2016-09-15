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
        
        class ObjectSubSkin : public MyGUI::ISubWidgetRect {
            MYGUI_RTTI_DERIVED( ObjectSubSkin )
        public:
            ObjectSubSkin();
            virtual void createDrawItem(MyGUI::ITexture* _texture, MyGUI::ILayerNode* _node);
            virtual void destroyDrawItem();
            virtual void doRender(MyGUI::IRenderTarget*);
            virtual void _setColour(const MyGUI::Colour& _value) { m_colour = _value; }
            virtual void setAlpha(float _alpha) { m_alpha = _alpha; }
        private:
            MyGUI::Colour   m_colour;
            float m_alpha;
            MyGUI::ILayerNode* mNode;
            MyGUI::RenderItem* mRenderItem;
        };
        
        class SceneObjectWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( SceneObjectWidget )
        public:
            SceneObjectWidget();
            ~SceneObjectWidget();
            
            void setObject(const SceneObjectPtr& object);
            const SceneObjectPtr& getObject() const { return m_object; }
        private:
            void initialiseOverride();
            void shutdownOverride();
            SceneObjectPtr  m_object;
            void handleFrame(float dt);
        };
    }
}

#endif /* defined(__Sandbox__sb_mygui_scene_object__) */
