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

#include <ghl_types.h>

#ifdef SB_DEBUG
#include "sb_log.h"
#include <cstdlib>
extern bool (*sb_terminate_handler)();
inline void sb_terminate() /*__attribute__((noreturn)) */{
    if (!sb_terminate_handler || !sb_terminate_handler())
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

namespace sb {
    template <bool f> struct static_assert_failure {
        typedef char failed[f ? 1 : -1];
    };
}

#define sb_static_assert( COND ) typedef ::sb::static_assert_failure<COND>::failed GHL_CONCAT(assertion,__LINE__)

#endif /*SB_ASSERT_H*/
