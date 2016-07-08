//
//  sb_sound.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 5/20/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_sound__
#define __sr_osx__sb_sound__

#include <sbstd/sb_intrusive_ptr.h>
#include "sb_notcopyable.h"
#include <ghl_types.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_list.h>
#include <sbstd/sb_string.h>

namespace GHL {
    struct SoundEffect;
    struct SoundInstance;
    struct Sound;
    struct MusicInstance;
}

namespace Sandbox {
    
    class Resources;
    
    class Sound;
    typedef sb::intrusive_ptr<Sound> SoundPtr;
    
    class SoundInstance : public sb::ref_countered_base_not_copyable {
    public:
        SoundInstance(const SoundPtr& effect, GHL::SoundInstance* instance, float crntVolume);
        ~SoundInstance();
        void Stop();
        void SetVolume(float vol);
        void SetPan(float pan);
        void FadeOut(float time);
        
        bool Update( float dt );
    private:
        SoundPtr    m_effect;
        GHL::SoundInstance* m_instance;
        float       m_current_volume;
        float       m_ref_volume;
        float       m_fade_speed;
        void FadeIn(float value,float time);
        friend  class Sound;
        void    UpdateVolume();
    };
    typedef sb::intrusive_ptr<SoundInstance> SoundInstancePtr;
    
    class SoundManager;

    class Sound : public sb::ref_countered_base_not_copyable {
    public:
        explicit Sound( SoundManager* mgr, GHL::SoundEffect* eff );
        ~Sound();
        void Play();
        void PlayEx(float vol,float pan);
        SoundInstancePtr    PlayExControl(float fadeIn,float vol,float pan);
    private:
        SoundManager*       m_mgr;
        GHL::SoundEffect*   m_effect;
        friend class SoundInstance;
    };
    
    
    class SoundManager : public NotCopyable {
    public:
        explicit SoundManager( );
        ~SoundManager();
        void    Init(GHL::Sound* snd, Resources* res);
        void    Deinit();
        void    Update(float dt);
        SoundPtr GetSound(const char* filename);
        void    SetSoundsDir(const char* dir);
        void    SetSoundsVolume( float v ) { m_sounds_volume = v; }
        float   GetSoundsVolume() const { return m_sounds_volume; }
        void    SetMusicVolume( float v);
        float   GetMusicVolume() const { return m_music_volume; }
        
        void PlayMusic(const char* filename,bool loop);
    private:
        friend class Sound;
        friend class SoundInstance;
        GHL::Sound* m_sound;
        Resources*  m_resources;
        sb::string  m_sounds_dir;
        float       m_sounds_volume;
        float       m_music_volume;
        sb::map<sb::string,SoundPtr>    m_sounds;
        typedef sb::list<SoundInstancePtr> SoundsList;
        SoundsList  m_fade_ins;
        SoundsList  m_fade_outs;
        GHL::MusicInstance* m_music;
    };
    
}

#endif /* defined(__sr_osx__sb_sound__) */
