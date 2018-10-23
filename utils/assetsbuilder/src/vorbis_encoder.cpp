/**
 *  AssetsBuilder vorbis_encoder.h
 *  Copyright (c) 2017 Andrey Kunitsyn. All rights reserved.
 */

// based on https://svn.xiph.org/trunk/vorbis/examples/encoder_example.c

#include "vorbis_encoder.h"
#include "application.h"
#include <stdlib.h>

VorbisEncoder::VorbisEncoder(Application* app,bool stereo) : m_app(app),m_stereo(stereo) {
    
}

static int get_channels(GHL::SampleType st) {
    return (st==GHL::SAMPLE_TYPE_MONO_8 || st==GHL::SAMPLE_TYPE_MONO_16) ? 1 : 2;
}

#define READ_SAMPLES 1024

static float convert_sample16_s(const GHL::Byte* data,int ch,int s) {
    GHL::Int16 ss = static_cast<GHL::UInt16>(data[4*s+ch*2+0]) |
        (static_cast<GHL::UInt16>(data[4*s+ch*2+1]) << 8);
    return ss /32768.f;
}
static float convert_sample16_m(const GHL::Byte* data,int /*ch*/,int s) {
    GHL::Int16 ss = static_cast<GHL::UInt16>(data[2*s+0]) |
    (static_cast<GHL::UInt16>(data[2*s+1]) << 8);
    return ss /32768.f;
}
static float convert_sample8_s(const GHL::Byte* data,int ch,int s) {
    return (float(data[2*s+ch]) / 255.0f)*2-255.0f;
}
static float convert_sample8_m(const GHL::Byte* data,int /*ch*/,int s) {
    return (float(data[s]) / 255.0f)*2-255.0f;
}
typedef float(*convert_sample_t)(const GHL::Byte* data,int ch,int s);

static void _v_writestring(oggpack_buffer *o,const char *s, int bytes){
    
    while(bytes--){
        oggpack_write(o,*s++,8);
    }
}

