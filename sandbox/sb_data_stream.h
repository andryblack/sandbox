#ifndef SB_DATA_STREAM_H_INCLUDED
#define SB_DATA_STREAM_H_INCLUDED

#include "sb_ref_cntr.h"
#include <ghl_data_stream.h>

namespace Sandbox {
    

    class SubDataStream : public Sandbox::RefCounter<GHL::DataStream> {
    private:
        GHL::UInt32 m_offset;
        GHL::UInt32 m_size;
        GHL::UInt32 m_position;
        GHL::DataStream* m_parent;
    public:
        SubDataStream( GHL::DataStream* stream, GHL::UInt32 offset, GHL::UInt32 size );
        ~SubDataStream();
        /// read data
        virtual GHL::UInt32 GHL_CALL Read(GHL::Byte* dest,GHL::UInt32 bytes);
        /// tell
        virtual GHL::UInt32 GHL_CALL Tell() const;
        /// seek
        virtual    bool GHL_CALL Seek(GHL::Int32 offset,GHL::FileSeekType st);
        /// End of file
        virtual bool GHL_CALL Eof() const;
    };
}

#endif /*SB_DATA_STREAM_H_INCLUDED*/
