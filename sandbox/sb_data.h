//
//  sb_data.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 12/29/12.
//  Copyright (c) 2012 Andrey Kunitsyn. All rights reserved.
//

#ifndef SB_DATA_H_INCLUDED
#define SB_DATA_H_INCLUDED

#include <sbstd/sb_vector.h>
#include <sbstd/sb_string.h>
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
    
    class StringData : public RefCounter<GHL::Data> {
    private:
        sb::string  m_data;
    public:
        StringData() {}
        explicit StringData(const sb::string& d) : m_data(d) {}
        /// clone data
        virtual GHL::Data* GHL_CALL  Clone() const {
            return new StringData(m_data);
        }
        /// Data size
        virtual GHL::UInt32 GHL_CALL	GetSize() const {
            return GHL::UInt32(m_data.length());
        }
        /// Const data ptr
        virtual const GHL::Byte* GHL_CALL	GetData() const {
            return reinterpret_cast<const GHL::Byte*>(m_data.c_str());
        }
        /// set data
        virtual void GHL_CALL	SetData( GHL::UInt32 offset, const GHL::Byte* data, GHL::UInt32 size ) {
            const char* src = reinterpret_cast<const char*>(data);
            size_t nsize = offset+size;
            if (m_data.size()<nsize) {
                m_data.resize(nsize);
            }
            std::copy(src,src+size,m_data.begin()+offset);
        }
        sb::string& string() { return m_data; }
        const sb::string& string() const { return m_data; }
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

#endif /*SB_DATA_H_INCLUDED*/
