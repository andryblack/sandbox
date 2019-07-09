//
//  sb_ref_cntr.h
//  backgammon-osx
//
//  Created by Andrey Kunitsyn on 12/29/12.
//  Copyright (c) 2012 Andrey Kunitsyn. All rights reserved.
//

#ifndef SB_REF_COUNTER_H_INCLUDED
#define SB_REF_COUNTER_H_INCLUDED

#include <sbstd/sb_assert.h>
#include <ghl_ref_counter.h>
#include "meta/sb_meta.h"
#include <sbstd/sb_unique_ptr.h>

namespace Sandbox {
    
    template <class T>
    class RefCounter : public T {
    private:
        mutable GHL::UInt32  m_refs;
        /// noncopyable
        RefCounter( const RefCounter& );
        RefCounter& operator = (const RefCounter&);
    public:
        RefCounter() : m_refs(1) {
            
        }
        virtual ~RefCounter() {
            sb_assert(m_refs==0);
        }
        
        virtual void GHL_CALL AddRef() const {
            m_refs++;
        }
        
        virtual void GHL_CALL Release() const {
            sb_assert(m_refs!=0);
            m_refs--;
            if (m_refs==0) {
                delete this;
            }
        }
        
    };
    
    template <class T>
    class GHLObjectImpl : virtual public T, public meta::object {
    public:
        /// add reference
        virtual void GHL_CALL AddRef() const SB_OVERRIDE {
            add_ref();
        }
        /// release reference
        virtual void GHL_CALL Release() const SB_OVERRIDE {
            remove_ref();
        }
    };
    
    struct GHLRefCounterDeleter {
        void operator () ( const GHL::RefCounter* o ) const {
            o->Release();
        }
    };

    template <class T>
    class GHLObjectPtr : public sb::unique_ptr<T, GHLRefCounterDeleter> {
    public:
        GHLObjectPtr() {}
        explicit GHLObjectPtr(T* v) : sb::unique_ptr<T, GHLRefCounterDeleter>(v) {}
    };
    
}

#endif /*SB_REF_COUNTER_H_INCLUDED*/
