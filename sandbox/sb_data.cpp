#include "sb_data.h"
#include "luabind/sb_luabind_stack.h"

SB_META_DECLARE_OBJECT(Sandbox::BinaryData, Sandbox::meta::object)


namespace Sandbox {
    
    
    int lua_ZlibUncompress(lua_State* L) {
        size_t data_len = 0;
        const char* compressed_string = luaL_checklstring(L, 1, &data_len);
                
        InlinedData compressed_data(compressed_string, data_len);
        GHL::Data* uncompressed = GHL_UnpackZlibData(&compressed_data);
        if (uncompressed) {
            lua_pushlstring(L, reinterpret_cast<const char*>(uncompressed->GetData()), uncompressed->GetSize());
            uncompressed->Release();
        } else {
            lua_pushnil(L);
        }
        return 1;
    }
    
    int lua_ZlibCompress(lua_State* L) {
        size_t data_len = 0;
        const char* uncompressed_string = luaL_checklstring(L, 1, &data_len);
        
        luaL_Buffer buffer;
        luaL_buffinit(L, &buffer);
        
        InlinedData uncompressed_data(uncompressed_string, data_len);
        GHL::Data* compressed = GHL_PackZlib(&uncompressed_data);
        if (compressed) {
            lua_pushlstring(L, reinterpret_cast<const char*>(compressed->GetData()), compressed->GetSize());
            compressed->Release();
        } else {
            lua_pushnil(L);
        }
        return 1;
    }

    
}
