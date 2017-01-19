#ifndef SB_MYGUI_CONFIG_H_INCLUDED
#define SB_MYGUI_CONFIG_H_INCLUDED

#define MYGUI_STATIC
#define MYGUI_DONT_USE_DYNLIB
#define MYGUI_DONT_USE_OBSOLETE
#define MYGUI_USE_PREMULTIPLIED_ALPHA

#if !defined(__FLASHPLAYER__) && !defined(__ANDROID__) && (!defined(__clang__))
#define MYGUI_DONT_REPLACE_NULLPTR
#elif defined( __clang__ )
#if __has_feature(cxx_nullptr) || __has_extension(cxx_nullptr)
    #define MYGUI_DONT_REPLACE_NULLPTR
#endif
#endif

#include <string>

namespace MyGUI
{
    struct LogLevel;
}
namespace Sandbox {
    namespace mygui {
        void log_message(const std::string& section,const MyGUI::LogLevel& level, const std::string& message);
        
    }
    namespace meta {
        struct type_info;
    }
}

        
#define MYGUI_LOGGING(section, level, text) \
    Sandbox::mygui::log_message(section,MyGUI::LogLevel::level,MyGUI::LogStream()<<text<<MyGUI::LogStream::End())

#define MYGUI_RTTI_TYPE const Sandbox::meta::type_info*

#define MYGUI_RTTI_GET_TYPE(type) type::get_static_type_info()

#define MYGUI_DECLARE_TYPE_NAME(Type) \
    public: \
        static const std::string& getClassTypeName() { static std::string type = #Type; return type; } \
        virtual const std::string& getTypeName() const { return getClassTypeName(); } \
        virtual const Sandbox::meta::type_info* get_type_info() const; \
        static const Sandbox::meta::type_info* get_static_type_info();\



#define MYGUI_RTTI_BASE(BaseType) \
    public: \
        typedef BaseType RTTIBase; \
        MYGUI_DECLARE_TYPE_NAME(BaseType) \
        /** Compare with selected type */ \
        virtual bool isType(MYGUI_RTTI_TYPE _type) const { return MYGUI_RTTI_GET_TYPE(BaseType) == _type; } \
        virtual bool isTypeNameOf(const std::string& name) const { return name == getClassTypeName(); } \
        /** Compare with selected type */ \
        template<typename Type> bool isType() const { return isType(MYGUI_RTTI_GET_TYPE(Type)); } \
        /** Try to cast pointer to selected type. \
        @param _throw If true throw exception when casting in wrong type, else return nullptr \
        */ \
        template<typename Type> Type* castType(bool _throw = true) \
        { \
            if (this->isType<Type>()) return static_cast<Type*>(this); \
            MYGUI_ASSERT(!_throw, "Error cast type") \
            return nullptr; \
        } \
        /** Try to cast pointer to selected type. \
        @param _throw If true throw exception when casting in wrong type, else return nullptr \
        */ \
        template<typename Type> const Type* castType(bool _throw = true) const \
        { \
            if (this->isType<Type>()) return static_cast<Type*>(this); \
            MYGUI_ASSERT(!_throw, "Error cast type") \
            return nullptr; \
        }

#define MYGUI_RTTI_DERIVED(DerivedType) \
    public: \
        MYGUI_DECLARE_TYPE_NAME(DerivedType) \
        typedef RTTIBase Base; \
        typedef DerivedType RTTIBase; \
        /** Compare with selected type */ \
        virtual bool isType(MYGUI_RTTI_TYPE _type) const { return MYGUI_RTTI_GET_TYPE(DerivedType) == _type || Base::isType(_type); } \
        virtual bool isTypeNameOf(const std::string& name) const { return name == getClassTypeName() || Base::isTypeNameOf(name); } \
        /** Compare with selected type */ \
        template<typename Type> bool isType() const { return isType(MYGUI_RTTI_GET_TYPE(Type)); }



#endif /*SB_MYGUI_CONFIG_H_INCLUDED*/
