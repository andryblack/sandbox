//
//  sb_mygui_cached_widget.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 30/01/14.
//
//

#ifndef __Sandbox__sb_mygui_cached_widget__
#define __Sandbox__sb_mygui_cached_widget__

#include "MyGUI_Widget.h"
#include "meta/sb_meta.h"
#include "sb_vector2.h"
#include "../sb_mygui_animated_layer.h"

namespace Sandbox {
    
    namespace mygui {
        class RenderTargetImpl;
        
        class ReplacedLayer : public MyGUI::ILayer {
        private:
            MyGUI::LayerNode    m_root;
            MyGUI::IntSize      m_view_size;
        public:
            ReplacedLayer();
            
            MyGUI::LayerNode* getRoot() { return &m_root; }
            
            virtual MyGUI::ILayerNode* createRootItemNode(MyGUI::Widget* _item);
            virtual void destroyRootItemNode(MyGUI::ILayerNode* _node);
            
            // up child item (make it draw and pick above other)
            virtual void upRootItemNode(MyGUI::ILayerNode* _node);
            
            // child items list
            virtual size_t getLayerNodeCount() const;
            
            virtual MyGUI::ILayerNode* getLayerNodeAt(size_t _index) const;
            
            // return widget at position
            virtual MyGUI::ILayerItem* getLayerItemByPoint(int _left, int _top) const;
            virtual MyGUI::ILayerItem* checkLayerItemByPoint(const MyGUI::ILayerItem* _target, int _left, int _top) const;
            
            
            // return position in layer coordinates
            virtual MyGUI::FloatPoint getPosition(float _left, float _top) const;
            
            // return layer size
            virtual const MyGUI::IntSize& getSize() const;
            
            // render layer
            virtual void renderToTarget(MyGUI::IRenderTarget* _target, bool _update);
            
            virtual void resizeView(const MyGUI::IntSize& _viewSize);
        };
        
        class CachedWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( CachedWidget )
        public:
            CachedWidget();
            ~CachedWidget();
            
            virtual void onWidgetCreated(MyGUI::Widget* _widget);
            
            virtual void addChildItem(LayerItem* _item);
            virtual void removeChildItem(LayerItem* _item);
            virtual void addChildNode(LayerItem* _item);
            virtual void removeChildNode(LayerItem* _item);

            void setSubOffset(const Sandbox::Vector2f& sub) {
                m_suboffset = sub;
            }
            void setRenderContent(bool r) { m_render_content = r; }
            
        protected:
            void initialiseOverride();
            void shutdownOverride();
            virtual void setPropertyOverride(const std::string& _key, const std::string& _value);
            virtual RenderTargetImpl* renderToTarget(bool update_);
            virtual void doRenderToTarget(MyGUI::IRenderTarget* rt);
            void recursiveRenderChilds(MyGUI::Widget* ch);
        private:
            RenderTargetImpl*        m_target;
            void releaseTarget();
            
            ReplacedLayer*      m_replaced_layer;
            void frameEntered(float dt);
            
            bool    m_render_content;
            Sandbox::Vector2f m_suboffset;
        };
        
    }
    
}

#endif /* defined(__Sandbox__sb_mygui_cached_widget__) */
