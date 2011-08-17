/*
 *  sb_shared_ptr.h
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_SHARED_PTR_H
#define SB_SHARED_PTR_H


#ifdef SB_HAVE_TR1
#include <tr1/memory>
namespace Sandbox {
    using std::tr1::shared_ptr;
    using std::tr1::weak_ptr;
    using std::tr1::enable_shared_from_this;
    using std::tr1::static_pointer_cast;
	//using std::tr1::null_ptr;
}
#else 

#include <cstring>
#include "sb_assert.h"

namespace Sandbox {
	template <class T> class weak_ptr;
    template <class T> class shared_ptr;
    template <class T> class enable_shared_from_this;
	
    namespace implement {
		
		
        template <class T> struct destroyer {
            static void destroy(const void *d) {
                delete reinterpret_cast<const T*>(d);
            }
        };
		
        typedef void (*deleter_t)(const void*);
		
        struct counter_t {
            counter_t(size_t refs_,size_t weaks_,deleter_t deleter_) :
			refs(refs_),weaks(weaks_),deleter(deleter_) {}
            size_t refs;
            size_t weaks;
            deleter_t deleter;
        };
		
        struct static_cast_tag{};
		
		
        template <class T,class Y>
        inline void shared_from_this_support( int /*fake*/,const enable_shared_from_this<T>* ptr, const shared_ptr<Y>* sptr ) {
            if (ptr) {
                const_cast<enable_shared_from_this<T>*>(ptr)->internal_get_weak() = weak_ptr<T>(*sptr);
            }
        }
        inline void shared_from_this_support( int /*fake*/,...) {
        }
    }
	
    template <class T> class shared_ptr {
    public:
        shared_ptr() : ptr(0), counter(0) {}
        template <class U>
        explicit shared_ptr(U* ptr_) : ptr(ptr_),counter(ptr ? new implement::counter_t(1,0,&implement::destroyer<U>::destroy) : 0) {
            implement::shared_from_this_support(0,ptr,this);
        }
        template <class U>
        shared_ptr(U* ptr_,implement::deleter_t deleter) : ptr(ptr_),counter(ptr ? new implement::counter_t(1,0,deleter) : 0) {
            implement::shared_from_this_support(0,ptr,this);
        }
        shared_ptr(T* ptr_,implement::counter_t* counter_) : ptr(ptr_),counter(ptr ? counter_ : 0) {
            if (this->counter)
                this->counter->refs++;
        }
        ~shared_ptr() {
            release();
        }
		
        shared_ptr(const shared_ptr& other ) : ptr(other.get()),counter(other.get_counter()) {
            if (counter) counter->refs++;
        }
		
        template <class U>
        shared_ptr(const shared_ptr<U>& other ) : ptr(other.get()),counter(other.get_counter()) {
            if (counter) counter->refs++;
        }
		
        template <class U>
        shared_ptr(const shared_ptr<U>& other,const implement::static_cast_tag& ) : ptr(static_cast<T*>(other.get())),counter(other.get_counter()) {
            if (counter) counter->refs++;
        }
		
		
        shared_ptr& operator = (const shared_ptr& other) {
            if ((other.get()!=ptr) || (other.get_counter()!=counter)) {
                release();
                ptr = other.get();
                counter = other.get_counter();
                if (counter) counter->refs++;
            }
            return *this;
        }
		
        template <class U>
        shared_ptr& operator = (const shared_ptr<U>& other) {
            if ((other.get()!=ptr) || (other.get_counter()!=counter)) {
                release();
                ptr = other.get();
                counter = other.get_counter();
                if (counter) counter->refs++;
            }
            return *this;
        }
		
        T* operator -> () const { sb_assert(ptr);return ptr; }
		
        bool operator == (const int val) const {
            sb_assert(val==0);
            return ptr==(T*)(val);
        }
		
        bool operator == (const shared_ptr<T>& other) const {
            return (ptr == other.ptr) && (counter == other.counter);
        }
        bool operator != (const shared_ptr<T>& other) const {
            return (ptr != other.ptr) || (counter != other.counter);
        }
		
        T* get() const { return ptr;}
		
        typedef T* shared_ptr::*unspecified_bool_type;
		
        operator unspecified_bool_type() const {
            return ptr ? &shared_ptr::ptr : 0;
        }
		
        void reset() {
            release();
        }
		
        implement::counter_t* get_counter() const { return counter;}
    private:
        void release() {
            if (counter) {
                sb_assert(counter->refs>0);
                if (counter->refs==1) {
                    if (counter->deleter && ptr) counter->deleter(ptr);
                }
                if (--counter->refs==0) {
                    if (counter->weaks==0) {
                        delete counter;
                    }
                }
            }
            ptr = 0;
            counter = 0;
        }
		
        T* ptr;
        implement::counter_t* counter;
    };
	
	
	
    template <class T> class weak_ptr {
    public:
        weak_ptr() : ptr(0), counter(0) {}
        ~weak_ptr() {
            release();
        }
		
        weak_ptr(const weak_ptr& other) : ptr(other.get_ptr()),counter(other.get_counter()) {
            if (counter) counter->weaks++;
        }
        template <class U>
        weak_ptr(const weak_ptr<U>& other) : ptr(other.get_ptr()),counter(other.get_counter()) {
            if (counter) counter->weaks++;
        }
        weak_ptr(const shared_ptr<T>& shared) : ptr(shared.get()),counter(shared.get_counter()) {
            if (counter) counter->weaks++;
        }
        template <class U>
        weak_ptr(const shared_ptr<U>& shared) : ptr(shared.get()),counter(shared.get_counter()) {
            if (counter) counter->weaks++;
        }
        weak_ptr& operator = (const weak_ptr& other) {
            if (&other!=this) {
                release();
                ptr = other.get_ptr();
                counter = other.get_counter();
                if (counter) counter->weaks++;
            }
            return *this;
        }
        template <class U>
        weak_ptr& operator = (const weak_ptr<U>& other) {
            if (&other!=this) {
                release();
                ptr = other.get_ptr();
                counter = other.get_counter();
                if (counter) counter->weaks++;
            }
            return *this;
        }
        weak_ptr& operator = (const shared_ptr<T>& shared) {
            release();
            ptr = shared.get();
            counter = shared.get_counter();
            if (counter) counter->weaks++;
            return *this;
        }
        template <class U>
        weak_ptr& operator = (const shared_ptr<U>& shared) {
            release();
            ptr = shared.get();
            counter = shared.get_counter();
            if (counter) counter->weaks++;
            return *this;
        }
        shared_ptr<T> lock() const {
            if (ptr && counter && counter->refs)
                return shared_ptr<T>(ptr,counter);
            return shared_ptr<T>();
        }
		
        implement::counter_t* get_counter() const { return counter;}
        T* get_ptr() const { return ptr;}
    private:
        void release() {
            if (counter) {
                sb_assert(counter->weaks>0);
                if (--counter->weaks==0) {
                    if (counter->refs==0) {
                        delete counter;
                    }
                }
            }
            ptr = 0;
            counter = 0;
        }
		
        T* ptr;
        implement::counter_t* counter;
    };
	
    template <class T> class enable_shared_from_this {
    public:
        shared_ptr<T> shared_from_this() { return weak.lock();}
    protected:
        enable_shared_from_this() : weak() {}
        ~enable_shared_from_this() {}
        enable_shared_from_this(const enable_shared_from_this&) {}
        enable_shared_from_this& operator = (const enable_shared_from_this& ) {
            return *this;
        }
    private:
        weak_ptr<T> weak;
    public:
        weak_ptr<T>& internal_get_weak()  { return weak;}
    };
	
    template <class T,class U>
    shared_ptr<T> static_pointer_cast( const shared_ptr<U>& ptr) {
        return shared_ptr<T>(ptr,implement::static_cast_tag());
    }
	
}
#endif /*SB_HAVE_TR1*/


#endif /*SB_SHARED_PTR_H*/