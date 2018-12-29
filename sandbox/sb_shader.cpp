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

SB_META_DECLARE_OBJECT(Sandbox::ShaderUniform, void)
SB_META_DECLARE_OBJECT(Sandbox::ShaderFloatUniform, Sandbox::ShaderUniform)
SB_META_DECLARE_OBJECT(Sandbox::ShaderMat4Uniform, Sandbox::ShaderUniform)
SB_META_DECLARE_OBJECT(Sandbox::Shader, void)

SB_META_DECLARE_OBJECT_T(Sandbox,ShaderVec2Uniform, ShaderUniform)
SB_META_DECLARE_OBJECT_T(Sandbox,ShaderVec3Uniform, ShaderUniform)
SB_META_DECLARE_OBJECT(Sandbox::ShaderColorUniform, Sandbox::ShaderUniform)


namespace Sandbox {

	Shader::Shader( GHL::ShaderProgram* prg) : m_program(prg) {
	}
	
	Shader::~Shader() {
		if (m_program) {
			m_program->Release();
		}
	}
    
    sb::intrusive_ptr<Shader> Shader::Clone() {
        ShaderPtr res(new Shader(m_program));
        if (m_program) m_program->AddRef();
        return res;
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
    template <>
    void ShaderVec2Uniform::DoSet() {
		if (m_uniform) m_uniform->SetValueFloat2(m_value.x,m_value.y);
	}
    template <>
    void ShaderVec3Uniform::DoSet() {
        if (m_uniform) m_uniform->SetValueFloat3(m_value.x,m_value.y,m_value.z);
    }
    void ShaderMat4Uniform::DoSet() {
        if (m_uniform) m_uniform->SetValueMatrix(m_value.matrix);
    }
   
    void ShaderColorUniform::DoSet() {
        if (m_uniform) m_uniform->SetValueFloat4(m_color.r,m_color.g,m_color.b,m_color.a);
    }
    
    GHL::ShaderUniform* Shader::GetRawUniform(const char* name) {
        return m_program ? m_program->GetUniform(name) : 0;
    }
	
    template <class T>
    sb::intrusive_ptr<ShaderUniform> Shader::GetUniform(const char* name) {
		sb::string sname = name;
		sb::map<sb::string,sb::intrusive_ptr<ShaderUniform> >::iterator it = m_uniforms_map.find(sname);
		if (it!=m_uniforms_map.end())
			return it->second;
        GHL::ShaderUniform* u = GetRawUniform(name);
		if (true) {
            sb::intrusive_ptr<T> fu(new T(u));
			m_uniforms_map.insert(std::make_pair(sname,fu));
			return fu;
		}
		return sb::intrusive_ptr<ShaderUniform>();
	}
    ShaderFloatUniformPtr Shader::GetFloatUniform(const char* name) {
        return ShaderFloatUniformPtr(meta::sb_dynamic_cast<ShaderFloatUniform>(GetUniform<ShaderFloatUniform>(name).get()));
    }
    ShaderVec2UniformPtr Shader::GetVec2Uniform(const char* name) {
        return ShaderVec2UniformPtr(meta::sb_dynamic_cast<ShaderVec2Uniform>(GetUniform<ShaderVec2Uniform>(name).get()));
    }
    ShaderVec3UniformPtr Shader::GetVec3Uniform(const char* name) {
        return ShaderVec3UniformPtr(meta::sb_dynamic_cast<ShaderVec3Uniform>(GetUniform<ShaderVec3Uniform>(name).get()));
    }
    ShaderMat4UniformPtr Shader::GetMat4Uniform(const char* name) {
        return ShaderMat4UniformPtr(meta::sb_dynamic_cast<ShaderMat4Uniform>(GetUniform<ShaderMat4Uniform>(name).get()));
    }
    ShaderColorUniformPtr Shader::GetColorUniform(const char* name) {
        return ShaderColorUniformPtr(meta::sb_dynamic_cast<ShaderColorUniform>(GetUniform<ShaderColorUniform>(name).get()));
    }
    
}
