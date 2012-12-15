/*
 *  sb_container_transform.h
 *  SR
 *
 *  Created by Андрей Куницын on 12.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_TRANSFORM_H
#define SB_CONTAINER_TRANSFORM_H

#include "sb_container.h"
#include "sb_vector2.h"

namespace Sandbox {
	
	class ContainerTransform : public Container {
        SB_META_OBJECT
	public:
      	ContainerTransform();
		~ContainerTransform();
		
		void Draw(Graphics& g) const;
        
        /// self mouse handling implementation
        virtual bool HandleTouch( const TouchInfo& touch );
		
		void SetTranslate(const Vector2f& tr) { m_translate = tr; }
		const Vector2f& GetTranslate() const { return m_translate;}
		void SetScale(float s) { m_scale_x = m_scale_y = s;}
        float GetScale() const { return (m_scale_x + m_scale_y)*0.5f; }
		void SetScaleX(float s) { m_scale_x = s;}
		float GetScaleX() const { return m_scale_x;}
		void SetScaleY(float s) { m_scale_y = s;}
		float GetScaleY() const { return m_scale_y;}
		void SetAngle(float a) { m_angle = a;}
		float GetAngle() const { return m_angle;}
	private:
		Vector2f	m_translate;
		float		m_scale_x;
		float		m_scale_y;
		float		m_angle;
	};
	typedef sb::shared_ptr<ContainerTransform> ContainerTransformPtr;
}

#endif /*SB_CONTAINER_TRANSFORM_H*/
