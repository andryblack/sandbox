/*
 *  sb_shader.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 13.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_shader.h"

#include <ghl_shader.h>
#include <ghl_render.h>

namespace Sandbox {

	Shader::Shader( GHL::ShaderProgram* prg) : m_program(prg) {
	}
	
	Shader::~Shader() {
		if (m_program) {
			m_program->Release();
		}
	}
	
	void Shader::Set(GHL::Render* r) {
		r->SetShader(m_program);
		SetUniforms();
	}
	
	void Shader::SetUniforms() {
		for (std::vector<ShaderUniformPtr>::iterator it=m_uniforms.begin();it!=m_uniforms.end();it++)
			(*it)->DoSet();
	}
	
	void ShaderFloatUniform::DoSet() {
		if (m_uniform) m_uniform->SetValueFloat(m_value);
	}
	
    sb::shared_ptr<ShaderFloatUniform> Shader::GetFloatUniform(const char* name) {
		sb::string sname = name;
		sb::map<std::string,sb::shared_ptr<ShaderFloatUniform> >::iterator it = m_float_uniforms.find(sname);
		if (it!=m_float_uniforms.end())
			return it->second;
		GHL::ShaderUniform* u = m_program ? m_program->GetUniform(name) : 0;
		if (true) {
            sb::shared_ptr<ShaderFloatUniform> fu = sb::make_shared<ShaderFloatUniform>(u);
			m_float_uniforms.insert(std::make_pair(sname,fu));
			m_uniforms.push_back(fu);
			return fu;
		}
		return sb::shared_ptr<ShaderFloatUniform>();
	}
}
