#include "sb_md5.h"

extern "C" {
#include "md5.h"
}

#include "sb_hex.h"
#include <ghl_vfs.h>

#include "sb_file_provider.h"
#include "luabind/sb_luabind_stack.h"

namespace Sandbox {
    
    sb::string MD5Sum(const char* src) {
        size_t len = ::strlen(src);
        MD5_CTX ctx;
        MD5_Init(&ctx);
        MD5_Update(&ctx, src, len);
        unsigned char sum[16];
        MD5_Final(sum, &ctx);
        return DataToHex(sum,16);
    }
    GHL::UInt32 MD5Hash(const char* src) {
        size_t len = ::strlen(src);
        MD5_CTX ctx;
        MD5_Init(&ctx);
        MD5_Update(&ctx, src, len);
        unsigned char sum[16];
        MD5_Final(sum, &ctx);
        GHL::UInt32 sum32[4];
        memcpy(sum32, sum, 16);
        return sum32[0] ^ sum32[1] ^ sum32[2] ^ sum32[3];
    }
    sb::string MD5SumData(const GHL::Byte* data,size_t size) {
        MD5_CTX ctx;
        MD5_Init(&ctx);
        MD5_Update(&ctx, data, size);
        unsigned char sum[16];
        MD5_Final(sum, &ctx);
        return DataToHex(sum,16);
    }
    sb::string MD5SumStream( GHL::DataStream* ds , size_t& total_size) {
        MD5_CTX ctx;
        MD5_Init(&ctx);
        static const size_t BUFFER_SIZE = 1024 * 4;
        GHL::Byte buffer[BUFFER_SIZE];
        while (!ds->Eof()) {
            size_t size = ds->Read(buffer, BUFFER_SIZE);
            total_size+=size;
            MD5_Update(&ctx, buffer, size);
        }
        unsigned char sum[16];
        MD5_Final(sum, &ctx);
        return DataToHex(sum,16);
    }
    sb::string MD5SumStream( GHL::DataStream* ds) {
        size_t s = 0;
        return MD5SumStream(ds,s);
    }
    
    int MD5SumFile( lua_State* L ) {
        const char* fn = luaL_checkstring(L, 1);
        FileProvider* fp = luabind::stack<FileProvider*>::get(L, 2, false);
        GHL::DataStream* ds = fp->OpenFile(fn);
        if (!ds) {
            luaL_error(L, "not found file %s",fn);
        }
        size_t size = 0;
        lua_pushstring(L, MD5SumStream(ds,size).c_str());
        ds->Release();
        lua_pushinteger(L, size);
        return 2;
    }
    
}
