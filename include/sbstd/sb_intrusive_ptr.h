/*
 *  sb_intrusive_ptr.h
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_INTRUSIVE_PTR_H
#define SB_INTRUSIVE_PTR_H


#include "sb_assert.h"
#include "sb_pointer.h"

namespace sb {
    template <class T> class intrusive_ptr;
    
    
    namespace implement {
    }
    
    template <class T> class intrusive_ptr {
    public:
        intrusive_ptr() : ptr(0) {}
        template <class U>
        explicit intrusive_ptr(U* ptr_) : ptr(ptr_) {
            if (ptr_)
                ptr_->add_ref();
        }
        ~intrusive_ptr() {
            release();
        }
        
        intrusive_ptr(const intrusive_ptr& other ) : ptr(other.get()) {
            if (ptr) ptr->add_ref();
        }
        
        template <class U>
        intrusive_ptr(const intrusive_ptr<U>& other ) : ptr(other.get()) {
            if (ptr) ptr->add_ref();
        }
        
        template <class U>
        intrusive_ptr(const intrusive_ptr<U>& other,const implement::static_cast_tag& ) : ptr(static_cast<T*>(other.get())) {
            if (ptr) ptr->add_ref();
        }
        template <class U>
        intrusive_ptr(const intrusive_ptr<U>& other,const implement::dynamic_cast_tag& ) : ptr(dynamic_cast<T*>(other.get())) {
            if (ptr) ptr->add_ref();
        }
        
        
        intrusive_ptr& operator = (const intrusive_ptr& other) {
            if (other.get()!=ptr) {
                release();
                ptr = other.get();
                if (ptr) ptr->add_ref();
            }
            return *this;
        }
        
        template <class U>
        intrusive_ptr& operator = (const intrusive_ptr<U>& other) {
            if (other.get()!=ptr) {
                release();
                ptr = other.get();
                if (ptr) ptr->add_ref();
            }
            return *this;
        }
        
        T* operator -> () const { sb_assert(ptr);return ptr; }
        
        T & operator*() const { sb_assert(ptr);return *ptr; }
        
        bool operator == (const int val) const {
            sb_assert(val==0);
            return ptr==(T*)(val);
        }
        
        bool operator == (const intrusive_ptr<T>& other) const {
            return (ptr == other.ptr);
        }
        bool operator != (const intrusive_ptr<T>& other) const {
            return (ptr != other.ptr);
        }
        bool operator < (const intrusive_ptr<T>& other) const {
            return (ptr < other.ptr);
        }
        
        T* get() const { return ptr;}
        
        typedef T* intrusive_ptr::*unspecified_bool_type;
        
        operator unspecified_bool_type() const {
            return ptr ? &intrusive_ptr::ptr : 0;
        }
        
        void reset() {
            release();
        }
        
        template <class U>
        void reset(U* v) {
            intrusive_ptr other(v);
            *this = other;
        }
        
    private:
        void release() {
            if (ptr) {
                ptr->remove_ref();
                ptr = 0;
            }
        }
        
        T* ptr;
    };
        
    template <class T,class U>
    inline intrusive_ptr<T> static_pointer_cast( const intrusive_ptr<U>& ptr) {
        return intrusive_ptr<T>(ptr,implement::static_cast_tag());
    }
    
    template <class T,class U>
    inline intrusive_ptr<T> dynamic_pointer_cast( const intrusive_ptr<U>& ptr) {
        return intrusive_ptr<T>(ptr,implement::dynamic_cast_tag());
    }
}

#endif /*SB_INTRUSIVE_PTR_H*/
