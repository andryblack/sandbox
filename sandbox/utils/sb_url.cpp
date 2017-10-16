#include "sb_url.h"
#include "sb_utf.h"
#include "sb_hex.h"

namespace Sandbox {
    sb::string UrlEncode(const char* data) {
        sb::string encoded;
        char sybm[4] = {'%','0','0',0};
        const char* str = data;
        while (*str) {
            char ch = *str;
            if (!ch) break;
            if ((ch>='a' && ch <='z') ||
                (ch>='A' && ch <='Z') ||
                (ch>='0' && ch <='9') ||
                (ch=='-') || (ch=='_') ||
                (ch=='.') || (ch=='~')) {
                encoded.append(1, ch);
            } else {
                EncodeByteU(GHL::Byte(ch),&sybm[1]);
                encoded.append(sybm);
            }
            ++str;
        }
        return encoded;
    }
    
    sb::string UrlDecode(const char* data) {
        sb::string decoded;
        const char* str = data;
        while (*str) {
            char ch = *str;
            ++str;
            if (!ch) break;
            if (ch=='%') {
                if (str[0] && str[1]) {
                    decoded.append(1, DecodeHEXByte(str));
                    str += 2;
                }
            } else {
                decoded.append(1, ch);
            }
        }
        return decoded;
    }
}
