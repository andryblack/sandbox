#ifndef SB_CRC32_H_INCLUDED
#define SB_CRC32_H_INCLUDED

#include <sbstd/sb_string.h>
#include <ghl_types.h>

namespace GHL {
    struct Data;
    struct DataStream;
}
namespace Sandbox {
    GHL::UInt32 CRC32(const char* src);
    GHL::UInt32 CRC32(const GHL::Byte* data,size_t size);
}

#endif /*SB_CRC32_H_INCLUDED*/

