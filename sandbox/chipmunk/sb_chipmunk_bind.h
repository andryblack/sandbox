/*
 *  sb_chipmunk_bind.h
 *  YinYang
 *
 *  Created by Андрей Куницын on 04.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */
#ifndef SB_CHIPMUNK_BIND_H_INCLUDED
#define SB_CHIPMUNK_BIND_H_INCLUDED

namespace Sandbox {
	
	class LuaVM;
	
	namespace Chipmunk {
		void Bind( LuaVM* lua );
	}
}

#endif /*SB_CHIPMUNK_BIND_H_INCLUDED*/