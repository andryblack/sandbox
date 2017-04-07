#ifndef SB_MD5_H_INCLUDED
#define SB_MD5_H_INCLUDED

#include <sbstd/sb_string.h>
#include <ghl_types.h>

namespace GHL {
    struct Data;
    struct DataStream;
}
struct lua_State;

namespace Sandbox {
    
    sb::string MD5Sum(const char* src);
    sb::string MD5SumData(const GHL::Byte* data,size_t size);
    sb::string MD5SumStream( GHL::DataStream* ds );
    sb::string MD5SumStream( GHL::DataStream* ds , size_t& size);
    int MD5SumFile( lua_State* L );
    unsigned int MD5Hash(const char* src);
}

#endif /*SB_MD5_H_INCLUDED*/
