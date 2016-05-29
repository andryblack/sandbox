#include "sb_base64.h"
#include "sb_data.h"
#include <sbstd/sb_vector.h>
#include <ghl_types.h>
#include "sb_log.h"

namespace Sandbox {
    
    static const sb::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
    
    
    void base64_encode(GHL::Byte const* bytes_to_encode, size_t in_len,std::string& ret) {
        
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];
        
        size_t out_size = (in_len * 4 + 2) / 3;
        ret.reserve(out_size);
        
        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;
                
                for(i = 0; (i <4) ; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }
        
        if (i)
        {
            for(j = i; j < 3; j++)
                char_array_3[j] = '\0';
            
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];
            
            while((i++ < 3))
                ret += '=';
            
        }
        
    }
    
    static bool base64_decode(const char* encoded_string, sb::vector<GHL::Byte>& ret) {
        if (!encoded_string)
            return false;
        
        int i = 0;
        int j = 0;
        
        GHL::Byte char_array_4[4], char_array_3[3];
        
        while (*encoded_string) {
            if (*encoded_string == '=')
                break;
            size_t p = base64_chars.find(*encoded_string);
            if (p == base64_chars.npos) {
                if (!isspace(*encoded_string)) {
                    LogError() << "invalid character " << int(*encoded_string);
                    return false;
                }
                ++encoded_string;
                continue;
            }
            ++encoded_string;
            char_array_4[i++] = p;
            if (i ==4) {
                
                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
                
                for (i = 0; (i < 3); i++)
                    ret.push_back( char_array_3[i] );
                i = 0;
            }
        }
        
        if (i) {
            for (j = i; j <4; j++)
                char_array_4[j] = 0;
            
            
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            
            for (j = 0; (j < i - 1); j++)
                ret.push_back( char_array_3[j] );
        }
        
        return true;
    }

    
    GHL::Data* Base64DecodeData(const char* src) {
        if (!src)
            return 0;
        VectorData<GHL::Byte>* d = new VectorData<GHL::Byte>();
        if (!base64_decode(src, d->vector())) {
            d->Release();
            return 0;
        }
        return d;
    }
    
    sb::string Base64Encode(const char* str) {
        sb::string ret;
        base64_encode(reinterpret_cast<const GHL::Byte*>(str),::strlen(str),ret);
        return ret;
    }
    
    sb::string Base64EncodeData(const GHL::Byte* data,size_t size) {
        sb::string ret;
        base64_encode(data,size,ret);
        return ret;
    }
}