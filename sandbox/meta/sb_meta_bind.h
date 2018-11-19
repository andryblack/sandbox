//
//  sb_meta_bind.h
//
//  Created on 7/10/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef sb_meta_bind_h
#define sb_meta_bind_h

namespace Sandbox {
    
    namespace meta {
        
        template <class T> struct bind_type {
            template <class U>
            static void bind( U& r );
        };
        
        
        template <class Proto> struct constructor_holder {
        };
        
        template <class Func> struct constructor_ptr_holder{
            Func func;
            explicit constructor_ptr_holder( Func func ) : func(func) {}
        };
        
        template <class Proto>
        static inline constructor_holder<Proto> constructor() {
            return constructor_holder<Proto>();
        }
        template <class Func>
        static inline constructor_ptr_holder<Func> constructor(Func func) {
            return constructor_ptr_holder<Func>(func);
        }
        
        
        
        template <class T,class U> struct property_holder {
            typedef U T::* prop_ptr;
            const char* name;
            prop_ptr prop;
            property_holder( const char* name, prop_ptr prop ) : name(name),prop(prop){
                sb_assert(name && *name!='"');
            }
        };
        template <class T,class U> 
        static inline property_holder<T, U> property( const char* name, U T::* prop ) {
            return property_holder<T, U>(name,prop);
        }
        
        template <class T,class Getter> struct property_holder_ro {
            const char* name;
            Getter getter;
            property_holder_ro( const char* name, Getter getter ) : name(name),getter(getter){
                sb_assert(name && *name!='"');
            }
        };
        template <class T,class U> 
        static inline property_holder_ro<T,U (T::*)() const> property_ro( const char* name,U (T::*getter)() const) {
            return property_holder_ro<T,U (T::*)() const>(name,getter);
        }
        template <class T,class U> 
        static inline property_holder_ro<T,const U& (T::*)() const> property_ro( const char* name,const U& (T::*getter)() const) {
            return property_holder_ro<T,const U& (T::*)() const>(name,getter);
        }
        template <class T,class Setter> struct property_holder_wo {
            const char* name;
            Setter setter;
            property_holder_wo( const char* name, Setter setter ) : name(name),setter(setter){
                sb_assert(name && *name!='"');
            }
        };
        template <class T,class U>
        static inline property_holder_wo<T,void (T::*)(U)> property_wo( const char* name,void (T::*setter)(U)) {
            return property_holder_wo<T,void(T::*)(U)>(name,setter);
        }
        template <class T,class U>
        static inline property_holder_wo<T,void(T::*)(const U&)> property_wo( const char* name,void(T::*setter)(const U&)) {
            return property_holder_wo<T,void(T::*)(const U&)>(name,setter);
        }
        static inline property_holder_wo<void,int(*)(lua_State*)> property_wo( const char* name,int(*func)(lua_State*)) {
            return property_holder_wo<void,int(*)(lua_State*)>(name,func);
        }

        
        template <class T,class Getter,class Setter> struct property_holder_rw {
            const char* name;
            Getter getter;
            Setter setter;
            property_holder_rw( const char* name, Getter getter, Setter setter ) : name(name),getter(getter),setter(setter){
                sb_assert(name && *name!='"');
            }
        };
        template <class T,class U> 
        static inline property_holder_rw<T,U (T::*)() const,void(T::*)(U)> property_rw( const char* name,
                                                                                U (T::*getter)() const,
                                                                                void (T::*setter)(U)) {
            return property_holder_rw<T,U (T::*)() const,void(T::*)(U)>(name,getter,setter);
        }
        template <class T,class U>
        static inline property_holder_rw<T,U (*)(const T*),void(*)(T*,U)> property_rw( const char* name,
                                                                                       U (*getter)(const T*),
                                                                                       void (*setter)(T*, U)) {
            return property_holder_rw<T,U (*)(const T*),void(*)(T*,U)>(name,getter,setter);
        }
        template <class T,class U>
        static inline property_holder_rw<T,U (*)(const T*),void(T::*)(U)> property_rw( const char* name,
                                                                                      U (*getter)(const T*),
                                                                                      void(T::*setter)(U)) {
            return property_holder_rw<T,U (*)(const T*),void(T::*)(U)>(name,getter,setter);
        }
        template <class T,class U>
        static inline property_holder_rw<T,const U& (T::*)() const,void(T::*)(const U&)> property_rw( const char* name,
                                                                                              const U& (T::*getter)() const,
                                                                                              void(T::*setter)(const U&)) {
            return property_holder_rw<T,const U& (T::*)() const,void(T::*)(const U&)>(name,getter,setter);
        }
        template <class T,class U> 
        static inline property_holder_rw<T, U (T::*)() const,void(T::*)(const U&)> property_rw( const char* name,
                                                                                             U (T::*getter)() const,
                                                                                              void(T::*setter)(const U&)) {
            return property_holder_rw<T,U (T::*)() const,void(T::*)(const U&)>(name,getter,setter);
        }
        
