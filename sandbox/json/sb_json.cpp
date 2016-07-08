#include "sb_json.h"
#include <algorithm>

extern "C" {
#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
    
}
#include "luabind/sb_luabind_stack.h"
#include <ghl_data.h>
#include "sb_log.h"

#include <ghl_data_stream.h>
#include "sb_file_provider.h"

namespace Sandbox {
    
    struct parse_context {
        lua_State* L;
        int stack_depth;
        struct state_t {
            enum type {
                array,
                map
            } type;
            int count;
        };
        sb::vector<state_t> state;
        void on_pre_value() {
            if (!state.empty()) {
                if (state.back().type == state_t::array) {
                    ++state.back().count;
                    lua_pushinteger(L, state.back().count);
                    ++stack_depth;
                }
            }
        }
        void on_value(bool pop = false) {
            if (pop) {
                sb_assert(!state.empty());
                state.pop_back();
            }
            if (!state.empty()) {
                if (state.back().type == state_t::map) {
                    sb_assert(lua_istable(L, -3));
                    sb_assert(lua_isstring(L, -2));
                    lua_settable(L, -3);
                    stack_depth-=2;
                } else if (state.back().type == state_t::array) {
                    sb_assert(lua_istable(L, -3));
                    sb_assert(lua_isnumber(L, -2));
                    lua_settable(L, -3);
                    stack_depth-=2;
                }
            }
            
        }
    };
    
    static int yajl_parse_null(void * ctx) {
        parse_context* c = static_cast<parse_context*>(ctx);
        c->on_pre_value();
        lua_pushnil(c->L);
        ++c->stack_depth;
        c->on_value();
        return 1;
    }
    static int yajl_parse_boolean(void * ctx, int boolVal) {
        parse_context* c = static_cast<parse_context*>(ctx);
        c->on_pre_value();
        lua_pushboolean(c->L, boolVal);
        ++c->stack_depth;
        c->on_value();
        return 1;
    }
    static int yajl_parse_integer(void * ctx, long long integerVal) {
        parse_context* c = static_cast<parse_context*>(ctx);
        c->on_pre_value();
        lua_pushnumber(c->L, integerVal);
        ++c->stack_depth;
        c->on_value();
        return 1;
    }
    static int yajl_parse_double(void * ctx, double doubleVal) {
        parse_context* c = static_cast<parse_context*>(ctx);
        c->on_pre_value();
        lua_pushnumber(c->L, doubleVal);
        ++c->stack_depth;
        c->on_value();
        return 1;
    }
    
    /** strings are returned as pointers into the JSON text when,
     * possible, as a result, they are _not_ null padded */
    static int yajl_parse_string(void * ctx, const unsigned char * stringVal,
                                 size_t stringLen) {
        parse_context* c = static_cast<parse_context*>(ctx);
        c->on_pre_value();
        lua_pushlstring(c->L, reinterpret_cast<const char*>(stringVal),stringLen);
        ++c->stack_depth;
        c->on_value();
        return 1;
    }
    
    static int yajl_parse_start_map(void * ctx) {
        parse_context* c = static_cast<parse_context*>(ctx);
        c->on_pre_value();
        lua_newtable(c->L);
        ++c->stack_depth;
        parse_context::state_t s;
        s.type = parse_context::state_t::map;
        s.count = 0;
        c->state.push_back(s);
        return 1;
    }
    static int yajl_parse_map_key(void * ctx, const unsigned char * key,
                                  size_t stringLen) {
        parse_context* c = static_cast<parse_context*>(ctx);
        lua_pushlstring(c->L, reinterpret_cast<const char*>(key),stringLen);
        ++c->stack_depth;
        return 1;
    }
    static int yajl_parse_end_map(void * ctx) {
        parse_context* c = static_cast<parse_context*>(ctx);
        sb_assert(!c->state.empty());
        sb_assert(c->state.back().type==parse_context::state_t::map);
        c->on_value(true);
        return 1;
    }
    
    static int yajl_parse_start_array(void * ctx) {
        parse_context* c = static_cast<parse_context*>(ctx);
        c->on_pre_value();
        lua_newtable(c->L);
        ++c->stack_depth;
        parse_context::state_t s;
        s.type = parse_context::state_t::array;
        s.count = 0;
        c->state.push_back(s);
        return 1;
    }
    static int yajl_parse_end_array(void * ctx) {
        parse_context* c = static_cast<parse_context*>(ctx);
        sb_assert(!c->state.empty());
        sb_assert(c->state.back().type==parse_context::state_t::array);
        c->on_value(true);
        return 1;
    }
    
