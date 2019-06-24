#ifndef SB_PLATFORM_H_INCLUDED
#define SB_PLATFORM_H_INCLUDED

#include <cstdio>
#include <stdarg.h>

#if defined( _MSC_VER )
#define SB_COMPILER_MSVC
#endif

#if defined(__clang__)
#define SB_COMPILER_CLANG
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
#define SB_COMPILER_MINGW
#endif
#if defined(__GNUC__)
#define SB_COMPILER_GCC
#endif

#if defined(SB_COMPILER_MINGW)
/* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52991 */
#define SB_BEGIN_PACKED _Pragma("pack(push,1)")
#define SB_END_PACKED _Pragma("pack(pop)")
#elif defined(SB_COMPILER_GCC) || defined(SB_COMPILER_CLANG)
#define SB_ATTRIBUTE_PACKED __attribute__(( packed ))
#elif defined(SB_COMPILER_MSVC)
#define SB_BEGIN_PACKED __pragma(pack(push,1))
#define SB_END_PACKED __pragma(pack(pop))
#else
#error("unknown compiler");
#endif

#ifndef SB_ATTRIBUTE_PACKED
#define SB_ATTRIBUTE_PACKED
#endif

#ifndef SB_BEGIN_PACKED
#define SB_BEGIN_PACKED
#endif

#ifndef SB_END_PACKED
#define SB_END_PACKED
#endif

#if __cplusplus > 199711L
#define SB_OVERRIDE override
#endif
#ifndef SB_OVERRIDE
#define SB_OVERRIDE
#endif


namespace sb {

#ifdef SB_COMPILER_MSVC
    
    static inline int vsnprintf(char* str, size_t size, const char* format, va_list ap)
    {
        int count = -1;
        
        if (size != 0)
            count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
        if (count == -1)
            count = _vscprintf(format, ap);
        
        return count;
    }
    
    static inline int snprintf(char* str, size_t size, const char* format, ...)
    {
        int count;
        va_list ap;
        
        va_start(ap, format);
        count = vsprintf_s(str, size, format, ap);
        va_end(ap);
        
        return count;
    }
#else
    using ::snprintf;
#endif

}

#endif /*SB_PLATFORM_H_INCLUDED*/
