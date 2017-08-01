/*
 *  sb_container_transform3d.h
 *  SR
 *
 *  Created by Андрей Куницын on 11.05.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_TRANSFORM3D_H
#define SB_CONTAINER_TRANSFORM3D_H

#include "sb_container.h"
#include "sb_matrix4.h"

namespace Sandbox {
	
	class ContainerTransform3d : public Container {
        SB_META_OBJECT
   public:
		ContainerTransform3d();
		~ContainerTransform3d();
		
		void Draw(Graphics& g) const SB_OVERRIDE;
		
		void SetProjectionMatrix(const Matrix4f& m) { m_projection = m; }
		const Matrix4f& GetProjectionMatrix() const { return m_projection;}
		void SetViewMatrix(const Matrix4f& m) { m_view = m; }
		const Matrix4f& GetViewMatrix() const { return m_view;}
	private:
		Matrix4f	m_projection;
		Matrix4f	m_view;
	};
	typedef sb::intrusive_ptr<ContainerTransform3d> ContainerTransform3dPtr;
}

#endif /*SB_CONTAINER_TRANSFORM3D_H*/
