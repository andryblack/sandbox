/*
 *  sb_assert.h
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_ASSERT_H
#define SB_ASSERT_H

#ifdef SB_DEBUG
#include "sb_log.h"
#include <cstdlib>
inline void sb_terminate() /*__attribute__((noreturn)) */{
    ::abort();
}
#define sb_assert( COND ) do { if (!(COND)) { \
	::Sandbox::LogError("assert") << "assert :" << #COND; \
	::Sandbox::LogError("assert") << "in function :" << __FUNCTION__ ; \
	::Sandbox::LogError("assert") << "at : " << __FILE__ << " " << __LINE__ << " " << #COND;\
	sb_terminate();} } while (false)
#else
	///// release build
	#define sb_assert( COND ) do { } while (false)
#endif


#endif /*SB_ASSERT_H*/
