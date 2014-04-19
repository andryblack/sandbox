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
		for (sb::map<sb::string,ShaderUniformPtr>::iterator it=m_uniforms_map.begin();it!=m_uniforms_map.end();it++)
			it->second->DoSet();
	}
	
	void ShaderFloatUniform::DoSet() {
		if (m_uniform) m_uniform->SetValueFloat(m_value);
	}
    void ShaderVec2Uniform::DoSet() {
		if (m_uniform) m_uniform->SetValueFloat2(m_value.x,m_value.y);
	}
	
    template <class T>
    sb::shared_ptr<ShaderUniform> Shader::GetUniform(const char* name) {
		sb::string sname = name;
		sb::map<sb::string,sb::shared_ptr<ShaderUniform> >::iterator it = m_uniforms_map.find(sname);
		if (it!=m_uniforms_map.end())
			return it->second;
		GHL::ShaderUniform* u = m_program ? m_program->GetUniform(name) : 0;
		if (true) {
            sb::shared_ptr<T> fu = sb::make_shared<T>(u);
			m_uniforms_map.insert(std::make_pair(sname,fu));
			return fu;
		}
		return sb::shared_ptr<ShaderUniform>();
	}
    ShaderFloatUniformPtr Shader::GetFloatUniform(const char* name) {
        return sb::dynamic_pointer_cast<ShaderFloatUniform>(GetUniform<ShaderFloatUniform>(name));
    }
    ShaderVec2UniformPtr Shader::GetVec2Uniform(const char* name) {
        return sb::dynamic_pointer_cast<ShaderVec2Uniform>(GetUniform<ShaderVec2Uniform>(name));
    }
    
    class ShaderIntUniform : public ShaderUniform {
	public:
		explicit ShaderIntUniform(GHL::ShaderUniform* uniform) : ShaderUniform(uniform),m_value(0) {}
		virtual void DoSet() {
            if (m_uniform) m_uniform->SetValueInt(m_value);
        }
		void SetValue(int v) { m_value = v;}
	private:
		int m_value;
	};
    
    void Shader::SetTextureStage(const char* uniform_name,int stage) {
        sb::shared_ptr<ShaderIntUniform> uniform = sb::dynamic_pointer_cast<ShaderIntUniform>(GetUniform<ShaderIntUniform>(uniform_name));
        if (uniform) {
            uniform->SetValue(stage);
        }
    }
}
