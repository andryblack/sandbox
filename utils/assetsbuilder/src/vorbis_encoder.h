#ifndef _VORBIS_ENCODER_H_INCLUDED_
#define _VORBIS_ENCODER_H_INCLUDED_

/**
 *  AssetsBuilder vorbis_encoder.h
 *  Copyright (c) 2017 Andrey Kunitsyn. All rights reserved.
 */

#include <ogg/ogg.h>
#include <vorbis/vorbisenc.h>
#include <ghl_sound.h>
#include <ghl_data_stream.h>

class Application;
class VorbisEncoder {
private:
    Application*        m_app;
    ogg_stream_state    m_os;
    ogg_page            m_og;
    ogg_packet          m_op;
    vorbis_info         m_vi;
    vorbis_comment      m_vc;
    vorbis_dsp_state    m_vd;
    vorbis_block        m_vb;
public:
    explicit VorbisEncoder(Application* app);
public:
    bool convert(GHL::SoundDecoder* decoder,GHL::WriteStream* output,int serial);
};



#endif /*_VORBIS_ENCODER_H_INCLUDED_*/