        template <class Func> struct method_holder {
            const char* name;
            Func func;
            method_holder( const char* name,Func func ) : name(name),func(func){
                sb_assert(name && *name!='"');
            }
        };
        template <typename Func> 
        static inline method_holder<Func> method( const char* name, Func func ) {
            return method_holder<Func>(name,func);
        }
        
        enum operator_name {
            op_add,
            op_mul,
            op_div,
            op_sub,
            op_eq,
            op_index
        };
        template <class Func> struct operator_holder {
            operator_name name;
            Func func;
            operator_holder( operator_name name,Func func ) : name(name),func(func){}
        };
        template <typename Func> 
        static inline operator_holder<Func> operator_( operator_name name, Func func ) {
            return operator_holder<Func>(name,func);
        }
        
        template <class Func> struct static_method_holder {
            const char* name;
            Func func;
            static_method_holder( const char* name,Func func ) : name(name),func(func){}
        };
        template <typename Func> 
        static inline static_method_holder<Func> static_method( const char* name, Func func ) {
            return static_method_holder<Func>(name,func);
        }
        
        template <class Type> struct static_value_holder {
            const char* name;
            const Type& value;
            static_value_holder( const char* name,const Type& value ) : name(name),value(value){}
        };
        template <typename Type> 
        static inline static_value_holder<Type> static_value( const char* name, const Type& value  ) {
            return static_value_holder<Type>(name,value);
        }
        
#define SB_META_BEGIN_KLASS_BIND(Klass)  \
    namespace Sandbox { namespace meta { \
        template <> template <class U> void bind_type<Klass>::bind(U& bind) { \
            typedef Klass ThisType;(void)bind;
        
#define SB_META_END_KLASS_BIND() \
        } \
    }}
        
#define SB_META_PROPERTY( name ) bind( property( #name , &ThisType::name ) );
#define SB_META_PROPERTY_RO( name, getter ) bind( property_ro( #name, &ThisType::getter ) );
#define SB_META_PROPERTY_WO( name, setter ) bind( property_wo( #name, &ThisType::setter ) );
#define SB_META_PROPERTY_RW( name, getter, setter ) bind( property_rw( #name, &ThisType::getter, &ThisType::setter ) );
#define SB_META_PROPERTY_RW_DEF( name ) bind( property_rw( #name, &ThisType::Get##name, &ThisType::Set##name ) );
#define SB_META_METHOD( name ) bind( method( #name , &ThisType::name ) );
#define SB_META_METHOD_S( name , sign ) bind( method( #name , static_cast<sign>(&ThisType::name) ) );
#define SB_META_OPERATOR_ADD() bind( operator_( op_add , &ThisType::operator+ ) );
#define SB_META_OPERATOR_MUL() bind( operator_( op_mul , &ThisType::operator* ) );
#define SB_META_OPERATOR_DIV() bind( operator_( op_div , &ThisType::operator/ ) );
#define SB_META_OPERATOR_SUB() bind( operator_( op_sub , &ThisType::operator- ) );
#define SB_META_OPERATOR_EQ() bind( operator_( op_eq , &ThisType::operator== ) );
#define SB_META_OPERATOR_ADD_(S) bind( operator_( op_add , (S)&ThisType::operator+ ) );
#define SB_META_OPERATOR_MUL_(S) bind( operator_( op_mul , (S)&ThisType::operator* ) );
#define SB_META_OPERATOR_DIV_(S) bind( operator_( op_div , (S)&ThisType::operator/ ) );
#define SB_META_OPERATOR_SUB_(S) bind( operator_( op_sub , (S)&ThisType::operator- ) );
#define SB_META_OPERATOR_EQ_(S) bind( operator_( op_eq , (S)&ThisType::operator== ) );
#define SB_META_STATIC_METHOD( name ) bind( static_method( #name , &ThisType::name ) );
#define SB_META_CONSTRUCTOR( args ) bind( constructor<void args>() );
        

#define SB_META_ENUM_BIND(Klass,Namespace,Items)  \
    namespace Sandbox { namespace meta { \
        template <> template <class U> void bind_type<Klass>::bind(U& bind) { \
            using Namespace; \
            Items \
        }\
    }}
#define SB_META_ENUM_ITEM( name ) bind( static_value( #name, name ));
#define SB_META_ENUM_MEMBER_ITEM( name,val ) bind( static_value( #name, val ));
    
    }
}

#endif
