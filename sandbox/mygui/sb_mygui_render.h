//
//  sb_mygui_render.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 25/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_mygui_render__
#define __sr_osx__sb_mygui_render__

#include "MyGUI_RenderManager.h"
#include "MyGUI_IRenderTarget.h"
#include <sbstd/sb_list.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_string.h>

#include "sb_scene.h"
#include <ghl_render.h>
#include "sb_data.h"
#include "sb_lua_context.h"
#include "sb_image.h"
#include "sb_font.h"

namespace GHL {
    struct Render;
}

namespace Sandbox {
    
    class Resources;
    class Graphics;
    class Texture;
    typedef sb::intrusive_ptr<Texture> TexturePtr;
    
    class RenderTarget;
    typedef sb::intrusive_ptr<RenderTarget> RenderTargetPtr;
    
    
    namespace mygui {
   
        
        class TextureImpl;
        class WidgetRender;
        typedef sb::intrusive_ptr<WidgetRender> WidgetRenderPtr;
        
        class RenderTargetImpl : public MyGUI::IRenderTarget {
        public:
            virtual void begin();
            virtual void end();
            
            virtual void setTexture(MyGUI::ITexture* _texture) ;
            virtual void addVertex(const MyGUI::Vertex& v) ;
            virtual void addQuad(const MyGUI::VertexQuad& q);
            
            void    drawScene(const ScenePtr& scene);
            
            Resources* resources() { return m_resources; }
            Graphics* graphics() { return m_graphics; }
            
            TextureImpl* getTexture() { return m_texture; }
            
            void SetRT( const RenderTargetPtr& rt );
            RenderTargetImpl( Graphics* graphics, Resources* resources);
            RenderTargetImpl( Graphics* graphics, Resources* resources ,
                             RenderTargetPtr rt, const MyGUI::IntSize& rt_size);
            ~RenderTargetImpl();
            
            const MyGUI::IntSize& getSize() const { return m_rendertarget_size; }
            int getWidth() const { return m_rendertarget_size.width;}
            int getHeight() const { return m_rendertarget_size.height; }
            
            void resize( const MyGUI::IntSize& size);
        protected:
            Graphics*       m_graphics;
            Resources*      m_resources;
            TextureImpl*    m_texture;
            MyGUI::IntSize  m_rendertarget_size;
            void setSize(int width,int height);
            RenderTargetPtr m_target;
        };
        
        class TextureImpl : public MyGUI::ITexture {
        public:
            TextureImpl( const sb::string& name,const TexturePtr& tex);
           
            virtual const std::string& getName() const { return m_name; }
           
            virtual void destroy();

            virtual int getWidth();
            virtual int getHeight();
            
            const TexturePtr& GetTexture() { return m_texture; }
            void SetTexture(const TexturePtr& tex) { m_texture = tex; }
        private:
            sb::string  m_name;
            TexturePtr  m_texture;
        };
        
        class RenderManager : public MyGUI::RenderManager, public RenderTargetImpl {
        public:
            explicit RenderManager( Graphics* graphics, Resources* resources );
            
            ImagePtr getImage(const std::string& name);
            void setContext(const LuaContextPtr& ctx );
            const Sandbox::LuaContextPtr& getContext() const { return m_context; }
            
            static RenderManager& getInstance() { return static_cast<RenderManager&>(MyGUI::RenderManager::getInstance()); }
            static RenderManager* getInstancePtr() { return static_cast<RenderManager*>(MyGUI::RenderManager::getInstancePtr()); }
            
            void reshape(float w,float h);
            
            /** Get texture by name */
            virtual MyGUI::ITexture* getTexture(const std::string& _name);
            /** Destroy texture */
            virtual void destroyTexture(MyGUI::ITexture* _texture);
            /** Get texture by name */
            virtual MyGUI::ITexture* loadTexture(const std::string& _filename);
            
            //FIXME возможно перенести в структуру о рендер таргете
            virtual const MyGUI::IntSize& getViewSize() const;
            
#if MYGUI_DEBUG_MODE == 1
            /** Check if texture is valid */
            virtual bool checkTexture(MyGUI::ITexture* _texture);
#endif
            
            
            void    timerFrame(float dt);
            void    drawFrame();
            
            virtual float getDisplayScale() const;
            
            void setRTScale(float s);
            float getRTScale() const { return m_scale; }
            
            RenderTargetImpl* createTarget(MyGUI::IntSize size);
            
            MyGUI::ITexture* wrapTexture( const TexturePtr& texture );
            RenderTargetImpl* wrapTarget(const RenderTargetPtr& rt );
            
            WidgetRenderPtr renderLayout( const MyGUI::IntSize& size, const sb::string& layout );
            FontPtr getFont( const sb::string& _name );
        private:
            sb::map<sb::string,TextureImpl*>  m_textures;
            Sandbox::LuaContextPtr  m_context;
            float   m_scale;
        };
        
    }
    
}

#endif /* defined(__sr_osx__sb_mygui_render__) */
