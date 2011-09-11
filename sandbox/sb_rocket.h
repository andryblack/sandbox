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

namespace Sandbox {
	
	
	
	typedef shared_ptr< ::Rocket::Core::Context > RocketContextPtr;
	typedef shared_ptr< ::Rocket::Core::ElementDocument > RocketDocumentPtr;
	
	class Graphics;
	class Resources;
	
	class RocketLib {
	public:
		explicit RocketLib( Resources* resources);
		~RocketLib();
		void Draw( Graphics& g ) const;
		RocketDocumentPtr LoadDocument( const char* file );
		void SetBasePath( const char* path );
		bool LoadFont( const char* path );
		void OnMouseDown( GHL::MouseButton key, int x,int y, GHL::UInt32 mods);
		void OnMouseMove( int x,int y, GHL::UInt32 mods );
		void OnMouseUp( GHL::MouseButton key, int x,int y, GHL::UInt32 mods);
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