#ifndef SB_JSON_TREE_H_INCLUDED
#define SB_JSON_TREE_H_INCLUDED

#include "sb_json.h"

namespace Sandbox {
    
    namespace internal {
        enum json_val_e {
            json_type_nil,
            json_type_bool_true,
            json_type_bool_false,
            json_type_string,
            json_type_integer,
            json_type_double,
            json_type_object,
            json_type_array
        };
        struct json_val_t {
            json_val_e type;
        };
        template <class T>
        struct json_val_raw_t : json_val_t {
            T value;
        };
        typedef json_val_raw_t<long long> json_val_integer_t;
        typedef json_val_raw_t<double> json_val_double_t;
        struct json_val_string_t : json_val_t {
            char value[0];
        };
        struct json_val_array_element_t {
            json_val_array_element_t* next;
            json_val_t val;
        };
        struct json_val_array_t : json_val_t {
            size_t len;
            json_val_array_element_t* first;
            json_val_array_element_t* last;
        };
        struct json_val_object_element_t {
            json_val_object_element_t* next;
            char* key;
            json_val_t val;
        };
        struct json_val_object_t : json_val_t {
            size_t len;
            json_val_object_element_t* first;
            json_val_object_element_t* last;
        };
        
        struct json_tree_t {
            json_val_t* root;
            json_tree_t();
            ~json_tree_t();
            struct stack_el {
                json_val_array_t* array;
                json_val_object_t* object;
                char* key;
            };
            sb::vector<stack_el> stack;
            static stack_el empty;
            stack_el& top() { return stack.empty() ? empty : stack.back(); }
            void free();
            json_val_t* new_nil();
            json_val_t* new_bool(bool v);
            json_val_t* new_string(const InplaceString& v);
            json_val_t* new_integer(long long v);
            json_val_t* new_double(double v);
            json_val_array_t* new_array();
            json_val_object_t* new_object();
            
        };
    }
    
    class JsonValue {
    protected:
        const internal::json_val_t* m_val;
        void reset() { m_val = 0; }
    public:
        explicit JsonValue(const internal::json_val_t* val) : m_val(val) {}
        explicit JsonValue() : m_val(0) {}
        typedef const internal::json_val_t* JsonValue::*unspecified_bool_type;
        
        operator unspecified_bool_type() const {
            return m_val ? &JsonValue::m_val : 0;
        }
        bool IsEmpty() const { return m_val == 0;}
        bool IsNil() const { return m_val ? (m_val->type == internal::json_type_nil) : false; }
        bool IsBool() const { return m_val ? (m_val->type == internal::json_type_bool_true ||
                                               m_val->type == internal::json_type_bool_false) : false; }
        bool IsInteger() const { return m_val ? (m_val->type == internal::json_type_integer) : false; }
        bool IsDouble() const { return m_val ? (m_val->type == internal::json_type_double) : false; }
        bool IsNumber() const { return m_val ? (m_val->type == internal::json_type_integer ||
                                                m_val->type == internal::json_type_double) : false; }
        bool IsString() const { return m_val ? (m_val->type == internal::json_type_string) : false; }
        bool IsArray() const { return m_val ? (m_val->type == internal::json_type_array) : false; }
        bool IsObject() const { return m_val ? (m_val->type == internal::json_type_object) : false; }
        
        bool GetBool(bool def = false) const {
            return IsBool() ? (m_val->type == internal::json_type_bool_true) : def;
        }
        long long GetInteger(long long def = 0) const {
            return IsInteger() ? static_cast<const internal::json_val_integer_t*>(m_val)->value : def;
        }
        double GetDouble(double def = 0.0) const {
            return IsDouble() ? static_cast<const internal::json_val_double_t*>(m_val)->value : def;
        }
        double GetNumber(double def = 0.0) const {
            if (IsInteger()) return static_cast<const internal::json_val_integer_t*>(m_val)->value;
            if (IsDouble()) return static_cast<const internal::json_val_double_t*>(m_val)->value;
            return def;
        }
        const char* GetString(const char* def = 0) const {
            return IsString() ? static_cast<const internal::json_val_string_t*>(m_val)->value : def;
        }
    };
    class JsonObject : public JsonValue {
    protected:
        const internal::json_val_object_t* get() const { return static_cast<const internal::json_val_object_t*>(m_val); }
    public:
        explicit JsonObject(const internal::json_val_object_t* val);
        explicit JsonObject(const JsonValue& val);
        size_t GetSize() const { return m_val ? (get()->len) : 0; }
        JsonValue Get(const char* key) const;
        bool GetBool(const char* key,bool def=false) const {
            return Get(key).GetBool(def);
        }
        int GetInteger(const char* key,long long def = 0) const {
            return Get(key).GetInteger(def);
        }
        double GetDouble(const char* key,double def = 0.0) const {
            return Get(key).GetDouble(def);
        }
        double GetNumber(const char* key,double def = 0.0) const {
            return Get(key).GetNumber(def);
        }
        const char* GetString(const char* key,const char* def = 0) const {
            return Get(key).GetString(def);
        }
        
