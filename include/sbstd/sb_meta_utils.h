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

namespace sb {
	
	
	
	
	template <bool v,class A,class B> struct select_type {
		typedef A result;
	};
	template <class A,class B> struct select_type<false,A,B> {
		typedef B result;
	};
	
	template <class A,class B> struct is_some_type { enum { result = 0, }; };
	template <class T> struct is_some_type<T,T> { enum { result = 1, }; };
	
	struct null_type{};
	
}
#endif /*SB_META_UTILS_H*/
