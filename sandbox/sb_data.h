//
//  sb_data.h
//  backgammon-osx
//
//  Created by Andrey Kunitsyn on 12/29/12.
//  Copyright (c) 2012 Andrey Kunitsyn. All rights reserved.
//

#ifndef backgammon_osx_sb_data_h
#define backgammon_osx_sb_data_h

#include <sbstd/sb_vector.h>
#include "sb_ref_cntr.h"
#include <ghl_data.h>
#include "meta/sb_meta.h"

namespace Sandbox {
    
    
    template <class T>
    class VectorData : public RefCounter<GHL::Data> {
    private:
        typedef sb::vector<T> container_t;
        container_t   m_data;
    public:
        
        typedef typename container_t::value_type value_type;
        
        VectorData(){}
        explicit VectorData( const container_t& data ) : m_data(data) {
            
        }
        container_t& vector() { return m_data; }
        const container_t& vector() const { return m_data; }
        
        /// Data size
		virtual GHL::UInt32 GHL_CALL	GetSize() const {
            return GHL::UInt32(m_data.size() * sizeof(T));
        }
		/// Const data ptr
		virtual const GHL::Byte* GHL_CALL	GetData() const {
            return reinterpret_cast<const GHL::Byte*>(&m_data[0]);
        }
		/// set data
		virtual void GHL_CALL	SetData( GHL::UInt32 offset, const GHL::Byte* data, GHL::UInt32 size ) {
            sb_assert((offset%sizeof(T))==0);
            sb_assert((size%sizeof(T))==0);
            const T* src = reinterpret_cast<const T*>(data);
            size_t nsize = offset/sizeof(T)+size/sizeof(T);
            if (m_data.size()<nsize) {
                m_data.resize(nsize);
            }
            std::copy(src,src+size/sizeof(T),m_data.begin()+offset/sizeof(T));
        }
        /// clone data
        virtual GHL::Data* GHL_CALL  Clone() const {
            return new VectorData(m_data);
        }
        
        void push_back( const value_type& v ) { m_data.push_back(v); }
        size_t size() const { return m_data.size(); }
    };
    
    class BinaryData : public meta::object {
        SB_META_OBJECT
    private:
        GHL::Data*  m_data;
    public:
        explicit BinaryData(GHL::Data* data) : m_data(data) {
            if (m_data) m_data->AddRef();
        }
        ~BinaryData() {
            if (m_data) {
                m_data->Release();
            }
        }
        GHL::Data* GetData() { return m_data; }
        const GHL::Data* GetData() const { return m_data; }
    };
    typedef sb::intrusive_ptr<BinaryData> BinaryDataPtr;
    
}

#endif
