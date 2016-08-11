#ifndef SB_JSON_H_INCLUDED
#define SB_JSON_H_INCLUDED

#include <sbstd/sb_string.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_vector.h>

#include "sb_lua_context.h"

namespace GHL {
    struct Data;
    struct DataStream;
}

namespace Sandbox {
    
    
    void register_json(lua_State* L);
    
    sb::string convert_to_json(const LuaContextPtr& ctx);
    LuaContextPtr convert_from_json(lua_State* L,const char* json);
    LuaContextPtr convert_from_json(lua_State* L,const GHL::Data* data);
    
    bool json_parse_object(const char* data,sb::map<sb::string,sb::string>& res);

    class JsonBuilder  {
    private:
        struct Impl;
        Impl* m_impl;
        JsonBuilder(const JsonBuilder&);
        JsonBuilder& operator = (const JsonBuilder&);
    public:
        void reset();
        JsonBuilder();
        ~JsonBuilder();
        
        JsonBuilder& SetPretty(bool p);
        
        JsonBuilder& BeginObject();
        JsonBuilder& EndObject();
        
        JsonBuilder& BeginArray();
        JsonBuilder& EndArray();
        
        JsonBuilder& Key(const char* name);
        JsonBuilder& PutNull();
        JsonBuilder& PutBool(bool v);
        JsonBuilder& PutString(const char* value);
        JsonBuilder& PutInteger(int value);
        JsonBuilder& PutNumber(double value);
        
        
        const sb::string& End();
    };
    
    class JsonTraverser {
    private:
        sb::vector<char> m_stack;
        
        void BeginObject();
        void EndObject();
        void BeginArray();
        void EndArray();
        
        struct Ctx;
    protected:
        size_t GetDepth() const;
        bool IsObject() const;
        bool IsArray() const;
    public:
        JsonTraverser();
        
        virtual void OnBeginObject() {}
        virtual void OnEndObject() {}
        virtual void OnBeginArray() {}
        virtual void OnEndArray() {}
        virtual void OnKey(const sb::string& v) {}
        virtual void OnNull() {}
        virtual void OnBool(bool v) {}
        virtual void OnString(const sb::string& v) {}
        virtual void OnInteger(int v) {}
        virtual void OnNumber(double v) {}
        
        
        bool TraverseStream( GHL::DataStream* ds );
    };
    
    
}

#endif