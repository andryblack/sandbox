//
//  sb_sound.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 5/20/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_sound.h"
#include "sb_resources.h"
#include <ghl_sound.h>
#include <ghl_data_stream.h>
#include "sb_log.h"

namespace Sandbox {
    
    static const char* MODULE = "Sandbox:Sound";
    static const float SILENCE = 0.05f;
    
    Sound::Sound( SoundManager* mgr, GHL::SoundEffect* eff ) : m_mgr(mgr),m_effect(eff) {
        
        
    }
    
    Sound::~Sound() {
        if (m_effect) m_effect->Release();
    }
    
    SoundInstance::SoundInstance(const SoundPtr& effect, GHL::SoundInstance* instance, float crntVolume) : m_effect(effect),m_instance(instance),m_current_volume(crntVolume) {
        m_fade_speed = 0.0f;
        m_ref_volume = m_current_volume;
    }
    
    SoundInstance::~SoundInstance() {
        if (m_instance) {
            m_instance->Release();
        }
    }
    
    void    SoundInstance::UpdateVolume() {
        if (m_instance) {
            m_instance->SetVolume(m_current_volume);
        }
    }
    
    void SoundInstance::Stop() {
        if (m_instance) {
            m_instance->Stop();
            m_instance->Release();
        }
        m_instance = 0;
    }
    void SoundInstance::SetVolume(float vol) {
        m_current_volume = m_ref_volume = vol * m_effect->m_mgr->m_sounds_volume;
        m_fade_speed = 0.0f;
    }
    void SoundInstance::SetPan(float pan) {
        if (m_instance) {
            m_instance->SetPan(pan*100.0f);
        }
    }
    void SoundInstance::SetPitch(float pitch) {
        if (m_instance) {
            m_instance->SetPitch(pitch*100.0f);
        }
    }
    void SoundInstance::FadeOut(float time) {
        if (time==0.0f) {
            Stop();
            return;
        }
        m_ref_volume = 0.0f;
        m_fade_speed = m_current_volume / time;
        m_effect->m_mgr->m_fade_outs.push_back(SoundInstancePtr(this));
    }
    
    void SoundInstance::FadeIn(float value,float time) {
        m_ref_volume = value;
        m_fade_speed = (m_ref_volume-m_current_volume) / time;
    }
    
    bool SoundInstance::Update(  float dt ) {
        if (!m_instance) return true;
        if (m_fade_speed>0.0f) {
            m_current_volume += m_fade_speed;
            if (m_current_volume>=m_ref_volume) {
                m_current_volume = m_ref_volume;
            }
        } else if (m_fade_speed<0.0f) {
            m_current_volume += m_fade_speed;
            if (m_current_volume<=m_ref_volume) {
                m_current_volume = m_ref_volume;
                if (m_current_volume==0.0f) {
                    Stop();
                }
            }
        }
        UpdateVolume();
        return m_current_volume == m_ref_volume;
    }
    
    
    MusicInstance::MusicInstance(  GHL::MusicInstance* mus) :
    m_music(mus),
    m_volume(1.0f),
    m_fade_volume(1.0f),
    m_fade_speed(0.0f) {
        
    }
    MusicInstance::~MusicInstance() {
        if (m_music) {
            m_music->Release();
            m_music = 0;
        }
    }
    
    void MusicInstance::Play(bool loop) {
        if (m_music) {
            m_music->SetVolume(m_volume * m_fade_volume);
            m_music->Play(loop);
        }
    }
    void MusicInstance::Pause() {
        if (m_music) {
            m_music->Pause();
        }
    }
    void MusicInstance::Resume() {
        if (m_music) {
            m_music->Resume();
        }
    }
    
    void MusicInstance::Stop() {
        if (m_music) {
            m_music->Stop();
        }
    }
    void MusicInstance::FadeOut(float time) {
        m_fade_speed = -1.0f / time;
    }
    void MusicInstance::FadeIn(float time) {
        m_fade_volume = 0.0f;
        m_fade_speed = 1.0f;
        if (m_music) m_music->SetVolume(m_volume * m_fade_volume);
    }
    
