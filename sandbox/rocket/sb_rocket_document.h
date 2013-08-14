//
//  sb_rocket_document.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/13/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_rocket_document__
#define __sr_osx__sb_rocket_document__

#include <Rocket/Core/ElementDocument.h>
#include "sb_lua_context.h"

namespace Sandbox {
    
    class LuaVM;
    
    class RocketDocument : public Rocket::Core::ElementDocument {
    private:
        LuaContextPtr   m_context;
    public:
        RocketDocument(const Rocket::Core::String& tag,Rocket::Core::Context* context);
        virtual void LoadScript(Rocket::Core::Stream* stream, const Rocket::Core::String& source_name);
        const LuaContextPtr& GetLuaContext() { return m_context; }
    };
    
}

#endif /* defined(__sr_osx__sb_rocket_document__) */
