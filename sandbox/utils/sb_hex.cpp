#include "sb_hex.h"
#include <ghl_types.h>

namespace Sandbox {
    
    static const char hex_chars[] = "0123456789abcdef";
    
    sb::string DataToHex(const void* data, size_t size) {
        sb::string res;
        res.resize(size*2, '0');
        const GHL::Byte* bytes = static_cast<const GHL::Byte*>(data);
        for (size_t i=0;i<size;++i) {
            res[i*2+0] = hex_chars[(bytes[i]&0xf0)>>4];
            res[i*2+1] = hex_chars[(bytes[i]&0x0f)];
        }
        return res;
    }
    
}