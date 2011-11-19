/*
 *  sb_rocket.h
 *  YinYang
 *
 *  Created by Андрей Куницын on 10.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */

#ifndef SB_ROCKET_H_INCLUDED
#define SB_ROCKET_H_INCLUDED

#include "sb_shared_ptr.h"
#include <ghl_keys.h>
#include <ghl_types.h>


namespace Rocket {
	namespace Core {
		class Context;
		class ElementDocument;
	}
}

namespace GHL {
	struct System;
}
namespace Sandbox {
	
	
	
	typedef shared_ptr< ::Rocket::Core::Context > RocketContextPtr;
	typedef shared_ptr< ::Rocket::Core::ElementDocument > RocketDocumentPtr;
	
	class Graphics;
	class Resources;
	class Lua; 
	
	class RocketLib {
	public:
		explicit RocketLib( Resources* resources, GHL::System* system, Lua* lua);
		~RocketLib();
		void Draw( Graphics& g ) const;
		RocketDocumentPtr LoadDocument( const char* file );
		void SetBasePath( const char* path );
		bool LoadFont( const char* path );
		void OnMouseDown( GHL::MouseButton key, int x,int y, GHL::UInt32 mods);
		void OnMouseMove( int x,int y, GHL::UInt32 mods );
		void OnMouseUp( GHL::MouseButton key, int x,int y, GHL::UInt32 mods);
		void OnKeyDown( GHL::Key key, GHL::UInt32 mods ) ;
		void OnKeyUp( GHL::Key key, GHL::UInt32 mods ) ;
		void OnChar( GHL::UInt32 ch ) ;
		RocketContextPtr GetContext() { return m_context; }
		void SetDebuggerVisible( bool v );
		bool GetDebuggerVisible() const;
	private:
		struct Data;
		Data*	m_data;
		RocketContextPtr	m_context;
	};
	typedef shared_ptr<RocketLib> RocketLibPtr;
}

#endif /*SB_ROCKET_H_INCLUDED*/