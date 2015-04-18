#ifndef SB_BASE64_H_INCLUDED
#define SB_BASE64_H_INCLUDED

#include <sbstd/sb_string.h>

namespace GHL {
    struct Data;
}
namespace Sandbox {
    
    sb::string Base64Decode(const char* src);
    GHL::Data* Base64DecodeData(const char* src);
    
}

#endif /*SB_BASE64_H_INCLUDED*/