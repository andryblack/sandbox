#ifndef SB_JSON_H_INCLUDED
#define SB_JSON_H_INCLUDED

struct lua_State;

namespace Sandbox {
    
    void register_json(lua_State* L);
}

#endif