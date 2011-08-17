/*
 *  sb_meta_utils.h
 *  SR
 *
 *  Created by Андрей Куницын on 20.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_META_UTILS_H
#define SB_META_UTILS_H

namespace Sandbox {
	
	
	
	
	template <bool v,class A,class B> struct SelectType {
		typedef A Result;
	};
	template <class A,class B> struct SelectType<false,A,B> {
		typedef B Result;
	};
	
	template <class A,class B> struct IsSomeType { enum { Result = 0, }; };
	template <class T> struct IsSomeType<T,T> { enum { Result = 1, }; };
	
	struct NullType{};
	
}
#endif /*SB_META_UTILS_H*/
