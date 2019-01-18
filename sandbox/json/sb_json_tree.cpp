#include "sb_json_tree.h"
#include <cstdlib>

namespace Sandbox {
    
    namespace internal {
        
        static void json_free(void* val) {
            ::free(val);
        }
        template <class T>
        static T* json_alloc() {
            return static_cast<T*>(::malloc(sizeof(T)));
        }
        template <class T>
        static T* json_alloc(size_t add) {
            return static_cast<T*>(::malloc(sizeof(T)+add));
        }
        static void free_child(json_val_t* val);
        
        static void free_array(json_val_array_t* val) {
            json_val_array_element_t* el = val->first;
            while (el) {
                free_child(&el->val);
                json_val_array_element_t* prev = el;
                el = el->next;
                json_free(prev);
            }
        }
        static void free_object(json_val_object_t* val) {
            json_val_object_element_t* el = val->first;
            while (el) {
                free_child(&el->val);
                json_val_object_element_t* prev = el;
                el = el->next;
                json_free(prev->key);
                json_free(prev);
            }
        }
        
        static void free_child(json_val_t* val) {
            if (val->type == json_type_array) {
                free_array(static_cast<json_val_array_t*>(val));
            } else if (val->type == json_type_object) {
                free_object(static_cast<json_val_object_t*>(val));
            }
        }
        
        json_tree_t::json_tree_t() : root(0) {
            
        }
        
        json_tree_t::~json_tree_t() {
            free();
        }
        
        void json_tree_t::free() {
            if (root) {
                free_child(root);
                json_free(root);
                root = 0;
            }
        }
        
        static void put_el(json_val_array_t* array,json_val_array_element_t* el) {
            el->next = 0;
            ++array->len;
            if (array->last) {
                array->last->next = el;
            }
            array->last = el;
            if (!array->first) array->first = el;
        }
        static void put_el(json_val_object_t* object,json_val_object_element_t* el) {
            el->next = 0;
            ++object->len;
            if (object->last) {
                object->last->next = el;
            }
            object->last = el;
            if (!object->first) object->first = el;
        }
        
        static json_val_t* new_extra(json_tree_t* tree,size_t extra) {
            json_val_t* res = 0;
            json_tree_t::stack_el& t = tree->top();
            if (t.array) {
                json_val_array_element_t* el = json_alloc<json_val_array_element_t>(extra);
                res = &el->val;
                put_el(t.array,el);
            } else if (t.object) {
                json_val_object_element_t* el = json_alloc<json_val_object_element_t>(extra);
                sb_assert(t.key);
                el->key = t.key;
                t.key = 0;
                res = &el->val;
                put_el(t.object,el);
            } else {
                res = json_alloc<json_val_t>(extra);
            }
            return res;
        }
        
        template <class T>
        static T* new_val(json_tree_t* tree) {
            return static_cast<T*>(new_extra(tree, sizeof(T)-sizeof(json_val_t)));
        }
        
        template <class T>
        static json_val_t* new_raw(json_tree_t* tree,T v) {
            typedef json_val_raw_t<T> type_t;
            type_t* res = new_val<type_t>(tree);
            res->value = v;
            return res;
        }
        
        
        json_val_t* json_tree_t::new_nil() {
            json_val_t* res = new_extra(this, 0);
            res->type = json_type_nil;
            if (!root) root = res;
            return res;
        }
        json_val_t* json_tree_t::new_bool(bool v) {
            json_val_t* res = new_extra(this, 0);
            res->type = v ? json_type_bool_true : json_type_bool_false;
            if (!root) root = res;
            return res;
        }
        
