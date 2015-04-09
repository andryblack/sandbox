/*
 *  sb_animation.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 08.03.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_animation.h"

SB_META_DECLARE_OBJECT(Sandbox::Animation, Sandbox::Thread)

namespace Sandbox {
	
    
	AnimationData::AnimationData() {
		m_speed = 10.0f;
		m_loop_frame = 0;
	}
	
	AnimationData::~AnimationData() {
	}
	void AnimationData::Reserve(size_t size) {
		m_images.reserve(size);
	}
	void AnimationData::AddFrame(const ImagePtr& img) {
		m_images.push_back(img);
	}
	
	const ImagePtr& AnimationData::GetImage(size_t frame) const {
		static const ImagePtr empty;
		if (m_images.empty()) return empty;
		if (frame < m_images.size()) return m_images[frame];
		return m_images.back();
	}
	
	
	
	Animation::Animation(const AnimationDataPtr& data) : m_data(data) {
		m_started = false;
		m_frame = 0;
		m_played_once = false;
		m_loop = false;
	}
	
	Animation::~Animation() {
	}
	
	void Animation::Start(bool loop) {
		m_played_once = false;
		m_started = m_data && m_data->Frames();
		m_loop = loop;
		m_frame = 0;
		m_time = 0;
		if (!m_started) return;
		SetImages();
	}
	
	void Animation::Stop() {
		m_started = false;
	}
	
	bool Animation::Complete() const {
		return !m_started;
	}
	
	bool Animation::PlayedOnce() const {
		return !m_started || m_played_once;
	}
	
	void Animation::AddSprite(const SpritePtr& spr) {
		m_sprites.push_back(spr);
		if (m_started) {
			m_sprites.back()->SetImage(m_data->GetImage(m_frame));
		}
	}
	
	void Animation::ClearSprites() {
		m_sprites.clear();
	}
	
    void Animation::AddSync( const sb::intrusive_ptr<Animation>& anim ) {
        sb_assert(anim);
        sb_assert(anim.get()!=this);
        m_sync.push_back(anim);
    }
    
	bool Animation::Update(float dt) {
		if (!m_started) return true;
		m_time+=dt;
		const float frame_time = 1.0f / m_data->GetSpeed();
		size_t frames = static_cast<size_t>(m_time / frame_time );
		if (frames) {
			const size_t frames_amount = m_data->Frames();
			if ( (m_frame+frames) >= frames_amount ) {
				if (!m_loop) {
					//sb_assert( frames_amount >= (m_frame+1) );
					m_frame = frames_amount - 1;
					SetImages();
					m_started = false;
				} 
				if (m_end_event)
					m_end_event->Emmit();
				m_played_once = true;
			}
			if (m_started) {
                m_frame = ( m_frame+frames);
                if (m_frame >= frames_amount) {
                    m_frame = m_data->GetLoopFrame() + ((m_frame-m_data->GetLoopFrame()) % (m_data->Frames()-m_data->GetLoopFrame()));
                }
                SetImages();
				m_time -=  frame_time * float(frames);
			}
		}
		return Complete();
	}
	
	void Animation::SetImages() {
        ImagePtr img = m_data->GetImage(m_frame);
		for (size_t i=0;i<m_sprites.size();i++) {
			m_sprites[i]->SetImage(img);
		}
        for (size_t i=0;i<m_sync.size();++i) {
            m_sync[i]->m_frame = m_frame;
            m_sync[i]->SetImages();
        }
	}
}

