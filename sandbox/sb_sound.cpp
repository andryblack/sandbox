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
    
    void    Sound::Play() {
        if (!m_effect) return;
        m_mgr->m_sound->PlayEffect(m_effect,m_mgr->m_sounds_volume,0.0f,0);
    }

    void    Sound::PlayEx(float vol,float pan) {
        if (!m_effect) return;
        m_mgr->m_sound->PlayEffect(m_effect,m_mgr->m_sounds_volume*vol,pan*100.0f,0);
    }
    
    SoundInstancePtr    Sound::PlayExControl(float fadeIn,float vol,float pan) {
        if (!m_effect) return SoundInstancePtr(new SoundInstance(SoundPtr(this),static_cast<GHL::SoundInstance*>(0),0.0f));
        float initialVol = vol;
        if (fadeIn!=0.0f) {
            initialVol = 0;
        }
        initialVol *= m_mgr->m_sounds_volume;
        GHL::SoundInstance* instance = 0;
        m_mgr->m_sound->PlayEffect(m_effect,initialVol,pan*100.0f,&instance);
        SoundInstancePtr res(new SoundInstance(SoundPtr(this),instance,initialVol));
        if (fadeIn!=0.0f) {
            res->FadeIn(vol*m_mgr->m_sounds_volume, fadeIn);
            m_mgr->m_fade_ins.push_back(res);
        }
        return res;
    }
    
    SoundManager::SoundManager( ) : m_sound(0),m_resources(0),m_music(0) {
        m_sounds_volume = 70.0f;
        m_music_volume = 70.0f;
    }
    
    SoundManager::~SoundManager() {
        if (m_music) {
            m_music->Release();
            m_music = 0;
        }
    }
    void    SoundManager::Init(GHL::Sound* snd, Resources* res) {
        m_sound = snd;
        m_resources = res;
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
    }
    
    void SoundManager::PlayMusic(const char* filename,bool loop) {
        if (!m_resources)
            return;
        if (!m_sound)
            return;
        if (m_music) {
            m_music->Stop();
            m_music->Release();
            m_music = 0;
        }
        sb::string fullname = m_sounds_dir + filename;
        GHL::DataStream* ds = m_resources->OpenFile(fullname.c_str());
        if (!ds) {
            return;
        }
        m_music = m_sound->OpenMusic(ds);
        ds->Release();
        if (m_music) {
            m_music->SetVolume(m_music_volume);
            m_music->Play(loop);
        }
    }
    
}

