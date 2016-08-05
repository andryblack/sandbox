#include <pugixml.hpp>
#include <luabind/sb_luabind.h>
#include <meta/sb_meta.h>
#include <sb_file_provider.h>
#include <sbstd/sb_pointer.h>
#include <ghl_data.h>
#include <ghl_data_stream.h>

namespace XML {
    class Document;
    typedef sb::intrusive_ptr<Document> DocumentPtr;
    typedef sb::intrusive_ptr<const Document> DocumentConstPtr;
    
    class Node;
    typedef sb::intrusive_ptr<Node> NodePtr;
    
    class Document : public sb::ref_countered_base_not_copyable {
        pugi::xml_document  m_doc;
    public:
        Document() {}
        ~Document() {}
        static int load(lua_State* L) {
            Document* self = Sandbox::luabind::stack<Document*>::get(L, 1, false);
            const char* path = luaL_checkstring(L, 2);
            Sandbox::FileProvider* fp = Sandbox::luabind::stack<Sandbox::FileProvider*>::get(L, 3, false);
            GHL::DataStream* ds = fp->OpenFile(path);
            if (!ds) {
                lua_pushnil(L);
                lua_pushfstring(L, "not found file %s",path);
                return 2;
            }
            GHL::Data* da = GHL_ReadAllData(ds);
            ds->Release();
            pugi::xml_parse_result res = self->m_doc.load_buffer(da->GetData(),da->GetSize());
            da->Release();
            if (!res) {
                lua_pushnil(L);
                lua_pushstring(L, res.description());
                return 2;
            }
            lua_pushboolean(L, 1);
            return 1;
        }
        
        static int parse(lua_State* L) {
            Document* self = Sandbox::luabind::stack<Document*>::get(L, 1, false);
            const char* text = luaL_checkstring(L, 2);
            pugi::xml_parse_result res = self->m_doc.load_string(text);
            if (!res) {
                lua_pushnil(L);
                lua_pushstring(L, res.description());
                return 2;
            }
            lua_pushboolean(L, 1);
            return 1;
        }
        
        NodePtr get_root() const;
        NodePtr create_root(const char* name);
        struct string_writer : public pugi::xml_writer {
            sb::string result;
            virtual void write(const void* data, size_t size)  {
                result.append(static_cast<const char*>(data),size);
            }
        };
        sb::string to_text() const {
            string_writer writer;
            m_doc.save(writer);
            return writer.result;
        }
    };
    
    struct NodeIterator {
        NodePtr parent;
        pugi::xml_node_iterator iterator;
        pugi::xml_node_iterator end;
        static int iterator_func(lua_State* L);
    };
    
    class Node : public sb::ref_countered_base_not_copyable {
    private:
        DocumentConstPtr m_parent;
        pugi::xml_node  m_node;
    public:
        Node(const DocumentConstPtr& parent, const pugi::xml_node& node) : m_parent(parent),m_node(node) {
            
        }
        const DocumentConstPtr& get_parent() const { return m_parent; }
        const char* get_name() const {
            return m_node.name();
        }
        const char* get_value() const {
            return m_node.child_value();
        }
        void set_value(const char* v) {
            m_node.set_value(v);
        }
        
        NodePtr append_child(const char* name) {
            pugi::xml_node n = m_node.append_child(name);
            return NodePtr(new Node(m_parent,n));
        }
        void set_attribute(const char* name,const char* value) {
            if (!value) {
                m_node.remove_attribute(name);
            } else {
                if (!m_node.attribute(name))
                    m_node.append_attribute(name).set_value(value);
                else
                    m_node.attribute(name).set_value(value);
            }
        }
        bool has_attribute(const char* name) const {
            return m_node.attribute(name);
        }
        void remove_attribute(const char* name) {
            m_node.remove_attribute(name);
        }
        const char* get_attribute(const char* name) {
            if (!m_node.attribute(name))
                return 0;
            return m_node.attribute(name).value();
        }
        static int enumerate_childs(lua_State* L) {
            Node* self = Sandbox::luabind::stack<Node*>::get(L, 1, false);
            NodeIterator iterator = { NodePtr(self), self->m_node.begin(), self->m_node.end() };
            lua_pushcclosure(L, &NodeIterator::iterator_func, 0);
            Sandbox::luabind::stack<NodeIterator>::push(L, iterator);
            lua_pushnil(L);
            return 3;
        }
    };
    
    NodePtr Document::get_root() const {
        return NodePtr(new Node(DocumentConstPtr(this),m_doc.document_element()));
    }
    
    NodePtr Document::create_root(const char* name) {
        pugi::xml_node n = m_doc.append_child(name);
        return NodePtr(new Node(DocumentConstPtr(this),n));
    }
    
    int NodeIterator::iterator_func(lua_State* L) {
        NodeIterator* self = Sandbox::luabind::stack<NodeIterator*>::get(L, 1, false);
        if (self->iterator == self->end) {
            lua_pushnil(L);
            return 1;
        }
        Sandbox::luabind::stack<NodePtr>::push(L, NodePtr(new Node(self->parent->get_parent(),*self->iterator)));
        ++self->iterator;
        return 1;
    }
    
    
}
SB_META_DECLARE_KLASS(XML::Document, void)
SB_META_BEGIN_KLASS_BIND(XML::Document)
SB_META_CONSTRUCTOR(())
bind(method("load", &XML::Document::load));
bind(method("parse", &XML::Document::parse));
SB_META_PROPERTY_RO(root, get_root)
SB_META_METHOD(create_root)
SB_META_METHOD(to_text)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(XML::Node, void)
SB_META_BEGIN_KLASS_BIND(XML::Node)
SB_META_PROPERTY_RO(name, get_name)
SB_META_PROPERTY_RW(value, get_value, set_value)
SB_META_METHOD(set_attribute)
SB_META_METHOD(get_attribute)
SB_META_METHOD(remove_attribute)
SB_META_METHOD(has_attribute)
SB_META_METHOD(append_child)
bind(method("enumerate_childs",&XML::Node::enumerate_childs));
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(XML::NodeIterator, void)
SB_META_BEGIN_KLASS_BIND(XML::NodeIterator)
SB_META_END_KLASS_BIND()

int luaopen_xml(lua_State* L) {
    Sandbox::luabind::ExternClass<XML::Node>(L);
    Sandbox::luabind::Class<XML::Document>(L);
    Sandbox::luabind::RawClass<XML::NodeIterator>(L);
    lua_getglobal(L, "XML");
    return 1;
}

