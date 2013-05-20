//
//  sb_sound.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 5/20/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_sound__
#define __sr_osx__sb_sound__

#include <sbtl/sb_shared_ptr.h>
#include "sb_notcopyable.h"
#include <ghl_types.h>
#include <sbtl/sb_map.h>
#include <sbtl/sb_list.h>
#include <sbtl/sb_string.h>

namespace GHL {
    struct SoundEffect;
    struct SoundInstance;
    struct Sound;
}

namespace Sandbox {
    
    class Resources;
    
    class Sound;
    typedef sb::shared_ptr<Sound> SoundPtr;
    
    class SoundInstance : public NotCopyable, public sb::enable_shared_from_this<SoundInstance> {
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
    typedef sb::shared_ptr<SoundInstance> SoundInstancePtr;
    
    class SoundManager;

    class Sound : public NotCopyable, public sb::enable_shared_from_this<Sound> {
    public:
        explicit Sound( SoundManager* mgr, GHL::SoundEffect* eff );
        ~Sound();
        
        SoundInstancePtr    Play() { return PlayEx(0.0f,1.0f,0.0f);}
        SoundInstancePtr    PlayEx(float fadeIn,float vol,float pan);
    private:
        SoundManager*       m_mgr;
        GHL::SoundEffect*   m_effect;
        friend class SoundInstance;
    };
    
    
    class SoundManager : public NotCopyable {
    public:
        explicit SoundManager( );
        void    Init(GHL::Sound* snd, Resources* res);
        void    Update(float dt);
        SoundPtr GetSound(const char* filename);
        void    SetSoundsDir(const char* dir);
        void    SetSoundsVolume( float v ) { m_sounds_volume = v; }
        float   GetSoundsVolume() const { return m_sounds_volume; }
        void    SetMusicVolume( float v);
        float   GetMusicVolume() const { return m_music_volume; }
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
    };
    
}

#endif /* defined(__sr_osx__sb_sound__) */
