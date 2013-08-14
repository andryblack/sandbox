//
//  sb_rocket_event_listener.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/13/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_rocket_event_listener__
#define __sr_osx__sb_rocket_event_listener__

#include <Rocket/Core/EventListener.h>
#include "sb_lua_function.h"

namespace Sandbox {
    
    class RocketEventListener : public Rocket::Core::EventListener {
    private:
        LuaContextPtr   m_context;
        LuaFunctionPtr  m_function;
    public:
        RocketEventListener( const LuaContextPtr& ctx, const LuaFunctionPtr& func) : m_context(ctx),m_function(func) {}
        /// Process the incoming Event
        virtual void ProcessEvent(Rocket::Core::Event& event);
        
        /// Called when the listener has been attached to a new Element
        virtual void OnAttach(Rocket::Core::Element* element);
        
        /// Called when the listener has been detached from a Element
        virtual void OnDetach(Rocket::Core::Element* element);
    };
    
}

#endif /* defined(__sr_osx__sb_rocket_event_listener__) */
