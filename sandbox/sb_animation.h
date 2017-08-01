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
#include "sb_signal.h"
#include <sbstd/sb_vector.h>

namespace Sandbox {
	
	class AnimationData : public sb::ref_countered_base {
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
	typedef sb::intrusive_ptr<AnimationData> AnimationDataPtr;
	
	class Animation : public Thread {
        SB_META_OBJECT
	public:
		explicit Animation(const AnimationDataPtr& data);
		~Animation();
		
		void Start(bool loop);
		void Stop();
        
        size_t GetFrame() const { return m_frame; }
        void SetFrame(size_t frame);
        float GetFrameTime() const { return m_time; }
        void SetFrameTime(float f) { m_time = f; }
        
        void Randomize();
		
		bool Complete() const;
		bool PlayedOnce() const;
		
		void AddSprite(const SpritePtr& spr);
		void ClearSprites();
		
		void SetOnEnd(const SignalPtr& e) { m_end_signal = e;}
        const SignalPtr& GetOnEnd() const { return m_end_signal; }
		
        void AddSync( const sb::intrusive_ptr<Animation>& anim );
		virtual bool Update(float dt) SB_OVERRIDE;
	private:
		void SetImages();
		AnimationDataPtr		m_data;
		std::vector<SpritePtr>	m_sprites;
        std::vector<sb::intrusive_ptr<Animation> > m_sync;
		bool	m_started;
		bool	m_loop;
		float	m_time;
		size_t	m_frame;
		bool	m_played_once;
		SignalPtr	m_end_signal;
	};
	
}

#endif /*SB_ANIMATION_H*/
