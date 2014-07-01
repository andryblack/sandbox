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
    
    class ref_countered_base {
    private:
        typedef unsigned int counter_t;
        counter_t   counter;
    public:
        void add_ref() {
            ++counter;
        }
        void remove_ref() {
            sb_assert(counter!=0);
            --counter;
            if (counter==0) {
                delete this;
            }
        }
        bool unique() const { return counter == 1; }
        counter_t& internal_counter() { return counter; }
    protected:
        ref_countered_base() : counter(0) {}
        virtual ~ref_countered_base() {
            sb_assert(counter==0);
        }
        ref_countered_base(const ref_countered_base&) : counter(0) {}
        ref_countered_base& operator = (const ref_countered_base&) {
            return *this;
        }
    };
    
    class ref_countered_base_not_copyable : public ref_countered_base {
    protected:
        ref_countered_base_not_copyable() : ref_countered_base() {}
    private:
        ref_countered_base_not_copyable( const ref_countered_base_not_copyable& );
        ref_countered_base_not_copyable& operator = (const ref_countered_base_not_copyable&);
    };
    
}

#endif /*SB_POINTER_H*/
