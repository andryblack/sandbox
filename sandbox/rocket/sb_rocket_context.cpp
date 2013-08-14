//
//  sb_rocket_context.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_rocket_context.h"
#include "sb_rocket_file.h"
#include "sb_rocket_render.h"
#include <Rocket/Controls/Controls.h>
#include <Rocket/Core/ElementDocument.h>
#include <Rocket/Core/Factory.h>
#include <Rocket/Core/ElementInstancerGeneric.h>
#include "sb_graphics.h"
#include "sb_rocket_font.h"
#include "sb_rocket_document.h"
#include <ghl_keys.h>
#include "sb_lua_function.h"
#include "sb_rocket_event_listener.h"

namespace Sandbox {
    
    template <class T>
	struct  RocketDestroyHelper {
		static void destroy( const void* ptr ) {
			if (!ptr) return;
			const T* cont_ptr = reinterpret_cast< const T* >( ptr );
			const_cast< T* >(cont_ptr)->RemoveReference();
		}
	};
    
    RocketContext::RocketContext(Resources* res,LuaVM* vm) : Rocket::Core::Context("main") {
        m_vm = vm;
        RocketFileInterface* fi = new RocketFileInterface(res);
        SetFileInterface(fi);
        m_render = new RocketRenderInterface(res);
        m_render->AddReference();
        SetRenderInterface(m_render);
        
        

        GetFactory()->RegisterElementInstancer("body", new Rocket::Core::ElementInstancerGeneric< RocketDocument >())->RemoveReference();
        
        Rocket::Controls::Initialise(this);
    }
    
    RocketContext::~RocketContext() {
        for (fonts_map::iterator it = m_fonts.begin();it!=m_fonts.end();++it) {
            it->second->RemoveReference();
        }
        ReleaseTextures();
        m_render->RemoveReference();
    }
    
    void RocketContext::Draw(Graphics& g) {
        SetDimensions(Rocket::Core::Vector2i(g.GetScreenWidth(),g.GetScreenHeight()));
        m_render->BeginDraw(g);
        Render();
        m_render->EndDraw(g);
    }
    
    RocketDocumentPtr   RocketContext::LoadRocketDocument(const sb::string& file) {
        Rocket::Core::ElementDocument* doc = this->Rocket::Core::Context::LoadDocument(file.c_str());
        RocketDocument* rdoc = reinterpret_cast<RocketDocument*>(doc);
        return RocketDocumentPtr(rdoc,&RocketDestroyHelper<RocketDocument>::destroy);
    }
    
    ::Rocket::Core::FontFaceHandle* RocketContext::GetFontFaceHandle(const ::Rocket::Core::String& family, const ::Rocket::Core::String& charset, ::Rocket::Core::Font::Style style, ::Rocket::Core::Font::Weight weight, int size) {
        sb::string name = family.CString();
        sb::string hname = name;
        
        fonts_map::iterator it = m_fonts.find(hname);
        if (it!=m_fonts.end()) {
            it->second->AddReference();
            return it->second;
        }
        
        return 0;
    }
    
    ::Rocket::Core::EventListener* RocketContext::InstanceEventListener(const ::Rocket::Core::String& value, ::Rocket::Core::Element* element) {
        if (!element) return 0;
        Rocket::Core::ElementDocument* doc = element->GetOwnerDocument();
        if (!doc) return 0;
        RocketDocument* rdoc = reinterpret_cast<RocketDocument*>(doc);
        LuaContextPtr ctx = rdoc->GetLuaContext();
        if (!ctx) return 0;
        LuaContextPtr fctx = ctx->CreateInherited();
        LuaFunctionPtr func = sb::make_shared<LuaFunction>();
        
        if (!func->SetSource(fctx, value.CString())) {
            return 0;
        }
        
        return new RocketEventListener(fctx,func);
    }
    
    void RocketContext::RegisterFont(const sb::string& name,int size,bool italic,bool bold,const FontPtr& fnt) {
        sb::string hname = name;
        if (italic) {
            hname+="i";
        }
        if (bold) {
            hname+="b";
        }
        if (m_fonts[hname]) {
            m_fonts[hname]->RemoveReference();
        }
        m_fonts[hname] = new RocketFontFaceHandle(fnt);
    }
    
    static inline int convert_modifiers( GHL::UInt32 mods ) {
		int res = 0;
		if (mods&GHL::KEYMOD_SHIFT) res|=Rocket::Core::Input::KM_SHIFT;
		if (mods&GHL::KEYMOD_ALT) res|=Rocket::Core::Input::KM_ALT;
		if (mods&GHL::KEYMOD_CTRL) res|=Rocket::Core::Input::KM_CTRL;
		if (mods&GHL::KEYMOD_COMMAND) res|=Rocket::Core::Input::KM_META;
		return res;
	}
	static inline int convert_mouse_button( GHL::MouseButton btn ) {
		if (btn==GHL::MOUSE_BUTTON_LEFT)
			return 0;
		if (btn==GHL::MOUSE_BUTTON_RIGHT)
			return 1;
		return 0;
	}
    
    void RocketContext::OnMouseDown( GHL::MouseButton key, int x,int y, GHL::UInt32 mods) {
		ProcessMouseMove( x,y, convert_modifiers(mods) );
		ProcessMouseButtonDown( convert_mouse_button(key), convert_modifiers(mods) );
	}
	void RocketContext::OnMouseMove( int x,int y, GHL::UInt32 mods ) {
		ProcessMouseMove( x,y, convert_modifiers(mods) );
	}
	void RocketContext::OnMouseUp( GHL::MouseButton key, int x,int y, GHL::UInt32 mods) {
		ProcessMouseMove( x,y, convert_modifiers(mods) );
		ProcessMouseButtonUp( convert_mouse_button(key), convert_modifiers(mods) );
	}
}
