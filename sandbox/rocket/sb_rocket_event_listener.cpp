//
//  sb_rocket_event_listener.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/13/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_rocket_event_listener.h"
#include "sb_lua_context.h"

namespace Sandbox {
    
    /// Process the incoming Event
    void RocketEventListener::ProcessEvent(Rocket::Core::Event& event) {
        luabind::LuaVMHelperPtr h = m_context->GetHelper();
        m_context->SetValue("event", &event);
        m_function->Execute();
        m_context->SetValue("event", (void*)0);
    }
    
    /// Called when the listener has been attached to a new Element
    void RocketEventListener::OnAttach(Rocket::Core::Element* element) {
        m_context->SetValue("this", element);
    }
    
    /// Called when the listener has been detached from a Element
    void RocketEventListener::OnDetach(Rocket::Core::Element* element) {
        m_context->SetValue("this", (void*)0);
    }
}
