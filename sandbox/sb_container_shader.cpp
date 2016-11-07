/*
 *  sb_container_shader.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 13.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container_shader.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::ContainerShader, Sandbox::Container)

namespace Sandbox {

	ContainerShader::ContainerShader() : m_enabled(true){
        m_mask_tr.reset(new TransformModificator() );
	}
	
	ContainerShader::~ContainerShader() {
	}
	
	void ContainerShader::Draw(Graphics& g) const {
		if (m_enabled) {
			ShaderPtr ss = g.GetShader();
			g.SetShader(m_shader);
            TexturePtr crnt_mask = g.GetMaskTexture();
            if (m_texture) {
                g.SetMaskTexture(m_texture);
                if (m_mask_tr) {
                    Transform2d tr;
                    m_mask_tr->Apply(tr);
                    g.SetMaskTransform(tr);
                }
            }
            Container::Draw(g);
			g.SetShader(ss);
            if (m_texture) {
                g.SetMaskTexture(crnt_mask);
            }
		} else {
			Container::Draw(g);
		}
	}
	
	
}
