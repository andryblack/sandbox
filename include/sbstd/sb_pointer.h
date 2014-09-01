/*
 *  sb_pointer.h
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_POINTER_H
#define SB_POINTER_H

#include "sb_assert.h"

namespace sb {
    
    namespace implement {
        
        struct static_cast_tag{};
        struct dynamic_cast_tag{};
        struct empty_base {};
    }
    
    template <class T>
    class ref_counter : public T {
    private:
        typedef unsigned int counter_t;
        mutable counter_t   counter;
    public:
        void add_ref() const {
            ++counter;
        }
        void remove_ref() const {
            sb_assert(counter!=0);
            --counter;
            if (counter==0) {
                delete this;
            }
        }
        bool unique() const { return counter == 1; }
        counter_t& internal_counter() { return counter; }
    protected:
        ref_counter() : counter(0) {}
        virtual ~ref_counter() {
            sb_assert(counter==0);
        }
        ref_counter(const ref_counter&) : counter(0) {}
        ref_counter& operator = (const ref_counter&) {
            return *this;
        }
    };
    
    typedef ref_counter<implement::empty_base> ref_countered_base;
    
    template <class T>
    class ref_counter_not_copyable : public ref_counter<T> {
    protected:
        ref_counter_not_copyable() : ref_counter<T>() {}
    private:
        ref_counter_not_copyable( const ref_counter_not_copyable& );
        ref_counter_not_copyable& operator = (const ref_counter_not_copyable&);
    };
    
    typedef ref_counter_not_copyable<implement::empty_base> ref_countered_base_not_copyable;
    
}

#endif /*SB_POINTER_H*/