    bool MusicInstance::Update( float dt ) {
        if (!m_music) return true;
        if (m_fade_speed!=0.0f) {
            m_fade_volume += m_fade_speed * dt;
            if (m_fade_volume < 0.0f) {
                m_fade_volume = 0.0f;
                if (m_music) m_music->Stop();
                return true;
            }
            if (m_fade_volume > 1.0f) {
                m_fade_volume = 1.0f;
                m_fade_speed = 0.0f;
            }
            if (m_music) m_music->SetVolume(m_volume * m_fade_volume);
        }
        return (m_volume * m_fade_volume) <= SILENCE;
    }
    void MusicInstance::SetVolume( float vol ) {
        m_volume = vol;
        if (m_music) m_music->SetVolume(m_volume * m_fade_volume);
    }
    
    
    void    Sound::Play() {
        if (!m_effect) return;
        if (m_mgr->m_sounds_volume > SILENCE)
            m_mgr->m_sound->PlayEffect(m_effect,m_mgr->m_sounds_volume,0.0f,0);
    }

    void    Sound::PlayEx(float vol,float pan) {
        if (!m_effect) return;
        vol *= m_mgr->m_sounds_volume;
        if (vol > SILENCE)
            m_mgr->m_sound->PlayEffect(m_effect,vol,pan*100.0f,0);
    }
    
    void Sound::Release() {
        if (m_effect) {
            m_effect->Release();
            m_effect = 0;
        }
    }
    
    SoundInstancePtr    Sound::PlayExControl(float fadeIn,float vol,float pan) {
        if (!m_effect) return SoundInstancePtr(new SoundInstance(SoundPtr(this),static_cast<GHL::SoundInstance*>(0),0.0f));
        float initialVol = vol;
        if (fadeIn!=0.0f) {
            initialVol = 0;
        }
        initialVol *= m_mgr->m_sounds_volume;
        vol *= m_mgr->m_sounds_volume;
        if (vol > SILENCE) {
            GHL::SoundInstance* instance = 0;
            m_mgr->m_sound->PlayEffect(m_effect,initialVol,pan*100.0f,&instance);
            SoundInstancePtr res(new SoundInstance(SoundPtr(this),instance,initialVol));
            if (fadeIn!=0.0f) {
                res->FadeIn(vol, fadeIn);
                m_mgr->m_fade_ins.push_back(res);
            }
            return res;
        }
        return SoundInstancePtr(new SoundInstance(SoundPtr(this),static_cast<GHL::SoundInstance*>(0),0.0f));
    }
    
    SoundManager::SoundManager( ) : m_sound(0),m_resources(0) {
        m_sounds_volume = 70.0f;
        m_music_volume = 70.0f;
    }
    
    SoundManager::~SoundManager() {
        
    }
    void    SoundManager::Init(GHL::Sound* snd, Resources* res) {
        m_sound = snd;
        m_resources = res;
    }
    
    void SoundManager::Deinit() {
        ClearCache();
        m_fade_ins.clear();
        m_fade_outs.clear();
        m_music.reset();
        m_fade_outs_musics.clear();
        m_sound = 0;
        m_resources = 0;
    }
    
    void SoundManager::ClearCache() {
        for (SoundsMap::iterator it = m_sounds.begin();it!=m_sounds.end();++it) {
            it->second->Release();
        }
        m_sounds.clear();
    }
    
    void    SoundManager::SetSoundsDir(const char* dir) {
        m_sounds_dir = dir;
        if (m_sounds_dir.length() && m_sounds_dir[m_sounds_dir.length()-1]!='/') {
            m_sounds_dir += "/";
        }
    }
    
    void SoundManager::SetMusicVolume(float v) {
        m_music_volume = v;
        if (m_music) {
            m_music->SetVolume(m_music_volume);
        } else if (m_music_volume > SILENCE) {
            if (!m_last_music.empty()) {
                PlayMusicEx(m_last_music.c_str(), true, 0.0f,0.0f);
            }
        }
    }
    
