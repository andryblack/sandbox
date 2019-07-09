/*
 *  sb_unique_ptr.h
 *
 *  Copyright 2019 andryblack. All rights reserved.
 *
 */

#ifndef SB_UNIQUE_PTR_H
#define SB_UNIQUE_PTR_H


#if __cplusplus >= 201103L
#include <memory>
namespace sb {
    using std::unique_ptr;
}
#else
#include "sb_assert.h"
#include "sb_shared_ptr.h"

namespace sb {
    

    template <class T,class D = implement::destroyer<T> > class unique_ptr {
    public:
        unique_ptr() : ptr(0), deleter() {}
        template <class U>
        explicit unique_ptr(U* ptr_) : ptr(ptr_),deleter() {}
        template <class U,class DU>
        unique_ptr(U* ptr_,DU deleter) : ptr(ptr_),deleter(deleter) {}
       
        T* release() {
            T* res = ptr;
            ptr = 0;
            return res;
        }
        T* get() const { return ptr;}
        
        T* operator -> () const { sb_assert(ptr);return ptr; }
        
        T & operator*() const { sb_assert(ptr);return *ptr; }
        
        typedef T* unique_ptr::*unspecified_bool_type;
        
        operator unspecified_bool_type() const {
            return ptr ? &unique_ptr::ptr : 0;
        }
        
        template <class U>
        void reset(U v) {
            T* old_ptr = ptr;
            ptr = v;
            if (old_ptr) {
                deleter(old_ptr);
            }
        }
        ~unique_ptr() {
            reset(static_cast<T*>(0));
        }
    private:
        unique_ptr(const unique_ptr& );
        unique_ptr& operator = (const unique_ptr&);
        T* ptr;
        D deleter;
    };
}

#endif

#endif /*SB_UNIQUE_PTR_H*/