    static void fill_parse_callbacks( yajl_callbacks& cb ) {
        cb.yajl_null = &yajl_parse_null;
        cb.yajl_boolean = &yajl_parse_boolean;
        cb.yajl_integer = &yajl_parse_integer;
        cb.yajl_double = &yajl_parse_double;
        cb.yajl_number = 0;
        cb.yajl_string = &yajl_parse_string;
        cb.yajl_start_map = &yajl_parse_start_map;
        cb.yajl_map_key = &yajl_parse_map_key;
        cb.yajl_end_map = &yajl_parse_end_map;
        cb.yajl_start_array = &yajl_parse_start_array;
        cb.yajl_end_array = &yajl_parse_end_array;
    }

    static int json_parse(lua_State* L) {
        if (!lua_isstring(L, 1)) {
            lua_pushnil(L);
            lua_pushstring(L, "need string");
            return 2;
        }
        size_t len = 0;
        const char* text = lua_tolstring(L, 1, &len);
        yajl_callbacks cb;
        fill_parse_callbacks(cb);
        parse_context ctx;
        ctx.L = L;
        ctx.stack_depth = 0;
        
        yajl_handle h = yajl_alloc(&cb, 0, &ctx);
        yajl_status s = yajl_parse(h,reinterpret_cast<const unsigned char * >(text),len);
        if (s == yajl_status_ok) {
            s = yajl_complete_parse(h);
        }
        if (s!=yajl_status_ok) {
            unsigned char* err_text = yajl_get_error(h, 1,reinterpret_cast<const unsigned char * >(text),len);
            lua_pop(L, ctx.stack_depth);
            lua_pushnil(L);
            lua_pushstring(L, reinterpret_cast<const char*>(err_text));
            yajl_free_error(h,err_text);
            yajl_free(h);
            return 2;
        }
        yajl_free(h);
        return ctx.stack_depth;
    }
    
    struct encode_context {
        sb::string data;
    };
    static void yajl_encode_print(void * ctx,const char * str,size_t len) {
        encode_context* c = static_cast<encode_context*>(ctx);
        c->data.append(str,len);
    }
    static void do_json_string(lua_State* L,int indx,yajl_gen g) {
        size_t len = 0;
        const char* str = lua_tolstring(L,indx,&len);
        if (!str || len == 0) {
            yajl_gen_string(g, reinterpret_cast<const unsigned char*>("unknown"),7 );
        } else {
            yajl_gen_string(g, reinterpret_cast<const unsigned char*>(str),len );
        }
    }
    static void do_json_encode(lua_State* L,int indx,yajl_gen g,bool sort_keys);
    
