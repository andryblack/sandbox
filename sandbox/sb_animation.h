/*
 *  sb_animation.h
 *  SR
 *
 *  Created by Андрей Куницын on 07.03.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_ANIMATION_H
#define SB_ANIMATION_H

#include "sb_sprite.h"
#include "sb_thread.h"
#include "sb_event.h"
#include <sbstd/sb_vector.h>

namespace Sandbox {
	
	class AnimationData {
	public:
		AnimationData();
		~AnimationData();
		
		void Reserve(size_t size);
		void AddFrame(const ImagePtr& img);
		
		size_t Frames() const { return m_images.size();}
		const ImagePtr& GetImage(size_t frame) const;
		
		void SetSpeed(float s) { m_speed = s;}
		float GetSpeed() const { return m_speed;}
		
		size_t GetLoopFrame() const { return m_loop_frame;}
		void SetLoopFrame(size_t f) { m_loop_frame = f;}
	private:
		sb::vector<ImagePtr> m_images;
		size_t	m_loop_frame;
		float	m_speed;
	};
	typedef sb::shared_ptr<AnimationData> AnimationDataPtr;
	
	class Animation : public Thread {
        SB_META_OBJECT
	public:
		explicit Animation(const AnimationDataPtr& data);
		~Animation();
		
		void Start(bool loop);
		void Stop();
		
		bool Complete() const;
		bool PlayedOnce() const;
		
		void AddSprite(const SpritePtr& spr);
		void ClearSprites();
		
		void SetEndEvent(const EventPtr& e) { m_end_event = e;}
        const EventPtr& GetEndEvent() const { return m_end_event; }
		
		virtual bool Update(float dt);
	private:
		void SetImages(const ImagePtr& img);
		AnimationDataPtr		m_data;
		std::vector<SpritePtr>	m_sprites;
		bool	m_started;
		bool	m_loop;
		float	m_time;
		size_t	m_frame;
		bool	m_played_once;
		EventPtr	m_end_event;
	};
	
}

#endif /*SB_ANIMATION_H*/
