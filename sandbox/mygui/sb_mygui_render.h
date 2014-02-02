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

namespace GHL {
    struct Render;
}

namespace Sandbox {
    
    class Resources;
    class Graphics;
    
    namespace mygui {
        
        class RenderTargetImpl : public MyGUI::IRenderTarget {
        public:
            virtual void begin();
            virtual void end();
            
            virtual void doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count);
            
            virtual const MyGUI::RenderTargetInfo& getInfo();
            
            void    drawScene(const ScenePtr& scene);
            
        protected:
            RenderTargetImpl( Graphics* graphics, Resources* resources );
            Graphics*       m_graphics;
            Resources*      m_resources;
            MyGUI::RenderTargetInfo m_rt_info;
            MyGUI::IntSize  m_rendertarget_size;
            void setSize(int width,int height);
            GHL::Render*    m_render;
        };
        class RenderManager : public MyGUI::RenderManager, public RenderTargetImpl {
        public:
            explicit RenderManager( Graphics* graphics, Resources* resources );
            /** Create vertex buffer.
             This method should create vertex buffer with triangles list type,
             each vertex have position, colour, texture coordinates.
             */
            virtual MyGUI::IVertexBuffer* createVertexBuffer();
            /** Destroy vertex buffer */
            virtual void destroyVertexBuffer(MyGUI::IVertexBuffer* _buffer);
            
            /** Create empty texture instance */
            virtual MyGUI::ITexture* createTexture(const std::string& _name);
            /** Destroy texture */
            virtual void destroyTexture(MyGUI::ITexture* _texture);
            /** Get texture by name */
            virtual MyGUI::ITexture* getTexture(const std::string& _name);
            
            //FIXME возможно перенести в структуру о рендер таргете
            virtual const MyGUI::IntSize& getViewSize() const;
            
            /** Get current vertex colour type */
            virtual MyGUI::VertexColourType getVertexFormat();
            
            /** Check if texture format supported by hardware */
            virtual bool isFormatSupported(MyGUI::PixelFormat _format, MyGUI::TextureUsage _usage);
            
#if MYGUI_DEBUG_MODE == 1
            /** Check if texture is valid */
            virtual bool checkTexture(MyGUI::ITexture* _texture);
#endif
            
            
            void    timerFrame(float dt);
            void    drawFrame();
            
            class Texture;
        private:
            sb::map<sb::string,Texture*>  m_textures;
        };
        
    }
    
}

#endif /* defined(__sr_osx__sb_mygui_render__) */
