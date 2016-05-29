#include "sb_md5.h"

extern "C" {
#include "md5.h"
}

#include "sb_hex.h"
#include <ghl_vfs.h>

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
    sb::string MD5SumData(const GHL::Byte* data,size_t size) {
        MD5_CTX ctx;
        MD5_Init(&ctx);
        MD5_Update(&ctx, data, size);
        unsigned char sum[16];
        MD5_Final(sum, &ctx);
        return DataToHex(sum,16);
    }
    sb::string MD5SumStream( GHL::DataStream* ds ) {
        MD5_CTX ctx;
        MD5_Init(&ctx);
        static const size_t BUFFER_SIZE = 1024 * 4;
        GHL::Byte buffer[BUFFER_SIZE];
        while (!ds->Eof()) {
            size_t size = ds->Read(buffer, BUFFER_SIZE);
            MD5_Update(&ctx, buffer, size);
        }
        unsigned char sum[16];
        MD5_Final(sum, &ctx);
        return DataToHex(sum,16);
    }
    
}