        class iterator {
        private:
            const internal::json_val_object_element_t* m_el;
            std::pair<const char*,JsonValue> m_val;
            void update() {
                if (m_el) {
                    m_val.second = JsonValue(&m_el->val);
                    m_val.first = m_el->key;
                } else {
                    m_val.second = JsonValue();
                    m_val.first = 0;
                }
            }
        public:
            explicit iterator( const internal::json_val_object_element_t* el) : m_el(el) { update(); }
            explicit iterator() : m_el(0) {}
            bool operator == (const iterator& o) const { return o.m_el == m_el; }
            bool operator != (const iterator& o) const { return o.m_el != m_el; }
            const std::pair<const char*,JsonValue>* operator -> () const { return &m_val; }
            const std::pair<const char*,JsonValue>& operator*() const { return m_val; }
            iterator& operator ++ () {
                if (m_el) {
                    m_el = m_el->next;
                    update();
                }
                return *this;
            }
        };
        iterator begin() const { return m_val ? iterator(get()->first) : iterator(0); }
        iterator end() const { return iterator(0); }
    };
    class JsonArray : public JsonValue {
    protected:
        const internal::json_val_array_t* get() const { return static_cast<const internal::json_val_array_t*>(m_val); }
    public:
        explicit JsonArray(const internal::json_val_array_t* val);
        explicit JsonArray(const JsonValue& val);
        size_t GetSize() const { return m_val ? (get()->len) : 0; }
        JsonValue Get(size_t idx) const;
        
        bool GetBool(size_t idx,bool def=false) const {
            return Get(idx).GetBool(def);
        }
        int GetInteger(size_t idx,long long def = 0) const {
            return Get(idx).GetInteger(def);
        }
        double GetDouble(size_t idx,double def = 0.0) const {
            return Get(idx).GetDouble(def);
        }
        double GetNumber(size_t idx,double def = 0.0) const {
            return Get(idx).GetNumber(def);
        }
        const char* GetString(size_t idx,const char* def = 0) const {
            return Get(idx).GetString(def);
        }
        
        class iterator {
        private:
            const internal::json_val_array_element_t* m_el;
            JsonValue m_val;
            void update() { if (m_el) m_val = JsonValue(&m_el->val); else m_val = JsonValue(); }
        public:
            explicit iterator( const internal::json_val_array_element_t* el) : m_el(el) { update(); }
            explicit iterator() : m_el(0) {}
            bool operator == (const iterator& o) const { return o.m_el == m_el; }
            bool operator != (const iterator& o) const { return o.m_el != m_el; }
            const JsonValue* operator -> () const { return &m_val; }
            const JsonValue& operator*() const { return m_val; }
            iterator& operator ++ () {
                if (m_el) {
                    m_el = m_el->next;
                    update();
                }
                return *this;
            }
        };
        iterator begin() const { return m_val ? iterator(get()->first) : iterator(0); }
        iterator end() const { return iterator(0); }
        
    };
    
    class JsonDocument : public JsonTraverseBase {
    private:
        internal::json_tree_t m_tree;
    public:
        JsonDocument();
        ~JsonDocument();
        
        virtual void OnBeginObject();
        virtual void OnEndObject();
        virtual void OnBeginArray();
        virtual void OnEndArray();
        virtual void OnKey(const InplaceString& str);
        virtual void OnNull();
        virtual void OnBool(bool v);
        virtual void OnString(const InplaceString& str);
        virtual void OnInteger(long long v);
        virtual void OnNumber(double v);
        
        JsonValue GetRoot() const { return JsonValue(m_tree.root); }
    };
    
}

#endif /*SB_JSON_TREE_H_INCLUDED*/


