#include "sb_multi_stream.h"

namespace Sandbox {
    
    
    MultiStream::StringDataField::StringDataField(const sb::string& data) : m_pos(0),m_data(data) {
        
    }
    GHL::UInt32 MultiStream::StringDataField::Read(GHL::Byte* dest,GHL::UInt32 bytes) {
        GHL::UInt32 tail = m_data.length() - m_pos;
        bytes = bytes < tail ? bytes : tail;
        memcpy(dest, m_data.c_str() + m_pos, bytes);
        m_pos += bytes;
        return bytes;
    }
    
    MultiStream::StreamDataField::StreamDataField(GHL::DataStream* ds) : m_ds(ds) {
        m_ds->AddRef();
    }
    MultiStream::StreamDataField::~StreamDataField() {
        Complete();
    }
    GHL::UInt32 MultiStream::StreamDataField::Read(GHL::Byte* dest,GHL::UInt32 bytes) {
        GHL::UInt32 readed = 0;
        while (bytes && !m_ds->Eof()) {
            GHL::UInt32 r = m_ds->Read(dest, bytes);
            dest += r;
            bytes -= r;
            readed += r;
        }
        return readed;
    }
    void MultiStream::StreamDataField::Complete() {
        if (m_ds) {
            m_ds->Release();
            m_ds = 0;
        }
    }

    
    MultiStream::MultiStream() {
        m_total_size = 0;
        m_writed = 0;
        m_current_field = 0;

    }
    

    void MultiStream::AddString(const char* str) {
        size_t len = ::strlen(str);
        m_fields.push_back(DataFieldPtr(new StringDataField(str)));
        m_total_size += len;
    }
    void MultiStream::AddString(const sb::string& str) {
        m_fields.push_back(DataFieldPtr(new StringDataField(str)));
        m_total_size += str.length();
    }
    void MultiStream::AddStream(GHL::DataStream* data) {
        data->Seek(0, GHL::F_SEEK_END);
        GHL::UInt32 file_size = data->Tell();
        data->Seek(0, GHL::F_SEEK_BEGIN);
        m_total_size += file_size;
        m_fields.push_back(DataFieldPtr(new StreamDataField(data)));
    }
    
    
    /// read data
    GHL::UInt32 GHL_CALL MultiStream::Read(GHL::Byte* dest,GHL::UInt32 bytes) {
        GHL::UInt32 readed = 0;
        while (bytes && m_current_field < m_fields.size()) {
            GHL::UInt32 r = m_fields[m_current_field]->Read(dest, bytes);
            if (r < bytes) {
                m_fields[m_current_field]->Complete();
                ++m_current_field;
            }
            m_writed += r;
            dest += r;
            bytes -= r;
            readed += r;
        }
        return readed;
    }
    /// tell
    GHL::UInt32 GHL_CALL MultiStream::Tell() const {
        return m_writed;

    }
    /// seek
    bool GHL_CALL MultiStream::Seek(GHL::Int32 offset,GHL::FileSeekType st) {
        if (offset == 0 && st == GHL::F_SEEK_END) {
            m_writed = m_total_size;
            return true;
        } else if (offset == 0 && st == GHL::F_SEEK_BEGIN) {
            m_writed = 0;
            return true;
        }
        return false;
    }
    /// End of file
    bool GHL_CALL MultiStream::Eof() const {
        return m_current_field > m_fields.size();
    }
}
