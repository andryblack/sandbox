#ifndef SB_HEX_H_INCLUDED
#define SB_HEX_H_INCLUDED

#include <sbstd/sb_string.h>

namespace Sandbox {
    
    sb::string DataToHex(const void* data, size_t size);
    
}

#endif /*SB_HEX_H_INCLUDED*/