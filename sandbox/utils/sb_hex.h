#ifndef SB_HEX_H_INCLUDED
#define SB_HEX_H_INCLUDED

#include <sbstd/sb_string.h>
#include <ghl_types.h>

namespace Sandbox {
    
    sb::string DataToHex(const void* data, size_t size);
    void EncodeByte(GHL::Byte d,char* buf);
    void EncodeByteU(GHL::Byte d,char* buf);
    GHL::Byte DecodeHEXByte(const char* buf);
}

#endif /*SB_HEX_H_INCLUDED*/
