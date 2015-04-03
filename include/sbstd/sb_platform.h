#ifndef SB_PLATFORM_H_INCLUDED
#define SB_PLATFORM_H_INCLUDED

#include <cstdio>
#include <stdarg.h>

namespace sb {

#ifdef _MSC_VER
    
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
    using std::snprintf;
#endif

}

#endif /*SB_PLATFORM_H_INCLUDED*/