#ifndef SB_JSON_H_INCLUDED
#define SB_JSON_H_INCLUDED

#include <sbstd/sb_string.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_vector.h>

#include "sb_lua_context.h"

namespace GHL {
    struct Data;
}

namespace Sandbox {
    
    
    void register_json(lua_State* L);
    
    sb::string convert_to_json(const LuaContextPtr& ctx);
    LuaContextPtr convert_from_json(lua_State* L,const char* json);
    LuaContextPtr convert_from_json(lua_State* L,const GHL::Data* data);
    
    bool json_parse_object(const char* data,sb::map<sb::string,sb::string>& res);

}

#endif