/*
 *  sb_container_shader.h
 *  SR
 *
 *  Created by Андрей Куницын on 13.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_SHADER_H
#define SB_CONTAINER_SHADER_H

#include "sb_container.h"
#include "sb_shader.h"

namespace Sandbox {
	
	class ContainerShader : public Container {
	public:
		ContainerShader();
		~ContainerShader();
		
		void Draw(Graphics& g) const;
		
		void SetShader(const ShaderPtr& sh) { m_shader = sh;}
		const ShaderPtr& GetShader() const { return m_shader;}
		
		void SetEnabled(bool e) { m_enabled = e;}
		bool GetEnabled() const { return m_enabled;}
	private:
		bool	m_enabled;
		ShaderPtr m_shader;
	};
	
}

#endif /*SB_CONTAINER_SHADER_H*/
