#include "sb_data_stream.h"

namespace Sandbox {
    
    
    SubDataStream::SubDataStream( GHL::DataStream* stream, GHL::UInt32 offset, GHL::UInt32 size) :
        m_offset(offset),m_size(size),m_position(0),m_parent(stream) {
        m_parent->AddRef();
    }
    SubDataStream::~SubDataStream() {
        m_parent->Release();
    }
    /// read data
    GHL::UInt32 GHL_CALL SubDataStream::Read(GHL::Byte* dest,GHL::UInt32 bytes) {
        if (bytes > (m_size-m_position)) {
            bytes = m_size-m_position;
        }
        if (bytes == 0)
            return 0;
        m_parent->Seek(m_offset+m_position, GHL::F_SEEK_BEGIN);
        GHL::UInt32 readed = m_parent->Read(dest, bytes);
        m_position += readed;
        return readed;
    }
    /// tell
    GHL::UInt32 GHL_CALL SubDataStream::Tell() const {
        return m_position;
    }
    /// seek
    bool GHL_CALL SubDataStream::Seek(GHL::Int32 offset,GHL::FileSeekType st) {
        if (st == GHL::F_SEEK_END) {
            m_position = m_size - offset;
        } else if (st == GHL::F_SEEK_BEGIN) {
            m_position = offset;
        } else {
            m_position += offset;
        }
        if (m_position > m_size) {
            m_position = m_size;
        }
        return true;
    }
    /// End of file
    bool GHL_CALL SubDataStream::Eof() const {
        return m_position == m_size;
    }
    
}