    static void do_json_encode_table(lua_State* L,int indx,yajl_gen g,bool sort_keys) {
        LUA_CHECK_STACK(0);
        sb_assert(lua_istable(L, indx));
        lua_pushvalue(L, indx);
        int count = 0;
        bool only_integers = true;
        sb::vector<sb::string> keys;
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            /* uses 'key' (at index -2) and 'value' (at index -1) */
            if (lua_isnumber(L, -2)) {
                lua_Integer i = lua_tointeger(L, -2);
                lua_Number n = lua_tonumber(L, -2);
                if ( lua_Number(i) == n ) {
                    ++count;
                    if (count!=i) {
                        only_integers = false;
                    }
                } else {
                    only_integers = false;
                }
            } else {
                only_integers = false;
            }
            
            if (sort_keys) {
                size_t len = 0;
                lua_pushvalue(L, -2);
                const char* str = lua_tolstring(L,-1,&len);
                lua_pop(L, 1);
                if (!str || len == 0) {
                    keys.push_back("");
                } else {
                    keys.push_back(sb::string(str,len));
                }
            }
            
            if (!only_integers && !sort_keys) {
                lua_pop(L, 2);
                break;
            }
            /* removes 'value'; keeps 'key' for next iteration */
            lua_pop(L, 1);
        }
        if (only_integers && count!=0) {
            yajl_gen_array_open(g);
            lua_pushnil(L);
            while (lua_next(L, -2) != 0) {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
                do_json_encode(L, -1, g, sort_keys);
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(L, 1);
            }
            yajl_gen_array_close(g);
        } else if (sort_keys) {
            std::sort(keys.begin(), keys.end());
            yajl_gen_map_open(g);
            for (sb::vector<sb::string>::const_iterator it = keys.begin();it!=keys.end();++it) {
                lua_getfield(L, -1, it->c_str());
                yajl_gen_string(g, reinterpret_cast<const unsigned char*>(it->c_str()),it->length() );
                do_json_encode(L, -1, g, sort_keys);
                lua_pop(L, 1);
            }
            yajl_gen_map_close(g);
        } else {
            yajl_gen_map_open(g);
            lua_pushnil(L);
            while (lua_next(L, -2) != 0) {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
                lua_pushvalue(L, -2); // k v k
                do_json_string(L, -1, g);
                do_json_encode(L, -2, g, sort_keys);
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(L, 2);
            }
            yajl_gen_map_close(g);
        }
        lua_pop(L, 1);
    }
    
    static void do_json_encode(lua_State* L,int indx,yajl_gen g,bool sort_keys) {
        LUA_CHECK_STACK(0);
        int type = lua_type(L, indx);
        if (type == LUA_TNIL) {
            yajl_gen_null(g);
        } else if (type == LUA_TBOOLEAN) {
            yajl_gen_bool(g, lua_toboolean(L, indx));
        } else if (type == LUA_TNUMBER) {
            lua_Integer i = lua_tointeger(L, indx);
            lua_Number n = lua_tonumber(L, indx);
            if ( lua_Number(i) == n ) {
                yajl_gen_integer(g, i);
            } else {
                yajl_gen_double(g, n);
            }
        } else if (type == LUA_TSTRING) {
            size_t len = 0;
            const char* str = lua_tolstring(L,indx,&len);
            yajl_gen_string(g, reinterpret_cast<const unsigned char*>(str), len);
        } else if (type == LUA_TFUNCTION) {
            yajl_gen_string(g, reinterpret_cast<const unsigned char*>("function"), 8);
        } else if (type == LUA_TTHREAD) {
            yajl_gen_string(g, reinterpret_cast<const unsigned char*>("thread"), 6);
        } else if (type == LUA_TLIGHTUSERDATA) {
            yajl_gen_string(g, reinterpret_cast<const unsigned char*>("ldata"), 5);
        }  else if (type == LUA_TUSERDATA) {
            yajl_gen_string(g, reinterpret_cast<const unsigned char*>("data"), 4);
        } else if (type == LUA_TTABLE) {
            do_json_encode_table(L,indx,g,sort_keys);
        } else {
            /// unknown;
            sb_assert(false);
            yajl_gen_null(g);
        }
    }
    
    static int json_encode(lua_State* L) {
        encode_context ctx;
        yajl_gen g = yajl_gen_alloc(0);
        yajl_gen_config(g,yajl_gen_print_callback,&yajl_encode_print,&ctx);
        bool sort_keys = false;
        if (lua_gettop(L)>1 && lua_isboolean(L, 2) && lua_toboolean(L, 2)) {
            yajl_gen_config(g,yajl_gen_beautify, 1);
        }
        if (lua_gettop(L)>2 && lua_isboolean(L, 3) && lua_toboolean(L, 3)) {
            sort_keys = true;
        }
        if (lua_gettop(L)>0) {
            do_json_encode(L,1,g,sort_keys);
        }
        yajl_gen_free(g);
        lua_pushstring(L, ctx.data.c_str());
        return 1;
    }
    
    static int json_load(lua_State* L) {
        const char* file = luaL_checkstring(L, 1);
        FileProvider* res = luabind::stack<FileProvider*>::get(L, 2);
        if (!res) {
            luaL_argerror(L, 2, "must be 'Sandbox::FileProvider'");
        }
        GHL::DataStream* ds = res->OpenFile(file);
        if (!ds) {
            luaL_error(L, "opening file %s", file);
        }
        lua_gc(L, LUA_GCSTOP, 1);
        GHL::Data* data = GHL_ReadAllData(ds);
        sb_assert(data);
        ds->Release();
        
        yajl_callbacks cb;
        fill_parse_callbacks(cb);
        parse_context ctx;
        ctx.L = L;
        ctx.stack_depth = 0;
        
        yajl_handle h = yajl_alloc(&cb, 0, &ctx);
        yajl_config(h,yajl_allow_comments,1);
        yajl_status s = yajl_parse(h,reinterpret_cast<const unsigned char * >(data->GetData()),data->GetSize());
        if (s == yajl_status_ok) {
            s = yajl_complete_parse(h);
        }
        lua_gc(L, LUA_GCRESTART,1);
        if (s!=yajl_status_ok) {
            unsigned char* err_text = yajl_get_error(h, 1,reinterpret_cast<const unsigned char * >(data->GetData()),data->GetSize());
            lua_pop(L, ctx.stack_depth);
            lua_pushnil(L);
            lua_pushstring(L, reinterpret_cast<const char*>(err_text));
            yajl_free_error(h,err_text);
            yajl_free(h);
            return 2;
        }
        yajl_free(h);
        return ctx.stack_depth;
    }
    
    static int cjson_encode(lua_State* L) {
        int results = json_encode(L);
        if (results>1) {
            if (lua_isnil(L, -2)) {
                lua_remove(L, -2);
                lua_error(L);
            }
        }
        return results;
    }
    
    static int cjson_decode(lua_State* L) {
        int results = json_parse(L);
        if (results>1) {
            if (lua_isnil(L, -2)) {
                lua_remove(L, -2);
                lua_error(L);
            }
        }
        return results;
    }
    
    void register_json(lua_State* L) {
        LUA_CHECK_STACK(0);
        lua_newtable(L);
        lua_pushcfunction(L, &json_parse);
        lua_setfield(L, -2, "decode");
        lua_pushcfunction(L, &json_load);
        lua_setfield(L, -2, "load");
        lua_pushcfunction(L, &json_encode);
        lua_setfield(L, -2, "encode");
        lua_pushvalue(L, -1);
        lua_setglobal(L, "json");
        
        lua_getglobal(L, "package"); // json, package
        sb_assert(lua_istable(L, -1));
        lua_getfield(L, -1, "loaded"); // json, package, loaded
        sb_assert(lua_istable(L, -1));
        lua_pushvalue(L, -3); // json, package, loaded, json
        lua_setfield(L, -2, "cjson.safe");
        
        lua_newtable(L);
        lua_pushcfunction(L, &cjson_decode);
        lua_setfield(L, -2, "decode");
        lua_pushcfunction(L, &cjson_encode);
        lua_setfield(L, -2, "encode");
        lua_setfield(L, -2, "cjson");
        
        lua_pop(L, 3);
    }
    
    sb::string convert_to_json(const LuaContextPtr& tctx) {
        encode_context ctx;
        if (!tctx) return ctx.data;
        lua_State* L = tctx->GetVM();
        if (!L) return ctx.data;
        LUA_CHECK_STACK(0)
        tctx->GetObject(L);
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            return ctx.data;
        }
        yajl_gen g = yajl_gen_alloc(0);
        yajl_gen_config(g,yajl_gen_print_callback,&yajl_encode_print,&ctx);
        do_json_encode_table(L,-1,g,false);
        yajl_gen_free(g);
        lua_pop(L, 1);
        return ctx.data;
    }
    static LuaContextPtr convert_from_json(lua_State* L,const void* data,size_t size) {
        
        LUA_CHECK_STACK(0)
        
        size_t len = size;
        yajl_callbacks cb;
        fill_parse_callbacks(cb);
        parse_context ctx;
        ctx.L = L;
        ctx.stack_depth = 0;
        
        yajl_handle h = yajl_alloc(&cb, 0, &ctx);
        yajl_status s = yajl_parse(h,static_cast<const unsigned char * >(data),len);
        if (s == yajl_status_ok) {
            s = yajl_complete_parse(h);
        }
        if (s!=yajl_status_ok) {
            unsigned char* err_text = yajl_get_error(h, 1,static_cast<const unsigned char * >(data),len);
            lua_pop(L, ctx.stack_depth);
            LogError() << "failed parse json: " << reinterpret_cast<const char*>(err_text);
            yajl_free_error(h,err_text);
            yajl_free(h);
            return LuaContextPtr();
        }
        yajl_free(h);
        sb_assert(ctx.stack_depth == 1);
        if (!ctx.stack_depth)
            return LuaContextPtr();
        
        LuaContextPtr res = luabind::stack<LuaContextPtr>::get(L, -1);
        lua_pop(L, ctx.stack_depth);
        return res;
    }
    LuaContextPtr convert_from_json(lua_State* L,const char* text) {
        if (!L) return LuaContextPtr();
        if (!text) return LuaContextPtr();
        return convert_from_json(L, text, ::strlen(text));
    }
    LuaContextPtr convert_from_json(lua_State* L,const GHL::Data* data) {
        if (!L) return LuaContextPtr();
        if (!data) return LuaContextPtr();
        return convert_from_json(L, data->GetData(),data->GetSize());
    }
    
    struct json_parse_map {
        sb::map<sb::string,sb::string>& res;
        sb::string* out_val;
        int depth;
        explicit json_parse_map(sb::map<sb::string,sb::string>& r) : res(r),out_val(0),depth(0) {}
        
        static int yajl_parse_null(void * ctx) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->out_val) {
                c->out_val->assign("null");
                c->out_val = 0;
                return 1;
            }
            return 0;
        }
        static int yajl_parse_boolean(void * ctx, int boolVal) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->out_val) {
                c->out_val->assign(boolVal == 0 ? "false" : "true");
                c->out_val = 0;
                return 1;
            }
            return 0;
        }
        
        static int yajl_parse_number(void * ctx, const char * numberVal,
                            size_t numberLen) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->out_val) {
                c->out_val->assign(numberVal,numberVal+numberLen);
                c->out_val = 0;
                return 1;
            }
            return 0;
        }
        
        /** strings are returned as pointers into the JSON text when,
         * possible, as a result, they are _not_ null padded */
        static int yajl_parse_string(void * ctx, const unsigned char * stringVal,
                                     size_t stringLen) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->out_val) {
                c->out_val->assign(stringVal,stringVal+stringLen);
                c->out_val = 0;
                return 1;
            }
            return 0;
        }
        
        static int yajl_parse_start_map(void * ctx) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->depth == 0) {
                c->depth++;
                return 1;
            }
            return 0;
        }
        static int yajl_parse_map_key(void * ctx, const unsigned char * key,
                                      size_t stringLen) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            sb::string key_s(key,key+stringLen);
            c->out_val = &(c->res[key_s]);
            return 1;
        }
        static int yajl_parse_end_map(void * ctx) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->depth == 1) {
                c->depth--;
                return 1;
            }
            return 0;
        }
        
        static int yajl_parse_start_array(void * ctx) {
            return 0;
        }
        static int yajl_parse_end_array(void * ctx) {
            return 0;
        }
        
        static void fill_callbacks(yajl_callbacks& cb) {
            cb.yajl_null = &json_parse_map::yajl_parse_null;
            cb.yajl_boolean = &json_parse_map::yajl_parse_boolean;
            cb.yajl_integer = 0;
            cb.yajl_double = 0;
            cb.yajl_number = &json_parse_map::yajl_parse_number;
            cb.yajl_string = &json_parse_map::yajl_parse_string;
            cb.yajl_start_map = &json_parse_map::yajl_parse_start_map;
            cb.yajl_map_key = &json_parse_map::yajl_parse_map_key;
            cb.yajl_end_map = &json_parse_map::yajl_parse_end_map;
            cb.yajl_start_array = &json_parse_map::yajl_parse_start_array;
            cb.yajl_end_array = &json_parse_map::yajl_parse_end_array;
        }
    };
    
    bool json_parse_object(const char* data,sb::map<sb::string,sb::string>& res) {
        yajl_callbacks cb;
        json_parse_map ctx(res);
        ctx.fill_callbacks(cb);
        
        yajl_handle h = yajl_alloc(&cb, 0, &ctx);
        size_t len = ::strlen(data);
        yajl_status s = yajl_parse(h,reinterpret_cast<const unsigned char * >(data),len);
        if (s == yajl_status_ok) {
            s = yajl_complete_parse(h);
        }
        if (s!=yajl_status_ok) {
            unsigned char* err_text = yajl_get_error(h, 1,reinterpret_cast<const unsigned char * >(data),len);
            LogError() << "failed parse json: " << reinterpret_cast<const char*>(err_text);
            yajl_free_error(h,err_text);
            yajl_free(h);
            return false;
        }
        yajl_free(h);
        return true;
    }
    
    struct json_parse_array_s {
        sb::vector<sb::string>& res;
        sb::string* out_val;
        int depth;
        explicit json_parse_array_s(sb::vector<sb::string>& r) : res(r),out_val(0),depth(0) {}
        
        static int yajl_parse_null(void * ctx) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->out_val) {
                c->out_val->assign("null");
                c->out_val = 0;
                return 1;
            }
            return 0;
        }
        static int yajl_parse_boolean(void * ctx, int boolVal) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->out_val) {
                c->out_val->assign(boolVal == 0 ? "false" : "true");
                c->out_val = 0;
                return 1;
            }
            return 0;
        }
        
        static int yajl_parse_number(void * ctx, const char * numberVal,
                                     size_t numberLen) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->out_val) {
                c->out_val->assign(numberVal,numberVal+numberLen);
                c->out_val = 0;
                return 1;
            }
            return 0;
        }
        
        /** strings are returned as pointers into the JSON text when,
         * possible, as a result, they are _not_ null padded */
        static int yajl_parse_string(void * ctx, const unsigned char * stringVal,
                                     size_t stringLen) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->out_val) {
                c->out_val->assign(stringVal,stringVal+stringLen);
                c->out_val = 0;
                return 1;
            }
            return 0;
        }
        
        static int yajl_parse_start_map(void * ctx) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->depth == 0) {
                c->depth++;
                return 1;
            }
            return 0;
        }
        static int yajl_parse_map_key(void * ctx, const unsigned char * key,
                                      size_t stringLen) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            sb::string key_s(key,key+stringLen);
            c->out_val = &(c->res[key_s]);
            return 1;
        }
        static int yajl_parse_end_map(void * ctx) {
            json_parse_map* c = static_cast<json_parse_map*>(ctx);
            if (c->depth == 1) {
                c->depth--;
                return 1;
            }
            return 0;
        }
        
        static int yajl_parse_start_array(void * ctx) {
            return 0;
        }
        static int yajl_parse_end_array(void * ctx) {
            return 0;
        }
        
        static void fill_callbacks(yajl_callbacks& cb) {
            cb.yajl_null = &json_parse_map::yajl_parse_null;
            cb.yajl_boolean = &json_parse_map::yajl_parse_boolean;
            cb.yajl_integer = 0;
            cb.yajl_double = 0;
            cb.yajl_number = &json_parse_map::yajl_parse_number;
            cb.yajl_string = &json_parse_map::yajl_parse_string;
            cb.yajl_start_map = &json_parse_map::yajl_parse_start_map;
            cb.yajl_map_key = &json_parse_map::yajl_parse_map_key;
            cb.yajl_end_map = &json_parse_map::yajl_parse_end_map;
            cb.yajl_start_array = &json_parse_map::yajl_parse_start_array;
            cb.yajl_end_array = &json_parse_map::yajl_parse_end_array;
        }
    };
    
    struct JsonBuilder::Impl {
        sb::string data;
        yajl_gen g;
        static void yajl_encode_print(void * ctx,const char * str,size_t len) {
            Impl* c = static_cast<Impl*>(ctx);
            c->data.append(str,len);
        }
        Impl() {
            g = yajl_gen_alloc(0);
            yajl_gen_config(g,yajl_gen_print_callback,&Impl::yajl_encode_print,this);
        }
        ~Impl() {
            yajl_gen_free(g);
        }
    };
    
    JsonBuilder::JsonBuilder() : m_impl(new Impl()) {
        
    }
    JsonBuilder::~JsonBuilder() {
        delete m_impl;
    }
    
    JsonBuilder& JsonBuilder::BeginObject() {
        yajl_gen_map_open(m_impl->g);
        return *this;
    }
    JsonBuilder& JsonBuilder::EndObject() {
        yajl_gen_map_close(m_impl->g);
        return *this;
    }
    
    JsonBuilder& JsonBuilder::BeginArray() {
        yajl_gen_array_open(m_impl->g);
        return *this;
    }
    JsonBuilder& JsonBuilder::EndArray() {
        yajl_gen_array_close(m_impl->g);
        return *this;
    }
    
    JsonBuilder& JsonBuilder::Key(const char* name) {
        yajl_gen_string(m_impl->g, reinterpret_cast<const unsigned char*>(name), ::strlen(name));
        return *this;
    }
    JsonBuilder& JsonBuilder::PutString(const char* value) {
        yajl_gen_string(m_impl->g, reinterpret_cast<const unsigned char*>(value), ::strlen(value));
        return *this;
    }
    JsonBuilder& JsonBuilder::PutInteger(int value) {
        yajl_gen_integer(m_impl->g, value);
        return *this;
    }
    JsonBuilder& JsonBuilder::PutNumber(double value) {
        yajl_gen_double(m_impl->g, value);
        return *this;
    }
    
    
    const sb::string& JsonBuilder::End() {
        return m_impl->data;
    }

    
}