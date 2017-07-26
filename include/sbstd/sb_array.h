/*
 *  sb_array.h
 *  SR
 *
 *  Created by Андрей Куницын on 13.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_ARRAY_H
#define SB_ARRAY_H

/**
 * @file sb_array.h
 * @brief array template
 */

#include "sb_assert.h"
#include <vector>

namespace sb
{
	/**
	 * array template
	 */
    template <typename T,size_t a_size>
    struct array
    {
        typedef T element_t;
        typedef T* element_ptr;
        typedef const T* const_element_ptr;
        typedef T& element_ref;
		
        typedef const T& get_type;
        typedef const T& set_type;
        enum
        {
            c_size = a_size,
        };
		
        T a[c_size];
		
        element_ref operator [] (size_t indx) { sb_assert(indx>=0 && indx<c_size); return a[indx];}
        get_type operator [] (size_t indx) const { sb_assert(indx>=0 && indx<c_size);return a[indx];}
		
        /**
         * @return number of elements
         */
        size_t size() const { return c_size;}
		
        typedef element_ptr iterator;
        typedef const_element_ptr const_iterator;
		
        iterator begin() { return a;}
        iterator end() { return begin()+size();}
        const_iterator begin() const { return a;}
        const_iterator end() const { return begin()+size();}
		
        element_ref front() { return *begin();}
        get_type front() const { return *begin();}
        
        bool operator == (const array& o) const {
            for (size_t i=0;i<c_size;++i) {
                if (o[i]!=a[i])
                    return false;
                return true;
            }
        }
    };
}

#endif /* SB_ARRAY_H*/
