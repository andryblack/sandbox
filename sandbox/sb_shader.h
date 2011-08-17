/*
 *  sb_shader.h
 *  SR
 *
 *  Created by Андрей Куницын on 13.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_SHADER_H
#define SB_SHADER_H

#include "sb_shared_ptr.h"
#include <map>
#include <vector>
#include <string>

namespace GHL {
	struct ShaderProgram;
	struct Render;
	struct ShaderUniform;
}
namespace Sandbox {
	
	class ShaderUniform {
	public:
		virtual void DoSet() = 0;
	protected:
		explicit ShaderUniform(GHL::ShaderUniform* uniform) : m_uniform(uniform) {}
		GHL::ShaderUniform* m_uniform;
	};
	typedef shared_ptr<ShaderUniform> ShaderUniformPtr;
	
	class ShaderFloatUniform : public ShaderUniform {
	public:
		explicit ShaderFloatUniform(GHL::ShaderUniform* uniform) : ShaderUniform(uniform),m_value(1.0f) {}
		virtual void DoSet();
		void SetValue(float v) { m_value = v;}
	private:
		float m_value;
	};
	class Shader {
	public:
		explicit Shader( GHL::ShaderProgram* prg);
		~Shader();
		void Set(GHL::Render* r);
		shared_ptr<ShaderFloatUniform> GetFloatUniform(const char* name);
	private:
		GHL::ShaderProgram* m_program;
		std::vector<ShaderUniformPtr> m_uniforms;
		void SetUniforms();
		std::map<std::string,shared_ptr<ShaderFloatUniform> > m_float_uniforms;
	};
	
	typedef shared_ptr<Shader> ShaderPtr;
}

#endif /*SB_SADER_H*/