        json_val_t* json_tree_t::new_double(double v) {
            json_val_t* res = new_raw(this, v);
            res->type = json_type_double;
            if (!root) root = res;
            return res;
        }
        json_val_t* json_tree_t::new_integer(long long v) {
            json_val_t* res = new_raw(this, v);
            res->type = json_type_integer;
            if (!root) root = res;
            return res;
        }
        json_val_t* json_tree_t::new_string(const InplaceString& s) {
            size_t add = sizeof(json_val_string_t)-sizeof(json_val_t)+s.length()+1;
            json_val_string_t* res = static_cast<json_val_string_t*>(new_extra(this, add));
            res->type = json_type_string;
            strncpy(res->value, s.begin(), s.length());
            res->value[s.length()]=0;
            if (!root) root = res;
            return res;
        }
        json_tree_t::stack_el json_tree_t::empty = {0,0,0};
        
        json_val_object_t* json_tree_t::new_object() {
            json_val_object_t* res = new_val<json_val_object_t>(this);
            res->type = json_type_object;
            res->first = 0;
            res->last = 0;
            res->len = 0;
            json_tree_t::stack_el el = empty;
            el.object = res;
            stack.push_back(el);
            if (!root) root = res;
            return res;
        }
        json_val_array_t* json_tree_t::new_array() {
            json_val_array_t* res = new_val<json_val_array_t>(this);
            res->type = json_type_array;
            res->first = 0;
            res->last = 0;
            res->len = 0;
            json_tree_t::stack_el el = empty;
            el.array = res;
            stack.push_back(el);
            if (!root) root = res;
            return res;
        }
    }
    
    JsonObject::JsonObject(const internal::json_val_object_t* val) : JsonValue(val) {
        if (!IsObject()) { reset(); }
    }
    JsonObject::JsonObject(const JsonValue& val) : JsonValue(val) {
        if (!IsObject()) { reset(); }
    }
    JsonValue JsonObject::Get(const char* key) const {
        if (m_val) {
            const internal::json_val_object_element_t* el = get()->first;
            while (el) {
                if (strcmp(key, el->key)==0) {
                    return JsonValue(&el->val);
                }
                el = el->next;
            }
        }
        return JsonValue();
    }
    
    
    JsonArray::JsonArray(const internal::json_val_array_t* val) : JsonValue(val) {
        if (!IsArray()) { reset(); }
    }
    JsonArray::JsonArray(const JsonValue& val) : JsonValue(val) {
        if (!IsArray()) { reset(); }
    }
    JsonValue JsonArray::Get(size_t idx) const {
        if (m_val) {
            if (idx > get()->len)
                return JsonValue();
            size_t i = 0;
            const internal::json_val_array_element_t* el = get()->first;
            while (el) {
                if (i == idx) {
                    return JsonValue(&el->val);
                }
                el = el->next;
                ++i;
            }
        }
        return JsonValue();
    }
    
    
    JsonDocument::JsonDocument() {
        
    }
    
    JsonDocument::~JsonDocument() {
        
    }
   
    void JsonDocument::OnBeginObject() {
        m_tree.new_object();
    }
    void JsonDocument::OnEndObject() {
        sb_assert(m_tree.top().object);
        m_tree.stack.pop_back();
    }
    void JsonDocument::OnBeginArray() {
        m_tree.new_array();
    }
    void JsonDocument::OnEndArray() {
        sb_assert(m_tree.top().array);
        m_tree.stack.pop_back();
    }
    void JsonDocument::OnKey(const InplaceString& str) {
        sb_assert(m_tree.top().object);
        sb_assert(!m_tree.top().key);
        m_tree.top().key = internal::json_alloc<char>(str.length());
        strncpy(m_tree.top().key, str.begin(),str.length());
        m_tree.top().key[str.length()]=0;
    }
    void JsonDocument::OnNull() {
        m_tree.new_nil();
    }
    void JsonDocument::OnBool(bool v) {
        m_tree.new_bool(v);
    }
    void JsonDocument::OnString(const InplaceString& str) {
        m_tree.new_string(str);
    }
    void JsonDocument::OnInteger(long long v) {
        m_tree.new_integer(v);
    }
    void JsonDocument::OnNumber(double v) {
        m_tree.new_double(v);
    }
    
    
}
