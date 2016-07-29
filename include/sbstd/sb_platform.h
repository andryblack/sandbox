#ifndef SB_PLATFORM_H_INCLUDED
#define SB_PLATFORM_H_INCLUDED

#include <cstdio>
#include <stdarg.h>

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