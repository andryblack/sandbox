#ifndef SB_BASE64_H_INCLUDED
#define SB_BASE64_H_INCLUDED

#include <sbstd/sb_string.h>
#include <ghl_types.h>

namespace GHL {
    struct Data;
}
struct lua_State;
namespace Sandbox {
    
    int lua_Base64Decode(lua_State* L);
    sb::string Base64Decode(const char* src);
    GHL::Data* Base64DecodeData(const char* src);
    sb::string Base64EncodeData(const GHL::Byte* data,size_t size);
    sb::string Base64Encode(const char* src);
    
}

#endif /*SB_BASE64_H_INCLUDED*/