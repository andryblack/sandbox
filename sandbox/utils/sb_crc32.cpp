#include "sb_crc32.h"
#include <ghl_data.h>

namespace Sandbox {
    
    
    GHL::UInt32 CRC32(const char* src) {
        return CRC32(reinterpret_cast<const GHL::Byte*>(src),::strlen(src));
    }
    GHL::UInt32 CRC32(const GHL::Byte* data,size_t size) {
        return GHL_CRC32(data,GHL::UInt32(size));
    }
}
