/*
 *  sb_platform.h
 *  SR
 *
 *  Created by Андрей Куницын on 30.04.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_PLATFORM_H
#define SB_PLATFORM_H

#if defined( _MSC_VER )
#define SB_COMPILER_MSVC
#endif

#if defined( __GNUC__ )
#define SB_COMPILER_GCC
#endif

#ifdef SB_COMPILER_GCC
#define SB_ATTRIBUTE_PACKED __attribute__(( packed ))
#endif

#ifndef SB_ATTRIBUTE_PACKED
#define SB_ATTRIBUTE_PACKED
#endif


#endif /*SB_PLATFORM_H*/