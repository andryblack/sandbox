#ifndef SB_URL_H_INCLUDED
#define SB_URL_H_INCLUDED

#include <sbstd/sb_string.h>

namespace Sandbox {
    
    sb::string UrlEncode(const char* data);
    sb::string UrlDecode(const char* data);
    
}

#endif /*SB_URL_H_INCLUDED*/
