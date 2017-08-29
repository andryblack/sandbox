//
//  sb_ref_cntr.h
//  backgammon-osx
//
//  Created by Andrey Kunitsyn on 12/29/12.
//  Copyright (c) 2012 Andrey Kunitsyn. All rights reserved.
//

#ifndef backgammon_osx_sb_ref_cntr_h
#define backgammon_osx_sb_ref_cntr_h

#include <sbstd/sb_assert.h>
#include <ghl_api.h>
#include "meta/sb_meta.h"

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

    
}

#endif
