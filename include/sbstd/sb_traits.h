/**
 * Copyright (C) 2010-2010, by Andrey AndryBlack Kunitsyn
 * (support.andryblack@gmail.com)
 */

#ifndef SB_TRAITS_H
#define SB_TRAITS_H

#include "sb_meta_utils.h"

    
namespace sb {

    namespace implementation {



        template <class T> struct is_std_unsigned_int {
            enum { result = false};
        };
        template <> struct is_std_unsigned_int<unsigned char> { enum { result=true}; };
        template <> struct is_std_unsigned_int<unsigned short int> { enum { result=true}; };
        template <> struct is_std_unsigned_int<unsigned int> { enum { result=true}; };
        template <> struct is_std_unsigned_int<unsigned long int> { enum { result=true}; };

        template <class T> struct is_std_signed_int {
            enum { result = false};
        };
        template <> struct is_std_signed_int<signed char> { enum { result=true}; };
        template <> struct is_std_signed_int<short int> { enum { result=true}; };
        template <> struct is_std_signed_int<int> { enum { result=true}; };
        template <> struct is_std_signed_int<long int> { enum { result=true}; };

        template <class T> struct is_std_other_int {
            enum { result = false};
        };
        template <> struct is_std_other_int<bool> { enum { result=true}; };
        template <> struct is_std_other_int<char> { enum { result=true}; };
        template <> struct is_std_other_int<wchar_t> { enum { result=true}; };

        template <class T> struct is_std_float {
            enum { result = false};
        };
        template <> struct is_std_float<float> { enum { result=true}; };
        template <> struct is_std_float<double> { enum { result=true}; };
        template <> struct is_std_float<long double> { enum { result=true}; };


        template <typename U> struct add_pointer
        {
            typedef U* result;
        };
        template <typename U> struct add_pointer<U&>
        {
            typedef U* result;
        };
        template <typename U> struct add_reference
        {
            typedef U& result;
        };
        template <typename U> struct add_reference<U&>
        {
            typedef U& result;
        };
        template <typename U> struct add_parameter_type
        {
            typedef const U& result;
        };
        template <typename U> struct add_parameter_type<U&>
        {
            typedef U& result;
        };

        template <typename T>
        struct is_function_pointer_raw
        {enum{result = 0};};
        template <typename T>
        struct is_function_pointer_raw<T(*)()>
        {enum{result = 1};};
        template <typename T,typename P1>
        struct is_function_pointer_raw<T(*)(P1)>
        {enum{result = 1};};
        template <typename T,typename P1,typename P2>
        struct is_function_pointer_raw<T(*)(P1,P2)>
        {enum{result = 1};};
        template <typename T,typename P1,typename P2,typename P3>
        struct is_function_pointer_raw<T(*)(P1,P2,P3)>
        {enum{result = 1};};
        template <typename T,typename P1,typename P2,typename P3,typename P4>
        struct is_function_pointer_raw<T(*)(P1,P2,P3,P4)>
        {enum{result = 1};};

        template <typename T>
        struct is_member_function_pointer_raw
        {enum{result = 0};};
        template <typename T,typename S>
        struct is_member_function_pointer_raw<T(S::*)()>
        {enum{result = 1};};
        template <typename T,typename S,typename P1>
        struct is_member_function_pointer_raw<T(S::*)(P1)>
        {enum{result = 1};};
        template <typename T,typename S,typename P1,typename P2>
        struct is_member_function_pointer_raw<T(S::*)(P1,P2)>
        {enum{result = 1};};
        template <typename T,typename S,typename P1,typename P2,typename P3>
        struct is_member_function_pointer_raw<T(S::*)(P1,P2,P3)>
        {enum{result = 1};};
        template <typename T,typename S,typename P1,typename P2,typename P3,typename P4>
        struct is_member_function_pointer_raw<T(S::*)(P1,P2,P3,P4)>
        {enum{result = 1};};
        template <typename T,typename S>
        struct is_member_function_pointer_raw<T(S::*)()const>
        {enum{result = 1};};
        template <typename T,typename S,typename P1>
        struct is_member_function_pointer_raw<T(S::*)(P1)const>
        {enum{result = 1};};
        template <typename T,typename S,typename P1,typename P2>
        struct is_member_function_pointer_raw<T(S::*)(P1,P2)const>
        {enum{result = 1};};
        template <typename T,typename S,typename P1,typename P2,typename P3>
        struct is_member_function_pointer_raw<T(S::*)(P1,P2,P3)const>
        {enum{result = 1};};
        template <typename T,typename S,typename P1,typename P2,typename P3,typename P4>
        struct is_member_function_pointer_raw<T(S::*)(P1,P2,P3,P4)const>
        {enum{result = 1};};
    }

    	

