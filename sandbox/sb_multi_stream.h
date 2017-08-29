#ifndef __SB_MULTI_STREAM_H_INCLUDED_
#define __SB_MULTI_STREAM_H_INCLUDED_

#include <ghl_data_stream.h>
#include "sb_ref_cntr.h"
#include <sbstd/sb_string.h>
#include <sbstd/sb_vector.h>

namespace Sandbox {

    class MultiStream : public GHLObjectImpl<GHL::DataStream> {
    protected:
        class DataField : public sb::ref_countered_base_not_copyable {
        public:
            virtual ~DataField() {}
            virtual GHL::UInt32 Read(GHL::Byte* dest,GHL::UInt32 bytes) = 0;
            virtual void Complete() {}
        };
        class StringDataField : public DataField {
        private:
            GHL::UInt32 m_pos;
            sb::string m_data;
        public:
            explicit StringDataField(const sb::string& data);
            virtual GHL::UInt32 Read(GHL::Byte* dest,GHL::UInt32 bytes) SB_OVERRIDE;
        };
        class StreamDataField : public DataField {
        protected:
            GHL::DataStream* m_ds;
        public:
            explicit StreamDataField(GHL::DataStream* ds);
            virtual ~StreamDataField() SB_OVERRIDE;
            virtual GHL::UInt32 Read(GHL::Byte* dest,GHL::UInt32 bytes) SB_OVERRIDE;
            virtual void Complete() SB_OVERRIDE;
        };
        typedef sb::intrusive_ptr<DataField> DataFieldPtr;
        sb::vector<DataFieldPtr> m_fields;
        size_t m_current_field;
        
        GHL::UInt32 m_total_size;
        GHL::UInt32 m_writed;

    public:
        MultiStream();
        void AddString(const char* text);
        void AddString(const sb::string& text);
        void AddStream(GHL::DataStream* ds);
        
        /// DataStream
        /// read data
        virtual GHL::UInt32 GHL_CALL Read(GHL::Byte* dest,GHL::UInt32 bytes) SB_OVERRIDE;
        /// tell
        virtual GHL::UInt32 GHL_CALL Tell() const SB_OVERRIDE;
        /// seek
        virtual	bool GHL_CALL Seek(GHL::Int32 offset,GHL::FileSeekType st) SB_OVERRIDE;
        /// End of file
        virtual bool GHL_CALL Eof() const SB_OVERRIDE;
        
        GHL::UInt32 GetTotalSize() const { return m_total_size; }
    };

}


#endif /*__SB_MULTI_STREAM_H_INCLUDED_*/