    SoundPtr SoundManager::GetSound(const char* filename) {
        if (!m_resources) return SoundPtr();
        
        sb::string fullname = m_sounds_dir + filename;
        sb::map<sb::string, SoundPtr>::const_iterator it = m_sounds.find(fullname);
        if (it!=m_sounds.end())
            return it->second;
        
        GHL::SoundEffect* effect = 0;
        if (m_sound) {
            
            GHL::DataStream*  ds = m_resources->OpenFile(fullname.c_str());
            if (!ds) {
                LogWarning(MODULE) << "not found file " << fullname;
                return SoundPtr();
            }
            GHL::SoundDecoder* decoder = GHL_CreateSoundDecoder(ds);
            ds->Release();
            if (!decoder) {
                return SoundPtr();
            }
        
            GHL::Data* data = decoder->GetAllSamples();
            
            if (!data) {
                decoder->Release();
                return SoundPtr();
            }
            
            effect = m_sound->CreateEffect(decoder->GetSampleType(), decoder->GetFrequency(), data);
            data->Release();
            decoder->Release();
            
        }
        
        SoundPtr res(new Sound(this,effect));
        m_sounds[fullname] = res;
        return res;
    }
    
    void    SoundManager::Update(float dt) {
        for (SoundsList::iterator it = m_fade_ins.begin();it!=m_fade_ins.end();) {
            if ((*it)->Update(dt))
                it = m_fade_ins.erase(it);
            else
                ++it;
        }
        for (SoundsList::iterator it = m_fade_outs.begin();it!=m_fade_outs.end();) {
            if ((*it)->Update(dt))
                it = m_fade_outs.erase(it);
            else
                ++it;
        }
        for (MusicsList::iterator it = m_fade_outs_musics.begin();it!=m_fade_outs_musics.end();) {
            if ((*it)->Update(dt))
                it = m_fade_outs_musics.erase(it);
            else
                ++it;
        }
        if (m_music && m_music->Update(dt)) {
            m_music.reset();
        }
    }
    
    void SoundManager::Pause() {
        if (!m_sound)
            return;
        if (m_music) {
            m_music->Pause();
        }
        for (MusicsList::iterator it = m_fade_outs_musics.begin();it!=m_fade_outs_musics.end();++it) {
            (*it)->Stop();
        }
        m_fade_outs_musics.clear();
    }
    void SoundManager::Resume() {
        if (!m_sound)
            return;
        if (m_music) {
            m_music->Resume();
        }
    }
    
    void SoundManager::PlayMusic(const char* filename,bool loop) {
        PlayMusicEx(filename, loop, 3.0f, 3.0f);
    }
    
    void SoundManager::PlayMusicEx(const char *filename, bool loop, float fade_in, float fade_out_current) {
        if (!m_resources)
            return;
        if (!m_sound)
            return;
        if (m_music) {
            if (fade_out_current > 0.0f) {
                m_music->FadeOut(fade_out_current);
                m_fade_outs_musics.push_back(m_music);
            } else {
                m_music->Stop();
            }
            m_music.reset();
        }
        if (m_music_volume <= SILENCE) {
            
        } else {
            GHL::MusicInstance* music = open_music(filename);
            if (music) {
                m_music.reset( new MusicInstance(music) );
            }
        }
       
        if (m_music) {
            m_music->SetVolume(m_music_volume);
            if (fade_in > 0.0f) {
                m_music->FadeIn(fade_in);
            }
            m_music->Play(loop);
        }
        if (loop) {
            m_last_music = filename;
        } else {
            m_last_music.clear();
        }
    }
    
    GHL::MusicInstance* SoundManager::open_music(const char* filename) {
        sb::string fullname = m_sounds_dir + filename;
        GHL::DataStream* ds = m_resources->OpenFile(fullname.c_str());
        if (!ds) {
            return 0;
        }
        GHL::MusicInstance* music = m_sound->OpenMusic(ds);
        if (!music) {
            SB_LOGD("failed open music " << filename);
        }
        ds->Release();
        return music;
    }
}

