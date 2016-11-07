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
#include "sb_texture.h"

namespace Sandbox {
	
	class ContainerShader : public Container {
        SB_META_OBJECT
	public:
        ContainerShader();
		~ContainerShader();
		
		void Draw(Graphics& g) const;
		
		void SetShader(const ShaderPtr& sh) { m_shader = sh;}
		const ShaderPtr& GetShader() const { return m_shader;}
        
        void SetTexture(const TexturePtr& texture) { m_texture = texture; }
        const TexturePtr& GetTexture() const { return m_texture; }
		
		void SetEnabled(bool e) { m_enabled = e;}
		bool GetEnabled() const { return m_enabled;}
        
        const TransformModificatorPtr& GetMaskTransform() const { return m_mask_tr; }
	private:
		bool	m_enabled;
		ShaderPtr m_shader;
        TexturePtr m_texture;
        TransformModificatorPtr    m_mask_tr;
	};
	
}

#endif /*SB_CONTAINER_SHADER_H*/
