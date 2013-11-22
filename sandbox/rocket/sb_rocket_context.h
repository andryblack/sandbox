//
//  sb_rocket_context.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_rocket_context__
#define __sr_osx__sb_rocket_context__

#include <Rocket/Core/Context.h>
#include "sbstd/sb_shared_ptr.h"
#include "sbstd/sb_string.h"
#include "sbstd/sb_map.h"
#include "sb_font.h"
#include <ghl_keys.h>

namespace Sandbox {
    
    class Resources;
    class RocketRenderInterface;
    class Graphics;
    class RocketFontFaceHandle;
    class LuaVM;
    class RocketDocument;
    
    typedef sb::shared_ptr<RocketDocument> RocketDocumentPtr;
    
    class RocketContext : public ::Rocket::Core::Context {
    private:
        RocketRenderInterface*  m_render;
        LuaVM*  m_vm;
        typedef sb::map<sb::string,RocketFontFaceHandle*> fonts_map;
        fonts_map   m_fonts;
    public:
        ~RocketContext();
        explicit RocketContext(Resources* res,LuaVM* vm);
        
        void Draw(Graphics& g);
        RocketDocumentPtr   LoadRocketDocument(const sb::string& file);
        virtual ::Rocket::Core::FontFaceHandle* GetFontFaceHandle(const ::Rocket::Core::String& family, const ::Rocket::Core::String& charset, ::Rocket::Core::Font::Style style, ::Rocket::Core::Font::Weight weight, int size);
        virtual ::Rocket::Core::EventListener* InstanceEventListener(const ::Rocket::Core::String& value, ::Rocket::Core::Element* element);
        void RegisterFont(const sb::string& name,int size,bool italic,bool bold,const FontPtr& fnt);
        
        void OnMouseDown( GHL::MouseButton key, int x,int y, GHL::UInt32 mods);
        void OnMouseMove( int x,int y, GHL::UInt32 mods );
        void OnMouseUp( GHL::MouseButton key, int x,int y, GHL::UInt32 mods);
        
        LuaVM* GetLuaVM() { return m_vm; }
    };
    
}

#endif /* defined(__sr_osx__sb_rocket_context__) */