bool VorbisEncoder::convert(GHL::SoundDecoder* decoder,GHL::WriteStream* output,int serial) {
    
    bool eos = false;
    
    vorbis_info_init(&m_vi);
    int channels = get_channels(decoder->GetSampleType());
//    /*
//     Encoding using a VBR quality mode.  The usable range is -.1
//     (lowest quality, smallest file) to 1. (highest quality, largest file).
//     Example quality mode .4: 44kHz stereo coupled, roughly 128kbps VBR
//     */
//    int ret = vorbis_encode_init_vbr(&m_vi,channels,decoder->GetFrequency(),0.4);
    /*
     Encoding using an average bitrate mode (ABR).
     example: 44kHz stereo coupled, average 128kbps VBR
    */
    int ret = vorbis_encode_init(&m_vi,m_stereo?channels:1,decoder->GetFrequency(),-1,m_app->get_sounds_encode_bps(),-1);
    if (ret != 0) {
        return false;
    }

    /* add a comment */
    vorbis_comment_init(&m_vc);
    vorbis_comment_add_tag(&m_vc,"ENCODER","assetsbuilder");
 
    /* set up the analysis state and auxiliary encoding storage */
    vorbis_analysis_init(&m_vd,&m_vi);
    vorbis_block_init(&m_vd,&m_vb);
    
    ogg_stream_init(&m_os,serial);
    
    /* Vorbis streams begin with three headers; the initial header (with
     most of the codec setup parameters) which is mandated by the Ogg
     bitstream spec.  The second header holds any comment fields.  The
     third header holds the bitstream codebook.  We merely need to
     make the headers, then pass them to libvorbis one at a time;
     libvorbis handles the additional Ogg bitstream constraints */
    
    {
        ogg_packet header;
        ogg_packet header_comm;
        ogg_packet header_code;
        
        vorbis_analysis_headerout(&m_vd,&m_vc,&header,&header_comm,&header_code);
        ogg_stream_packetin(&m_os,&header); /* automatically placed in its own
                                           page */
        
        const char* override_encoder_comment = m_app->get_vorbis_encoder_comment();
        if (override_encoder_comment) {
            oggpack_buffer opb;
            oggpack_writeinit(&opb);
            
            ogg_int32_t bytes = ::strlen(override_encoder_comment);
            
            /* preamble */
            oggpack_write(&opb,0x03,8);
            _v_writestring(&opb,"vorbis", 6);
            
            /* vendor */
            oggpack_write(&opb,bytes,32);
            _v_writestring(&opb,override_encoder_comment, bytes);
            
            /* comments */
            
            oggpack_write(&opb,m_vc.comments,32);
            if(m_vc.comments){
                int i;
                for(i=0;i<m_vc.comments;i++){
                    if(m_vc.user_comments[i]){
                        oggpack_write(&opb,m_vc.comment_lengths[i],32);
                        _v_writestring(&opb,m_vc.user_comments[i], m_vc.comment_lengths[i]);
                    }else{
                        oggpack_write(&opb,0,32);
                    }
                }
            }
            oggpack_write(&opb,1,1);
            
            header_comm.packet = static_cast<unsigned char*>(::malloc(oggpack_bytes(&opb)));
            header_comm.bytes = oggpack_bytes(&opb);
            memcpy(header_comm.packet,opb.buffer,oggpack_bytes(&opb));
        }
        
        ogg_stream_packetin(&m_os,&header_comm);
        ogg_stream_packetin(&m_os,&header_code);
        
        /* This ensures the actual
         * audio data will start on a new page, as per spec
         */
        while(!eos){
            int result=ogg_stream_flush(&m_os,&m_og);
            if(result==0)break;
            output->Write(reinterpret_cast<const GHL::Byte*>(m_og.header), m_og.header_len);
            output->Write(reinterpret_cast<const GHL::Byte*>(m_og.body), m_og.body_len);
        }
        
        if (override_encoder_comment) {
            ::free(header_comm.packet);
        }
    }
    
    convert_sample_t convert_sample = 0;
    switch (decoder->GetSampleType()) {
        case GHL::SAMPLE_TYPE_MONO_8:
            convert_sample = &convert_sample8_m;
            break;
        case GHL::SAMPLE_TYPE_MONO_16:
            convert_sample = &convert_sample16_m;
            break;
        case GHL::SAMPLE_TYPE_STEREO_8:
            convert_sample = &convert_sample8_s;
            break;
        case GHL::SAMPLE_TYPE_STEREO_16:
            convert_sample = &convert_sample16_s;
            break;
            
        default:
            return false;
            break;
    }
    GHL::Byte* samples_buffer = (GHL::Byte*)::malloc(READ_SAMPLES*channels*2);
    while (!eos) {
        GHL::UInt32 samples = decoder->ReadSamples(READ_SAMPLES, samples_buffer);
        
        if(samples==0){
            /* end of file.  this can be done implicitly in the mainline,
             but it's easier to see here in non-clever fashion.
             Tell the library we're at end of stream so that it can handle
             the last frame and mark end of stream in the output properly */
            vorbis_analysis_wrote(&m_vd,0);
            
        }else{
            /* data to encode */
            
            /* expose the buffer to submit data */
            float **buffer=vorbis_analysis_buffer(&m_vd,READ_SAMPLES);
            /* uninterleave samples */
            if (!m_stereo) {
                for (GHL::UInt32 s=0;s<samples;++s) {
                    float d = 0.0f;
                    for (int ch=0;ch<channels;++ch) {
                        d+=convert_sample(samples_buffer,ch,s);
                    }
                    d = d / channels;
                    buffer[0][s]=d;
                }
            } else {
                for (int ch=0;ch<channels;++ch) {
                    for (GHL::UInt32 s=0;s<samples;++s) {
                        buffer[ch][s]=convert_sample(samples_buffer,ch,s);
                    }
                }
            }
            /* tell the library how much we actually submitted */
            vorbis_analysis_wrote(&m_vd,samples);
            
        }
        
        /* vorbis does some data preanalysis, then divvies up blocks for
         more involved (potentially parallel) processing.  Get a single
         block for encoding now */
        while(vorbis_analysis_blockout(&m_vd,&m_vb)==1){
            /* analysis, assume we want to use bitrate management */
            vorbis_analysis(&m_vb,NULL);
            vorbis_bitrate_addblock(&m_vb);
            while(vorbis_bitrate_flushpacket(&m_vd,&m_op)){
                /* weld the packet into the bitstream */
                ogg_stream_packetin(&m_os,&m_op);
                /* write out pages (if any) */
                while(!eos){
                    int result=ogg_stream_pageout(&m_os,&m_og);
                    if(result==0)break;
                    output->Write(reinterpret_cast<const GHL::Byte*>(m_og.header), m_og.header_len);
                    output->Write(reinterpret_cast<const GHL::Byte*>(m_og.body), m_og.body_len);
                    
                    /* this could be set above, but for illustrative purposes, I do
                     it here (to show that vorbis does know where the stream ends) */
                    
                    if(ogg_page_eos(&m_og))eos=1;
                }
            }
        }
        
    }
    
    ::free(samples_buffer);
    
    ogg_stream_clear(&m_os);
    vorbis_block_clear(&m_vb);
    vorbis_dsp_clear(&m_vd);
    vorbis_comment_clear(&m_vc);
    vorbis_info_clear(&m_vi);

    
    return true;
}
