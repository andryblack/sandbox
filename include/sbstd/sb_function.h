#ifndef SB_FUNCTION_H
#define SB_FUNCTION_H


#include "sb_function_traits.h"
#include "sb_traits.h"
#include "sb_meta_utils.h"
#include "sb_shared_ptr.h"

#include "sb_assert.h"

#include <memory>


    namespace sb {
    
        namespace impl {
            template <typename Res> struct functor_impl_base {
                virtual ~functor_impl_base() {}
                typedef Res result_type;
                typedef null_type arg1_type;
                typedef null_type arg2_type;
                typedef null_type arg3_type;
                typedef null_type arg4_type;
                typedef null_type arg5_type;
            };
            
            template <typename Res,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5> struct functor_impl : public functor_impl_base<Res>{
                typedef typename type_traits<Arg1>::parameter_type arg1_type;
                typedef typename type_traits<Arg2>::parameter_type arg2_type;
                typedef typename type_traits<Arg3>::parameter_type arg3_type;
                typedef typename type_traits<Arg4>::parameter_type arg4_type;
                typedef typename type_traits<Arg5>::parameter_type arg5_type;
                virtual Res operator()(arg1_type,arg2_type,arg3_type,arg4_type,arg5_type) = 0;
                virtual functor_impl* clone() const = 0;
            };
            
            template <typename Res> struct functor_impl<Res,null_type,null_type,null_type,null_type,null_type> : public functor_impl_base<Res>{
                virtual Res operator()() = 0;
                virtual functor_impl* clone() const = 0;
            };
            template <typename Res,typename Arg1> struct functor_impl<Res,Arg1,null_type,null_type,null_type,null_type> : public functor_impl_base<Res>{
                typedef typename type_traits<Arg1>::parameter_type arg1_type;
                virtual Res operator()(arg1_type) = 0;
                virtual functor_impl* clone() const = 0;
            };
            template <typename Res,typename Arg1,typename Arg2> struct functor_impl<Res,Arg1,Arg2,null_type,null_type,null_type> : public functor_impl_base<Res> {
                typedef typename type_traits<Arg1>::parameter_type arg1_type;
                typedef typename type_traits<Arg2>::parameter_type arg2_type;
                virtual Res operator()(arg1_type,arg2_type) = 0;
                virtual functor_impl* clone() const = 0;
            };
            template <typename Res,typename Arg1,typename Arg2,typename Arg3> struct functor_impl<Res,Arg1,Arg2,Arg3,null_type,null_type> : public functor_impl_base<Res> {
                typedef typename type_traits<Arg1>::parameter_type arg1_type;
                typedef typename type_traits<Arg2>::parameter_type arg2_type;
                typedef typename type_traits<Arg3>::parameter_type arg3_type;
                virtual Res operator()(arg1_type,arg2_type,arg3_type) = 0;
                virtual functor_impl* clone() const = 0;
            };
            template <typename Res,typename Arg1,typename Arg2,typename Arg3,typename Arg4> struct functor_impl<Res,Arg1,Arg2,Arg3,Arg4,null_type> : public functor_impl_base<Res> {
                typedef typename type_traits<Arg1>::parameter_type arg1_type;
                typedef typename type_traits<Arg2>::parameter_type arg2_type;
                typedef typename type_traits<Arg3>::parameter_type arg3_type;
                typedef typename type_traits<Arg4>::parameter_type arg4_type;
                virtual Res operator()(arg1_type,arg2_type,arg3_type,arg4_type) = 0;
                virtual functor_impl* clone() const = 0;
            };
            
            template <typename Res>
            inline Res invoke(Res(func)()) { return func();}
            template <typename Res,typename Arg1>
            inline Res invoke(Res(func)(Arg1),typename type_traits<Arg1>::parameter_type arg1) { return func(arg1); }
            template <typename Res,typename Arg1,typename Arg2>
            inline Res invoke(Res(func)(Arg1,Arg2),typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2) { return func(arg1,arg2); }
            template <typename Res,typename Arg1,typename Arg2,typename Arg3>
            inline Res invoke(Res(func)(Arg1,Arg2,Arg3),typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2,
                              typename type_traits<Arg3>::parameter_type arg3) { return func(arg1,arg2,arg3); }
            template <typename Res,typename Arg1,typename Arg2,typename Arg3,typename Arg4>
            inline Res invoke(Res(func)(Arg1,Arg2,Arg3,Arg4),typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2,
                              typename type_traits<Arg3>::parameter_type arg3,
                              typename type_traits<Arg4>::parameter_type arg4) { return func(arg1,arg2,arg3,arg4); }
            template <typename Res,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5>
            inline Res invoke(Res(func)(Arg1,Arg2,Arg3,Arg4,Arg5),typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2,
                              typename type_traits<Arg3>::parameter_type arg3,
                              typename type_traits<Arg4>::parameter_type arg4,
                              typename type_traits<Arg5>::parameter_type arg5) { return func(arg1,arg2,arg3,arg4,arg5); }
            
            template <typename Obj,typename Res>
            inline Res invoke(Res(Obj::*func)(),Obj* obj) { return ((*obj).*func)();}
            template <typename Obj,typename Res,typename Arg1>
            inline Res invoke(Res(Obj::*func)(Arg1),Obj* obj,typename type_traits<Arg1>::parameter_type arg1) { return ((*obj).*func)(arg1); }
            template <typename Obj,typename Res,typename Arg1,typename Arg2>
            inline Res invoke(Res(Obj::*func)(Arg1,Arg2),Obj* obj,typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2) { return ((*obj).*func)(arg1,arg2); }
            template <typename Obj,typename Res,typename Arg1,typename Arg2,typename Arg3>
            inline Res invoke(Res(Obj::*func)(Arg1,Arg2,Arg3),Obj* obj,typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2,
                              typename type_traits<Arg3>::parameter_type arg3) { return ((*obj).*func)(arg1,arg2,arg3); }
            template <typename Obj,typename Res,typename Arg1,typename Arg2,typename Arg3,typename Arg4>
            inline Res invoke(Res(Obj::*func)(Arg1,Arg2,Arg3,Arg4),Obj* obj,typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2,
                              typename type_traits<Arg3>::parameter_type arg3,
                              typename type_traits<Arg4>::parameter_type arg4) { return ((*obj).*func)(arg1,arg2,arg3,arg4); }
            template <typename Obj,typename Res,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5>
            inline Res invoke(Res(Obj::*func)(Arg1,Arg2,Arg3,Arg4,Arg5),Obj* obj,typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2,
                              typename type_traits<Arg3>::parameter_type arg3,
                              typename type_traits<Arg4>::parameter_type arg4,
                              typename type_traits<Arg5>::parameter_type arg5) { return ((*obj).*func)(arg1,arg2,arg3,arg4,arg5); }
            
            template <typename Obj,typename Res>
            inline Res invoke(Res(Obj::*func)()const ,const Obj* obj) { return ((*obj).*func)();}
            template <typename Obj,typename Res,typename Arg1>
            inline Res invoke(Res(Obj::*func)(Arg1)const ,const Obj* obj,typename type_traits<Arg1>::parameter_type arg1) { return ((*obj).*func)(arg1); }
            template <typename Obj,typename Res,typename Arg1,typename Arg2>
            inline Res invoke(Res(Obj::*func)(Arg1,Arg2)const,const Obj* obj,typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2) { return ((*obj).*func)(arg1,arg2); }
            template <typename Obj,typename Res,typename Arg1,typename Arg2,typename Arg3>
            inline Res invoke(Res(Obj::*func)(Arg1,Arg2,Arg3)const,const Obj* obj,typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2,
                              typename type_traits<Arg3>::parameter_type arg3) { return ((*obj).*func)(arg1,arg2,arg3); }
            template <typename Obj,typename Res,typename Arg1,typename Arg2,typename Arg3,typename Arg4>
            inline Res invoke(Res(Obj::*func)(Arg1,Arg2,Arg3,Arg4)const,const Obj* obj,typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2,
                              typename type_traits<Arg3>::parameter_type arg3,
                              typename type_traits<Arg4>::parameter_type arg4) { return ((*obj).*func)(arg1,arg2,arg3,arg4); }
            template <typename Obj,typename Res,typename Arg1,typename Arg2,typename Arg3,typename Arg4,typename Arg5>
            inline Res invoke(Res(Obj::*func)(Arg1,Arg2,Arg3,Arg4,Arg5)const,const Obj* obj,typename type_traits<Arg1>::parameter_type arg1,
                              typename type_traits<Arg2>::parameter_type arg2,
                              typename type_traits<Arg3>::parameter_type arg3,
                              typename type_traits<Arg4>::parameter_type arg4,
                              typename type_traits<Arg4>::parameter_type arg5) { return ((*obj).*func)(arg1,arg2,arg3,arg4,arg5); }
            
            template <class ParentFunctor,typename Func>
            class functor_handler : public ParentFunctor::impl_type{
            private:
                typedef typename ParentFunctor::impl_type base_type;
            public:
                typedef typename base_type::result_type result_type;
                typedef typename base_type::arg1_type arg1_type;
                typedef typename base_type::arg2_type arg2_type;
                typedef typename base_type::arg3_type arg3_type;
                typedef typename base_type::arg4_type arg4_type;
                typedef typename base_type::arg5_type arg5_type;
                
                base_type* clone() const { return new functor_handler(m_func); }
                
                explicit functor_handler(const Func& func) : m_func(func) { }
                virtual ~functor_handler() {}
                
                result_type operator () () { return invoke(m_func); }
                result_type operator () (arg1_type arg1) { return invoke(m_func,arg1); }
                result_type operator () (arg1_type arg1,arg2_type arg2) { return invoke(m_func,arg1,arg2); }
                result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3) { return invoke(m_func,arg1,arg2,arg3); }
                result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4) { return invoke(m_func,arg1,arg2,arg3,arg4); }
                result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4,arg5_type arg5) { return invoke(m_func,arg1,arg2,arg3,arg4,arg5); }
            private:
                Func m_func;
            };
            template <class ParentFunctor,typename Func,typename P>
            class ptr_functor_handler : public ParentFunctor::impl_type{
            private:
                typedef typename ParentFunctor::impl_type base_type;
            public:
                typedef typename base_type::result_type result_type;
                typedef typename base_type::arg1_type arg1_type;
                typedef typename base_type::arg2_type arg2_type;
                typedef typename base_type::arg3_type arg3_type;
                typedef typename base_type::arg4_type arg4_type;
                typedef typename base_type::arg5_type arg5_type;
                
                base_type* clone() const { return new ptr_functor_handler(m_func,m_ptr); }
                
                explicit ptr_functor_handler(const Func& func,const shared_ptr<P>& p) : m_func(func),m_ptr(p) { }
                virtual ~ptr_functor_handler() {}
                
                result_type operator () () { return invoke(m_func,m_ptr.get()); }
                result_type operator () (arg1_type arg1) { return invoke(m_func,m_ptr.get(),arg1); }
                result_type operator () (arg1_type arg1,arg2_type arg2) { return invoke(m_func,m_ptr.get(),arg1,arg2); }
                result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3) { return invoke(m_func,m_ptr.get(),arg1,arg2,arg3); }
                result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4) { return invoke(m_func,m_ptr.get(),arg1,arg2,arg3,arg4); }
                //result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4,arg5_type arg5) { return invoke(m_func,arg1,arg2,arg3,arg4,arg5); }
            private:
                Func m_func;
                shared_ptr<P> m_ptr;
            };
            
            
        } /* namespace impl */
        
        /// function object
        template <typename Proto = void()> class function {
        public:
            typedef typename function_traits<Proto*>::result_type result_type;
            typedef impl::functor_impl<result_type,
            typename function_traits<Proto*>::arg1_type,
            typename function_traits<Proto*>::arg2_type,
            typename function_traits<Proto*>::arg3_type,
            typename function_traits<Proto*>::arg4_type,
            typename function_traits<Proto*>::arg5_type> impl_type;
            typedef typename impl_type::arg1_type arg1_type;
            typedef typename impl_type::arg2_type arg2_type;
            typedef typename impl_type::arg3_type arg3_type;
            typedef typename impl_type::arg4_type arg4_type;
            typedef typename impl_type::arg5_type arg5_type;
            function() : m_impl(0) {}
            function    (const function& rhs) : m_impl( rhs.m_impl.get() ? rhs.m_impl->clone() : 0 ) {}
            
            explicit function( std::auto_ptr<impl_type> impl) : m_impl(impl) {}
            /// ctor from function-object or free function
            template <typename Func>
            explicit function(Func func) : m_impl(new impl::functor_handler<function,Func>(func)) {}
            /// ctor from object and object function
            /*template <class PtrObj, typename MemFn>
             function(const PtrObj& obj,MemFn func) : m_impl(new impl::mem_func_handler<function,PtrObj,MemFn>(obj,func)) {}
             */
            function& operator=(const function& rhs)
            {
                function copy(rhs);
                // swap auto_ptrs by hand
                impl_type* p = m_impl.release();
                m_impl.reset(copy.m_impl.release());
                copy.m_impl.reset(p);
                return *this;
            }
            
            typedef impl_type * (std::auto_ptr<impl_type>::*unspecified_bool_type)() const;
            operator unspecified_bool_type() const {
                return m_impl.get() ? &std::auto_ptr<impl_type>::get : 0;
            }
            
            
            
            bool empty() const
            {
                return m_impl.get() == 0;
            }
            void clear()
            {
                m_impl.reset(0);
            }
            
            result_type operator () () const {
                sb_assert(!empty());
                return (*m_impl)();
            }
            result_type operator () (arg1_type arg1) const {
                sb_assert(!empty());
                return (*m_impl)(arg1);
            }
            result_type operator () (arg1_type arg1,arg2_type arg2) const {
                sb_assert(!empty());
                return (*m_impl)(arg1,arg2);
            }
            result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3) const {
                sb_assert(!empty());
                return (*m_impl)(arg1,arg2,arg3);
            }
            result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4) const {
                sb_assert(!empty());
                return (*m_impl)(arg1,arg2,arg3,arg4);
            }
            result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4,arg5_type arg5) const {
                sb_assert(!empty());
                return (*m_impl)(arg1,arg2,arg3,arg4,arg5);
            }
        private:
            std::auto_ptr<impl_type>	m_impl;
        };
        
        
        //// bind first
        namespace impl {
            
            template <class T>
            struct proto_help {
                typedef typename type_traits<T>::referred_type result;
            };
            template <class T>
            struct proto_help<T&> {
                typedef T& result;
            };
            template <class T>
            struct proto_help<const T&> {
                typedef T result;
            };
            
            template <class Fctor> struct binder_first_traits;
            template <typename Proto>
            struct binder_first_traits< function<Proto> > {
                typedef function<Proto> original_function;
                typedef typename original_function::result_type result_type;
                typedef typename original_function::arg1_type original_arg;
                typedef typename original_function::arg2_type arg1_type;
                typedef typename original_function::arg3_type arg2_type;
                typedef typename original_function::arg4_type arg3_type;
                typedef typename original_function::arg5_type arg4_type;
                typedef null_type arg5_type;
                
                typedef typename proto_build<result_type,
                typename proto_help<arg1_type>::result,
                typename proto_help<arg2_type>::result,
                typename proto_help<arg3_type>::result,
                typename proto_help<arg4_type>::result,
                typename proto_help<arg5_type>::result>::result bound_proto;
                typedef function<bound_proto> bound_function;
                typedef typename bound_function::impl_type impl_type;
            };
            
            template <class T>
            struct binder_first_bound_storage {
                typedef typename type_traits<T>::referred_type storage;
            };
            
            template <class OriginalFunction,class arg_storage>
            class binder_first : public binder_first_traits<OriginalFunction>::impl_type {
            private:
                typedef typename binder_first_traits<OriginalFunction>::impl_type base_type;
                typedef typename binder_first_traits<OriginalFunction>::original_arg original_arg;
                /*typedef typename binder_first_bound_storage<original_arg>::storage arg_storage;*/
            public:
                typedef typename base_type::result_type result_type;
                typedef typename base_type::arg1_type arg1_type;
                typedef typename base_type::arg2_type arg2_type;
                typedef typename base_type::arg3_type arg3_type;
                typedef typename base_type::arg4_type arg4_type;
                typedef typename base_type::arg5_type arg5_type;
                binder_first(const OriginalFunction& func,typename type_traits<arg_storage>::parameter_type arg) : m_func(func),m_arg(arg) {}
                virtual ~binder_first() {}
                virtual base_type* clone() const { return new binder_first(m_func,m_arg);}
                
                result_type operator () () { return m_func(m_arg); }
                result_type operator () (arg1_type arg1) { return m_func(m_arg,arg1); }
                result_type operator () (arg1_type arg1,arg2_type arg2) { return m_func(m_arg,arg1,arg2); }
                result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3) { return m_func(m_arg,arg1,arg2,arg3); }
                result_type operator () (arg1_type arg1,arg2_type arg2,arg3_type arg3,arg4_type arg4) { return m_func(m_arg,arg1,arg2,arg3,arg4); }
            private:
                OriginalFunction m_func;
                arg_storage m_arg;
            };
        }
        
        
        template <class Function,class Arg1>
        inline typename impl::binder_first_traits<Function>::bound_function
        bind_first(Function func,Arg1 arg) {
            typedef typename type_traits<Arg1>::referred_type arg_storage;
            typedef typename impl::binder_first_traits<Function>::bound_function out_func;
            return out_func(std::auto_ptr<typename out_func::impl_type>(
                                                                        new impl::binder_first<Function,arg_storage>(func,arg)));
        }
        
        
        namespace placeholders {
            struct pl1{};
            inline pl1* _1() { return 0;}
        }
        
        
        
        /// member 0 args
        template <class Res,class Obj,class O>
        inline function<Res()> bind(
                                    Res(Obj::*func)(),O* arg1) {
            return bind_first(function<Res(Obj*)>(func),arg1);
        }
        template <class Res,class Obj,class O>
        inline function<Res()> bind(
                                    Res(Obj::*func)(),const shared_ptr<O>& o) {
            typedef impl::ptr_functor_handler<function<Res()>,Res(Obj::*)(),O> handler;
            typedef typename function<Res()>::impl_type impl_type;
            return function<Res()>( std::auto_ptr< impl_type >(  new handler(func,o))  );
        }
        template <class Res,class Obj,class O>
        inline function<Res()> bind(
                                    Res(Obj::*func)()const,const O* arg1) {
            return bind_first(function<Res(const Obj*)>(func),arg1);
        }
        template <class Res,class Obj,class O>
        inline function<Res()> bind(
                                    Res(Obj::*func)()const,const shared_ptr<O>& o) {
            typedef impl::ptr_functor_handler<function<Res()>,Res(Obj::*)()const,O> handler;
            typedef typename function<Res()>::impl_type impl_type;
            return function<Res()>( std::auto_ptr< impl_type >(  new handler(func,o))  );
        }
        
        
        
        
        
        
        /// member with 1 arg
        template <typename Res,typename Obj,typename O,typename Arg1,typename P1>
        inline function<Res()> bind(
                                    Res(Obj::*func)(Arg1),O* o,P1 p1) {
            return bind_first(bind_first(function<Res(Obj*,Arg1)>(func),o),p1);
        }
        template <typename Res,typename Obj,typename O,typename Arg1,typename P1>
        inline function<Res()> bind(
                                    Res(Obj::*func)(Arg1) const,const O* o,P1 p1) {
            return bind_first(bind_first(function<Res(const Obj*,Arg1)>(func),o),p1);
        }
        
        template <typename Res,typename Obj,typename O,typename Arg1>
        inline function<Res(typename impl::proto_help<Arg1>::result)> bind(
                                                                           Res(Obj::*func)(Arg1),O* o,placeholders::pl1*(*)()) {
            return bind_first(function<Res(Obj*,typename impl::proto_help<Arg1>::result)>(func),o);
        }
        template <typename Res,typename Obj,typename O,typename Arg1>
        inline function<Res(typename impl::proto_help<Arg1>::result)> bind(
                                                                           Res(Obj::*func)(Arg1)const,const shared_ptr<O>& o,placeholders::pl1*(*)()) {
            typedef impl::ptr_functor_handler<function<Res(typename impl::proto_help<Arg1>::result)>,Res(Obj::*)(Arg1)const,O> handler;
            typedef typename function<Res(typename impl::proto_help<Arg1>::result)>::impl_type impl_type;
            return function<Res(typename impl::proto_help<Arg1>::result)>( std::auto_ptr< impl_type >(  new handler(func,o))  );
        }
        template <typename Res,typename Obj,typename O,typename Arg1>
        inline function<Res(typename impl::proto_help<Arg1>::result)> bind(
                                                                           Res(Obj::*func)(Arg1) const,const O* o,placeholders::pl1*(*)()) {
            return bind_first(function<Res(const Obj*,typename impl::proto_help<Arg1>::result)>(func),o);
        }
        /// member with 2 arg
        template <typename Res,typename Obj,typename O,typename Arg1,typename P1,typename Arg2,typename P2>
        inline function<Res()> bind(
                                    Res(Obj::*func)(Arg1,Arg2),O* o,P1 p1,P2 p2) {
            return bind_first(bind_first(bind_first(function<Res(Obj*,typename impl::proto_help<Arg1>::result,typename impl::proto_help<Arg2>::result)>(func),o),p1),p2);
        }
        template <typename Res,typename Obj,typename O,typename Arg1,typename P1,typename Arg2,typename P2>
        inline function<Res()> bind(
                                    Res(Obj::*func)(Arg1,Arg2)const,const O* o,P1 p1,P2 p2) {
            return bind_first(bind_first(bind_first(function<Res(const Obj*,typename impl::proto_help<Arg1>::result,typename impl::proto_help<Arg2>::result)>(func),o),p1),p2);
        }
        template <typename Res,typename Obj,typename O,typename Arg1,typename P1,typename Arg2>
        inline function<Res(typename impl::proto_help<Arg2>::result)> bind(
                                                                           Res(Obj::*func)(Arg1,Arg2),O* o,P1 p1,placeholders::pl1*(*)()) {
            return bind_first(bind_first(function<Res(Obj*,typename impl::proto_help<Arg1>::result,typename impl::proto_help<Arg2>::result )>(func),o),p1);
        }
        /// member with 3 arg
        template <typename Res,typename Obj,typename O,typename Arg1,typename P1,typename Arg2,typename P2,typename Arg3,typename P3>
        inline function<Res()> bind(
                                    Res(Obj::*func)(Arg1,Arg2,Arg3),O* o,P1 p1,P2 p2,P3 p3) {
            return bind_first(bind_first(bind_first(bind_first(function<Res(Obj*,typename impl::proto_help<Arg1>::result,typename impl::proto_help<Arg2>::result,typename impl::proto_help<Arg3>::result)>(func),o),p1),p2),p3);
        }
        
        /// member with 4 arg
        template <typename Res,typename Obj,typename O,typename Arg1,typename P1,typename Arg2,typename P2,typename Arg3,typename P3,typename Arg4,typename P4>
        inline function<Res()> bind(Res(Obj::*func)(Arg1,Arg2,Arg3,Arg4),O* o,P1 p1,P2 p2,P3 p3,P4 p4) {
            return bind_first(bind_first(bind_first(bind_first(bind_first(function<Res(Obj*,typename impl::proto_help<Arg1>::result,typename impl::proto_help<Arg2>::result,typename impl::proto_help<Arg3>::result,typename impl::proto_help<Arg4>::result)>(func),o),p1),p2),p3),p4);
        }
        
        /// free function with 1 args
        template <typename Res,typename Arg1,typename P1>
        inline function<Res()> bind(Res(*func)(Arg1),P1 p1) {
            return bind_first(function<Res(typename impl::proto_help<Arg1>::result)>(func),p1);
        }
        /// free function with 2 args
        template <typename Res,typename Arg1,typename P1,typename Arg2,typename P2>
        inline function<Res()> bind(Res(*func)(typename impl::proto_help<Arg1>::result,typename impl::proto_help<Arg2>::result),P1 p1,P2 p2) {
            return bind_first(bind_first(function<Res(typename impl::proto_help<Arg1>::result,typename impl::proto_help<Arg2>::result)>(func),p1),p2);
        }
        template <typename Res,typename Arg1,typename P1,typename Arg2>
        inline function<Res(typename impl::proto_help<Arg2>::result)> bind(Res(*func)(Arg1,Arg2),P1 p1,placeholders::pl1*(*)()) {
            return bind_first(function<Res(typename impl::proto_help<Arg1>::result,typename impl::proto_help<Arg2>::result)>(func),p1);
        }
        
        /// free function with 3 args
        template <typename Res,typename Arg1,typename P1,typename Arg2,typename P2,typename Arg3>
        inline function<Res(typename impl::proto_help<Arg3>::result)> bind(Res(*func)(Arg1,Arg2,Arg3),P1 p1,P2 p2,placeholders::pl1*(*)()) {
            return bind_first(bind_first(function<Res(typename impl::proto_help<Arg1>::result,typename impl::proto_help<Arg2>::result,typename impl::proto_help<Arg3>::result)>(func),p1),p2);
        }
        
    }


#endif /*SB_FUNCTION_H*/
