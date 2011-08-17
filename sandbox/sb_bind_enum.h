/*
 *  sb_bind_enum.h
 *  SR
 *
 *  Created by Андрей Куницын on 13.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_BIND_ENUM_H
#define SB_BIND_ENUM_H

namespace Sandbox {

	namespace Bind {
		
		struct EnumItem {
			const char* name;
			int value;
		};
		
		struct EnumBind {
			const char* name;
			const EnumItem* items;
		};
		
#define SB_BIND_BEGIN_ENUM(Name) static const char* name = #Name; \
		static const Sandbox::Bind::EnumItem items[] = { 
#define SB_BIND_END_ENUM { 0,0} }; static const Sandbox::Bind::EnumBind bind = { name,items }; 
#define SB_BIND_ENUM_ITEM(Name,Namespace) { #Name, Namespace::Name },
		
		
	}
	
}

#endif /*SB_BIND_ENUM_H*/