    template <class T>
    struct type_traits {
    private:
        template <class U> struct reference_traits {
            enum { result = false };
            typedef U referred_type;
        };
        template <class U> struct reference_traits<U&> {
            enum { result = true };
            typedef U referred_type;
        };
        template <class U> struct pointer_traits {
            enum { result = false };
            typedef U pointee_type;
        };
        template <class U> struct pointer_traits<U*> {
            enum { result = true };
            typedef U pointee_type;
        };
        template <class U> struct pointer_traits<U*&> {
            enum { result = true };
            typedef U pointee_type;
        };
        template <class U> struct pointer_to_member_traits {
            enum { result = false };
        };
        template <class U,class V> struct pointer_to_member_traits<U V::*> {
            enum { result = true };
        };
        template <class U,class V> struct pointer_to_member_traits<U V::*&> {
            enum { result = true };
        };
        template <class U> struct function_pointer_traits {
            enum { result = implementation::is_function_pointer_raw<U>::result };
        };
        template <typename U> struct pointer_to_member_function_traits {
            enum { result = implementation::is_member_function_pointer_raw<U>::result };
        };
        template <class U> struct un_const {
            typedef U result;
            enum { is_const = false };
        };
        template <class U> struct un_const<const U> {
            typedef U result;
            enum { is_const = true };
        };
        template <class U> struct un_const<const U&> {
            typedef U& result;
            enum { is_const = true };
        };
    public:
        typedef typename un_const<T>::result non_const_type;
        typedef typename un_const<T>::result unqualified_type;
        typedef typename pointer_traits<unqualified_type>::pointee_type pointee_type;
        typedef typename reference_traits<T>::referred_type referred_type;

        enum { is_const     = un_const<T>::is_const };
        enum { is_reference = reference_traits<unqualified_type>::result };
        enum { is_function  = function_pointer_traits<typename implementation::add_pointer<T>::result>::result };
        enum { is_function_pointer  = function_pointer_traits<typename reference_traits<unqualified_type>::referred_type>::result };
        enum { is_member_function_pointer = pointer_to_member_function_traits<typename reference_traits<unqualified_type>::referred_type>::result };
        enum { is_member_pointer = pointer_to_member_traits<typename reference_traits<unqualified_type>::referred_type>::result || is_member_function_pointer };
        enum { is_pointer   = pointer_traits<typename reference_traits<unqualified_type>::referred_type>::result || is_function_pointer };

        enum { is_std_unsigned_int  = implementation::is_std_unsigned_int<unqualified_type>::result ||
                                      implementation::is_std_unsigned_int<typename reference_traits<unqualified_type>::referred_type>::result };
        enum { is_std_signed_int    = implementation::is_std_signed_int<unqualified_type>::result ||
                                      implementation::is_std_signed_int<typename reference_traits<unqualified_type>::referred_type>::result };
        enum { is_std_integral      = is_std_unsigned_int || is_std_signed_int ||
                                      implementation::is_std_other_int<unqualified_type>::result ||
                                      implementation::is_std_other_int<typename reference_traits<unqualified_type>::referred_type>::result };
        enum { is_std_float         = implementation::is_std_float<unqualified_type>::result ||
                                      implementation::is_std_float<typename reference_traits<unqualified_type>::referred_type>::result };
        enum { is_std_arith        = is_std_integral || is_std_float };
        enum { is_std_fundamental   = is_std_arith || is_some_type<T,void>::result };

        typedef typename select_type<is_std_arith || is_pointer || is_member_pointer, T,
                typename implementation::add_parameter_type<T>::result>::result parameter_type;
    };


	/*
	
	template <typename From,typename To> struct IsConvertible {
		
		enum { 
			Result = ( sizeof(func(get())) == 1 ),
		};
	};*/

	struct IntConv { IntConv(int); };
	struct dummy2 { char d[2];};


	struct any_conversion
	{
	    template <typename T> any_conversion(const volatile T&);
	    template <typename T> any_conversion(T&);
	};

	template <typename T> struct checker
	{
	    static dummy2 _m_check(any_conversion ...);
	    static char _m_check(T, int);
	};

	template <typename From, typename To>
	struct is_convertible_basic_impl
	{
	    static From _m_from;
	    static bool const value = sizeof( checker<To>::_m_check(_m_from, 0) )
		== sizeof(char);
	};
		
	//template <typename T> struct IsEnum;
	template <typename T> struct is_enum {
		
		enum {
			convertible = is_convertible_basic_impl<T,IntConv>::value,
			result = !type_traits<T>::is_std_fundamental && convertible,
		};
	};
	template <typename T> struct is_enum<T&> {
		enum { result = is_enum<T>::result, };
	};
	template <typename T> struct is_enum<const T&> {
		enum { result = is_enum<T>::result, };
	};
	template <> struct is_enum<float> {
		enum { result = false, };
	};
	template <> struct is_enum<bool> {
		enum { result = false, };
	};
	
}


#endif // SB_TRAITS_H
